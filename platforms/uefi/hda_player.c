#include <uefi_shim.h>
#include "hda_regs.h"
#include "hda_discovery.h"

// ---------------------------------------------------------------------------
// PciIo register access helpers — all HDA registers in BAR 0
// ---------------------------------------------------------------------------

EFI_PCI_IO_PROTOCOL *gPciIo = NULL;

UINT8 EFIAPI HdaRead8 (UINTN Off) {
  UINT8 V = 0;
  uefi_call_wrapper(gPciIo->Mem.Read, 6, gPciIo, EfiPciIoWidthUint8, 0, (UINT64)Off, 1, &V);
  return V;
}
UINT16 EFIAPI HdaRead16 (UINTN Off) {
  UINT16 V = 0;
  uefi_call_wrapper(gPciIo->Mem.Read, 6, gPciIo, EfiPciIoWidthUint16, 0, (UINT64)Off, 1, &V);
  return V;
}
UINT32 EFIAPI HdaRead32 (UINTN Off) {
  UINT32 V = 0;
  uefi_call_wrapper(gPciIo->Mem.Read, 6, gPciIo, EfiPciIoWidthUint32, 0, (UINT64)Off, 1, &V);
  return V;
}
VOID EFIAPI HdaWrite8 (UINTN Off, UINT8 V) {
  uefi_call_wrapper(gPciIo->Mem.Write, 6, gPciIo, EfiPciIoWidthUint8, 0, (UINT64)Off, 1, &V);
}
VOID EFIAPI HdaWrite16 (UINTN Off, UINT16 V) {
  uefi_call_wrapper(gPciIo->Mem.Write, 6, gPciIo, EfiPciIoWidthUint16, 0, (UINT64)Off, 1, &V);
}
VOID EFIAPI HdaWrite32 (UINTN Off, UINT32 V) {
  uefi_call_wrapper(gPciIo->Mem.Write, 6, gPciIo, EfiPciIoWidthUint32, 0, (UINT64)Off, 1, &V);
}

// ---------------------------------------------------------------------------
// CORB/RIRB state
// ---------------------------------------------------------------------------

STATIC VOID                  *mCorbRirbBuffer  = NULL;
STATIC VOID                  *mCorbRirbMapping = NULL;
STATIC EFI_PHYSICAL_ADDRESS   mCorbRirbBusAddr = 0;
STATIC UINTN                   mCorbRirbPages   = 0;
STATIC UINT32                 *mCorb            = NULL;
STATIC UINT64                 *mRirb            = NULL;
STATIC UINT16                  mCorbWp          = 0;

// ---------------------------------------------------------------------------
// Full controller reset — CRST cycle, wait for codec
// ---------------------------------------------------------------------------

EFI_STATUS EFIAPI HdaControllerReset (VOID)
{
  UINTN   Timeout;
  UINT16  StateChange;

  Print (L"  Resetting HDA controller...\n");

  HdaWrite8 (HDA_CORBCTL, 0);
  HdaWrite8 (HDA_RIRBCTL, 0);
  uefi_call_wrapper(BS->Stall, 1, 2000);

  Print (L"  Asserting CRST=0...\n");
  HdaWrite32 (HDA_GCTL, 0);
  Timeout = 500;
  while ((HdaRead32 (HDA_GCTL) & HDA_GCTL_CRST) && --Timeout)
    uefi_call_wrapper(BS->Stall, 1, 1000);
  if (!Timeout) { Print (L"  ERROR: CRST assert failed\n"); return EFI_TIMEOUT; }

  uefi_call_wrapper(BS->Stall, 1, 2000);

  Print (L"  Deasserting CRST=1...\n");
  HdaWrite32 (HDA_GCTL, HDA_GCTL_CRST);
  Timeout = 500;
  while (!(HdaRead32 (HDA_GCTL) & HDA_GCTL_CRST) && --Timeout)
    uefi_call_wrapper(BS->Stall, 1, 1000);
  if (!Timeout) { Print (L"  ERROR: CRST deassert failed\n"); return EFI_TIMEOUT; }

  Print (L"  Waiting for codec...\n");
  uefi_call_wrapper(BS->Stall, 1, 150000);   // 150 ms

  StateChange = HdaRead16 (HDA_STATESTS);
  Print (L"  GCTL=0x%08x STATESTS=0x%04x GCAP=0x%04x\n",
         HdaRead32 (HDA_GCTL), StateChange, HdaRead16 (HDA_GCAP));

  if (StateChange == 0)
    Print (L"  WARNING: no codec in STATESTS — will probe all slots\n");

  return EFI_SUCCESS;
}

// ---------------------------------------------------------------------------
// CORB/RIRB init via PciIo AllocateBuffer + Map
// ---------------------------------------------------------------------------

EFI_STATUS EFIAPI HdaInitCorbRirb (VOID)
{
  EFI_STATUS            Status;
  UINTN                 Timeout;
  UINTN                 BufBytes = EFI_PAGE_SIZE;
  UINTN                 Mapped;
  UINT8                 SizeCap;

  mCorbRirbPages = EFI_SIZE_TO_PAGES (BufBytes);

  Status = uefi_call_wrapper(gPciIo->AllocateBuffer, 6, gPciIo, AllocateAnyPages,
                                   EfiBootServicesData, mCorbRirbPages,
                                   &mCorbRirbBuffer, 0);
  if (EFI_ERROR (Status)) {
    Print (L"  ERROR: AllocateBuffer: %r\n", Status);
    return Status;
  }
  memset (mCorbRirbBuffer, 0, BufBytes);

  Mapped = BufBytes;
  Status = uefi_call_wrapper(gPciIo->Map, 6, gPciIo, EfiPciIoOperationBusMasterCommonBuffer,
                        mCorbRirbBuffer, &Mapped,
                        &mCorbRirbBusAddr, &mCorbRirbMapping);
  if (EFI_ERROR (Status)) {
    Print (L"  ERROR: Map: %r\n", Status);
    uefi_call_wrapper(gPciIo->FreeBuffer, 3, gPciIo, mCorbRirbPages, mCorbRirbBuffer);
    return Status;
  }

  mCorb = (UINT32 *)mCorbRirbBuffer;
  mRirb = (UINT64 *)((UINT8 *)mCorbRirbBuffer + 2048);

  Print (L"  DMA buf: virt=0x%lx bus=0x%lx\n", (UINT64)(UINTN)mCorbRirbBuffer, (UINT64)mCorbRirbBusAddr);

  // ---- CORB ----
  HdaWrite8 (HDA_CORBCTL, 0);
  Timeout = 200; while ((HdaRead8(HDA_CORBCTL) & HDA_CORBCTL_RUN) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);

  SizeCap = HdaRead8 (HDA_CORBSIZE);
  if      (SizeCap & BIT6) HdaWrite8 (HDA_CORBSIZE, 0x02);
  else if (SizeCap & BIT5) HdaWrite8 (HDA_CORBSIZE, 0x01);
  else                     HdaWrite8 (HDA_CORBSIZE, 0x00);

  HdaWrite32 (HDA_CORBLBASE, (UINT32)( mCorbRirbBusAddr        & 0xFFFFFF80));
  HdaWrite32 (HDA_CORBUBASE, (UINT32)((mCorbRirbBusAddr) >> 32));

  HdaWrite16 (HDA_CORBRP, HDA_CORBRP_RST);
  Timeout = 200; while (!(HdaRead16(HDA_CORBRP) & HDA_CORBRP_RST) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);
  HdaWrite16 (HDA_CORBRP, 0);
  Timeout = 200; while  ((HdaRead16(HDA_CORBRP) & HDA_CORBRP_RST) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);
  uefi_call_wrapper(BS->Stall, 1, 2000);

  HdaWrite16 (HDA_CORBWP, 0);
  mCorbWp = 0;

  HdaWrite32 (HDA_INTCTL, BIT31 | BIT30);  // GIE + CIE
  HdaWrite8 (HDA_CORBCTL, HDA_CORBCTL_RUN);
  Timeout = 200; while (!(HdaRead8(HDA_CORBCTL) & HDA_CORBCTL_RUN) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);

  // ---- RIRB ----
  HdaWrite8 (HDA_RIRBCTL, 0);
  Timeout = 200; while ((HdaRead8(HDA_RIRBCTL) & HDA_RIRBCTL_RUN) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);

  SizeCap = HdaRead8 (HDA_RIRBSIZE);
  if      (SizeCap & BIT6) HdaWrite8 (HDA_RIRBSIZE, 0x02);
  else if (SizeCap & BIT5) HdaWrite8 (HDA_RIRBSIZE, 0x01);
  else                     HdaWrite8 (HDA_RIRBSIZE, 0x00);

  {
    EFI_PHYSICAL_ADDRESS RirbBus = mCorbRirbBusAddr + 2048;
    HdaWrite32 (HDA_RIRBLBASE, (UINT32)( RirbBus        & 0xFFFFFF80));
    HdaWrite32 (HDA_RIRBUBASE, (UINT32)((RirbBus) >> 32));
  }

  HdaWrite16 (HDA_RIRBWP, HDA_RIRBWP_RST);
  uefi_call_wrapper(BS->Stall, 1, 2000);

  HdaWrite16 (HDA_RINTCNT, 1);
  HdaWrite8  (HDA_RIRBCTL, HDA_RIRBCTL_RUN | BIT0);
  Timeout = 200; while (!(HdaRead8(HDA_RIRBCTL) & HDA_RIRBCTL_RUN) && --Timeout) uefi_call_wrapper(BS->Stall, 1, 1000);

  Print (L"  CORBCTL=0x%02x RIRBCTL=0x%02x\n", HdaRead8(HDA_CORBCTL), HdaRead8(HDA_RIRBCTL));
  Print (L"  CORBLBASE=0x%08x RIRBLBASE=0x%08x\n", HdaRead32(HDA_CORBLBASE), HdaRead32(HDA_RIRBLBASE));

  return EFI_SUCCESS;
}

// ---------------------------------------------------------------------------
// Send verb via CORB, poll RIRB for response
// ---------------------------------------------------------------------------

EFI_STATUS EFIAPI
HdaSendVerb (IN UINT32 Verb, OUT UINT32 *Response)
{
  UINTN   Timeout;
  UINT16  NewWp;
  UINT16  CurRirbWp;

  NewWp = (mCorbWp + 1) & 0xFF;
  mCorb[NewWp] = Verb;
  MemoryFence ();
  HdaWrite16 (HDA_CORBWP, NewWp);
  mCorbWp = NewWp;

  Timeout = 2000;
  while (--Timeout) {
    uefi_call_wrapper(BS->Stall, 1, 100);
    if (HdaRead8 (HDA_RIRBSTS) & BIT0)
      break;
  }

  if (!Timeout) {
    Print (L"  TIMEOUT verb=0x%08x corbwp=%d rirbsts=0x%02x corbrp=%d rirbwp=%d\n",
           Verb, NewWp, HdaRead8 (HDA_RIRBSTS), HdaRead16(HDA_CORBRP), HdaRead16(HDA_RIRBWP) & 0xFF);
    return EFI_TIMEOUT;
  }

  CurRirbWp = HdaRead16 (HDA_RIRBWP) & 0xFF;
  if (Response != NULL)
    *Response = (UINT32)(mRirb[CurRirbWp] & 0xFFFFFFFF);

  HdaWrite8 (HDA_RIRBSTS, BIT0);  // W1C
  return EFI_SUCCESS;
}

// ---------------------------------------------------------------------------
// PCI scan — find ICH9 HDA and enable bus-master + memory
// ---------------------------------------------------------------------------

EFI_STATUS EFIAPI HdaFindPciIo (UINTN Index) {
  EFI_STATUS S; UINTN N; EFI_HANDLE *H;
  EFI_PCI_IO_PROTOCOL *P; PCI_TYPE00 Hdr; UINT16 Cmd; UINTN i;
  UINT64 Supported=0, Attrs=0; UINTN Found=0;

  S = uefi_call_wrapper(BS->LocateHandleBuffer, 5, ByProtocol, &gEfiPciIoProtocolGuid, NULL, &N, &H);
  if (EFI_ERROR(S)) return S;
  S = EFI_NOT_FOUND;

  for (i=0; i<N; i++) {
    if (EFI_ERROR(uefi_call_wrapper(BS->HandleProtocol, 3, H[i], &gEfiPciIoProtocolGuid, (VOID**)&P))) continue;
    if (EFI_ERROR(uefi_call_wrapper(P->Pci.Read, 5, P, EfiPciIoWidthUint8, 0, sizeof(PCI_TYPE00), &Hdr))) continue;
    if (Hdr.Hdr.ClassCode[2] != 0x04 || Hdr.Hdr.ClassCode[1] != 0x03) continue;
    if (Found++ < Index) continue;

    Print(L"  Found HDA controller [%04x:%04x]\n", Hdr.Hdr.VendorId, Hdr.Hdr.DeviceId);

    uefi_call_wrapper(P->Pci.Read, 5, P, EfiPciIoWidthUint16, OFFSET_OF(PCI_TYPE00, Hdr.Command), 1, &Cmd);
    Cmd |= EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE;
    uefi_call_wrapper(P->Pci.Write, 5, P, EfiPciIoWidthUint16, OFFSET_OF(PCI_TYPE00, Hdr.Command), 1, &Cmd);

    uefi_call_wrapper(P->Attributes, 4, P, EfiPciIoAttributeOperationGet, 0, &Supported);
    Attrs = Supported & (EFI_PCI_IO_ATTRIBUTE_BUS_MASTER | EFI_PCI_IO_ATTRIBUTE_MEMORY);
    uefi_call_wrapper(P->Attributes, 4, P, EfiPciIoAttributeOperationEnable, Attrs, NULL);

    gPciIo = P; S = EFI_SUCCESS; break;
  }
  uefi_call_wrapper(BS->FreePool, 1, H);
  return S;
}

UINTN EFIAPI HdaGetFirstOutBase (VOID) {
  UINT16 Gcap = HdaRead16 (HDA_GCAP);
  UINTN  Iss  = (Gcap >> 8) & 0x0F;
  UINTN  Bss  = (Gcap >> 3) & 0x1F;
  return HDA_SD_BASE (Iss + Bss);
}

VOID EFIAPI HdaGetPciIds (UINT16 *Vendor, UINT16 *Device) {
  PCI_TYPE00 Hdr;
  *Vendor = *Device = 0xFFFF;
  if (gPciIo && !EFI_ERROR (uefi_call_wrapper(gPciIo->Pci.Read, 5, gPciIo, EfiPciIoWidthUint8, 0, sizeof (PCI_TYPE00), &Hdr))) {
    *Vendor = Hdr.Hdr.VendorId;
    *Device = Hdr.Hdr.DeviceId;
  }
}

UINT16 EFIAPI HdaReadStateSts (VOID) { return HdaRead16 (HDA_STATESTS); }
