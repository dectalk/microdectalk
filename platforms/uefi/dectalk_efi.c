#include <efi.h>
#include <efilib.h>
#include <libsmbios.h>
#include "hda_discovery.h"
#include "hda_regs.h"
#include "dectalk.h"
#include "epsonapi.h"

// ---------------------------------------------------------------------------
// HDA stream format: 48000 Hz, stereo, 16-bit PCM
// ---------------------------------------------------------------------------
#define HDA_FMT_DECTALK     0x0011
#define HDA_STREAM_TAG      1

// ---------------------------------------------------------------------------
// Speech pre-buffer
// ---------------------------------------------------------------------------
#define SPEECH_OUT_RATE     48000
#define SPEECH_IN_RATE      11025
#define SPEECH_MAX_SECONDS  600
#define SPEECH_BUF_SAMPLES  (SPEECH_OUT_RATE * SPEECH_MAX_SECONDS)

STATIC INT16  *gSpeechBuf    = NULL;
STATIC UINTN   gSpeechBufLen = 0;

// ---------------------------------------------------------------------------
// Resampler
// ---------------------------------------------------------------------------
STATIC INT32  gResamplePhase = 0;
STATIC INT16  gResamplePrev  = 0;

STATIC VOID
ResamplePush (INT16 *Src, UINTN Count)
{
  UINTN i;
  for (i = 0; i < (UINTN)Count; i++) {
    INT16 s1 = Src[i];
    while (gResamplePhase < SPEECH_OUT_RATE) {
      if (gSpeechBufLen < SPEECH_BUF_SAMPLES) {
        INT32 out = ((INT32)gResamplePrev * (SPEECH_OUT_RATE - gResamplePhase)
                   + (INT32)s1           *  gResamplePhase)
                  / SPEECH_OUT_RATE;
        gSpeechBuf[gSpeechBufLen++] = (INT16)out;
      }
      gResamplePhase += SPEECH_IN_RATE;
    }
    gResamplePhase -= SPEECH_OUT_RATE;
    gResamplePrev = s1;
  }
}

#define DECTALK_CHUNK  71
STATIC INT16  gDtBuf[2][DECTALK_CHUNK];
STATIC UINTN  gDtBufIdx = 0;

STATIC short *
DtAudioCallback (short *samples, long count, int flags)
{
  (void)flags;
  if (samples != NULL && count > 0)
    ResamplePush ((INT16 *)samples, (UINTN)count);
  gDtBufIdx ^= 1;
  return (short *)gDtBuf[gDtBufIdx];
}

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------
extern EFI_PCI_IO_PROTOCOL *gPciIo;
extern EFI_STATUS EFIAPI HdaFindPciIo (UINTN Index);
extern EFI_STATUS EFIAPI HdaControllerReset (VOID);
extern EFI_STATUS EFIAPI HdaInitCorbRirb (VOID);
extern EFI_STATUS EFIAPI HdaSendVerb (UINT32 Verb, UINT32 *Response);
extern UINTN      EFIAPI HdaGetFirstOutBase (VOID);
extern VOID       EFIAPI HdaGetPciIds (UINT16 *Vendor, UINT16 *Device);

// ---------------------------------------------------------------------------
// Audio target list
// ---------------------------------------------------------------------------
typedef struct {
  UINTN          CtrlIdx;
  UINT16         PciVendor;
  UINT16         PciDevice;
  HDA_AUDIO_PATH Path;
} HDA_TARGET;

#define MAX_TARGETS 16
STATIC HDA_TARGET gTargets[MAX_TARGETS];
STATIC UINTN      gTargetCount = 0;

STATIC CONST CHAR16 *gDevNames[] = {
  L"Line Out", L"Speaker", L"HP Out", L"CD",
  L"SPDIF Out", L"Digital Out", L"Modem Line", L"Modem HS",
};

STATIC CONST CHAR16 * DevName (UINT8 Dev) {
  if (Dev < (sizeof (gDevNames) / sizeof (gDevNames[0]))) return gDevNames[Dev];
  return L"Output";
}

STATIC VOID ScanTargets (VOID) {
  UINTN CtrlIdx;
  HDA_AUDIO_PATH Paths[MAX_TARGETS];
  UINTN Found, j;
  UINT16 Vendor, Device;
  gTargetCount = 0;
  Print (L"  Scanning audio outputs...\n");
  for (CtrlIdx = 0; gTargetCount < MAX_TARGETS; CtrlIdx++) {
    if (EFI_ERROR (HdaFindPciIo (CtrlIdx))) break;
    HdaGetPciIds (&Vendor, &Device);
    if (EFI_ERROR (HdaControllerReset ())) continue;
    if (EFI_ERROR (HdaInitCorbRirb ())) continue;
    Found = 0;
    HdaDiscover (Paths, MAX_TARGETS - gTargetCount, &Found);
    for (j = 0; j < Found && gTargetCount < MAX_TARGETS; j++) {
      gTargets[gTargetCount].CtrlIdx = CtrlIdx;
      gTargets[gTargetCount].PciVendor = Vendor;
      gTargets[gTargetCount].PciDevice = Device;
      gTargets[gTargetCount].Path = Paths[j];
      gTargetCount++;
    }
  }
  Print (L"  Found %u analog output(s)\n\n", gTargetCount);
}

STATIC UINTN SelectTarget (VOID) {
  UINTN i, Index;
  EFI_INPUT_KEY Key;
  if (gTargetCount == 0) {
    Print (L"  No analog audio outputs found. Press any key to rescan...\n");
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &Index);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    return MAX_TARGETS;
  }
  Print (L"Available audio outputs:\n");
  for (i = 0; i < gTargetCount; i++) {
    HDA_TARGET *T = &gTargets[i];
    Print (L"  [%u] [%04x:%04x] Codec %d - %s  (DAC=%d", i, (UINTN)T->PciVendor, (UINTN)T->PciDevice, (UINTN)T->Path.CodecAddr, DevName (T->Path.DefaultDevice), (UINTN)T->Path.DacNode);
    if (T->Path.MixNode) Print (L", Mix=%d", (UINTN)T->Path.MixNode);
    Print (L", Pin=%d)\n", (UINTN)T->Path.PinNode);
  }
  if (gTargetCount == 1) {
    Print (L"\n  Auto-selected output 0. Press F1 to rescan.\n");
    return 0;
  }
  Print (L"\n  Select [0-%u], or F1 to rescan: ", gTargetCount - 1);
  for (;;) {
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &Index);
    if (EFI_ERROR (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key))) continue;
    if (Key.ScanCode == 0x000B) { Print(L"\n"); return MAX_TARGETS; }
    if (Key.UnicodeChar >= L'0' && Key.UnicodeChar < (CHAR16)(L'0' + gTargetCount)) {
      i = Key.UnicodeChar - L'0'; Print (L"%c\n", Key.UnicodeChar); return i;
    }
  }
}

STATIC EFI_STATUS DtCodecInit (IN HDA_AUDIO_PATH *Cfg) {
  UINT8 Ca=Cfg->CodecAddr, Afg=Cfg->AfgNode, Dac=Cfg->DacNode, Mix=Cfg->MixNode, Pin=Cfg->PinNode;
  UINT32 Resp;
  Print (L"  DECtalk codec init (48kHz stereo)...\n");
  if (EFI_ERROR(HdaSendVerb(HDA_VERB(Ca,0,HDA_GET_PARAM,HDA_PARAM_VENDOR_ID), &Resp))) return EFI_DEVICE_ERROR;
  Print (L"  Codec: 0x%08x\n", Resp);
  HdaSendVerb (HDA_VERB(Ca,Afg,HDA_SET_POWER_STATE,0x00), NULL); uefi_call_wrapper(BS->Stall, 1, 10000);
  HdaSendVerb (HDA_VERB(Ca,Dac,HDA_SET_POWER_STATE,0x00), NULL);
  if (Mix) HdaSendVerb (HDA_VERB(Ca,Mix,HDA_SET_POWER_STATE,0x00), NULL);
  HdaSendVerb (HDA_VERB(Ca,Pin,HDA_SET_POWER_STATE,0x00), NULL);
  uefi_call_wrapper(BS->Stall, 1, 5000);
  HdaSendVerb (HDA_VERB16(Ca,Dac,HDA_SET_STREAM_FMT,HDA_FMT_DECTALK), NULL);
  HdaSendVerb (HDA_VERB(Ca,Dac,0x706,(HDA_STREAM_TAG<<4)|0x00), NULL);
  HdaSendVerb (HDA_VERB16(Ca,Dac,HDA_SET_AMP_GAIN,HDA_AMP_OUT_UNMUTE), NULL);
  if (Mix) {
    HdaSendVerb (HDA_VERB16(Ca,Mix,HDA_SET_AMP_GAIN,HDA_AMP_OUT_UNMUTE),   NULL);
    HdaSendVerb (HDA_VERB16(Ca,Mix,HDA_SET_AMP_GAIN,HDA_AMP_IN_UNMUTE(0)), NULL);
  }
  HdaSendVerb (HDA_VERB (Ca,Pin,HDA_SET_EAPD_BTL,0x02), NULL);
  HdaSendVerb (HDA_VERB(Ca,Pin,HDA_SET_PIN_CTRL,HDA_PIN_OUT_EN|HDA_PIN_HP_EN), NULL);
  HdaSendVerb (HDA_VERB16(Ca,Pin,HDA_SET_AMP_GAIN,HDA_AMP_OUT_UNMUTE), NULL);
  Print (L"  Codec ready.\n");
  return EFI_SUCCESS;
}

extern UINT8 EFIAPI HdaRead8 (UINTN O);
extern UINT32 EFIAPI HdaRead32 (UINTN O);
extern VOID EFIAPI HdaWrite8 (UINTN O, UINT8 V);
extern VOID EFIAPI HdaWrite16 (UINTN O, UINT16 V);
extern VOID EFIAPI HdaWrite32 (UINTN O, UINT32 V);

STATIC VOID FillDmaStereo (INT16 *Dst, UINTN PlayPos, UINTN Frames) {
  UINTN avail = (gSpeechBufLen > PlayPos) ? gSpeechBufLen - PlayPos : 0;
  UINTN n = (avail < Frames) ? avail : Frames;
  for (UINTN i=0; i<n; i++) { Dst[2*i] = gSpeechBuf[PlayPos+i]; Dst[2*i+1] = gSpeechBuf[PlayPos+i]; }
  if (n < Frames) memset(Dst + 2*n, 0, (Frames-n)*2*sizeof(INT16));
}

STATIC VOID HdaStreamReset (UINTN Sd) {
  UINTN T;
  HdaWrite8(Sd+HDA_SDCTL_OFF, HdaRead8(Sd+HDA_SDCTL_OFF)|HDA_SDCTL_SRST);
  T=500; while(!(HdaRead8(Sd+HDA_SDCTL_OFF)&HDA_SDCTL_SRST)&&--T) uefi_call_wrapper(BS->Stall, 1, 1000);
  uefi_call_wrapper(BS->Stall, 1, 1000);
  HdaWrite8(Sd+HDA_SDCTL_OFF, HdaRead8(Sd+HDA_SDCTL_OFF)&~HDA_SDCTL_SRST);
  T=500; while((HdaRead8(Sd+HDA_SDCTL_OFF)&HDA_SDCTL_SRST)&&--T) uefi_call_wrapper(BS->Stall, 1, 1000);
}

#pragma pack(1)
typedef struct { UINT64 Address; UINT32 Length; UINT32 Flags; } MY_HDA_BDL_ENTRY;
#pragma pack()

STATIC EFI_STATUS DtStreamSpeak (IN CHAR8 *Text) {
  EFI_STATUS Status;
  VOID *DmaVirt=NULL, *DmaMap=NULL, *BdlVirt=NULL, *BdlMap=NULL;
  EFI_PHYSICAL_ADDRESS DmaBus=0, BdlBus=0;
  UINTN DmaBytes, BdlBytes=EFI_PAGE_SIZE;
  MY_HDA_BDL_ENTRY *Bdl;
  UINTN TotalFrames;

  Print (L"  Synthesising: \"%a\"\n", Text);
  gResamplePhase = 0; gResamplePrev = 0; gSpeechBufLen = 0;
  TextToSpeechStart ((char *)Text, NULL, WAVE_FORMAT_1M16);
  TextToSpeechSync ();
  Print (L"  Synthesis done: %u samples at 48kHz (~%u ms)\n", (UINT32)gSpeechBufLen, (UINT32)(gSpeechBufLen / 48));

  if (gSpeechBufLen == 0) return EFI_SUCCESS;

  TotalFrames = gSpeechBufLen;
  DmaBytes = TotalFrames * 2 * sizeof(INT16);
  Status = uefi_call_wrapper(gPciIo->AllocateBuffer, 6, gPciIo, AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(DmaBytes), &DmaVirt, 0);
  if (EFI_ERROR(Status)) return Status;
  memset(DmaVirt, 0, DmaBytes);

  uefi_call_wrapper(gPciIo->Map, 6, gPciIo, EfiPciIoOperationBusMasterCommonBuffer, DmaVirt, &DmaBytes, &DmaBus, &DmaMap);
  FillDmaStereo (DmaVirt, 0, TotalFrames);
  MemoryFence();

  uefi_call_wrapper(gPciIo->AllocateBuffer, 6, gPciIo, AllocateAnyPages, EfiBootServicesData, 1, &BdlVirt, 0);
  memset(BdlVirt, 0, EFI_PAGE_SIZE);
  uefi_call_wrapper(gPciIo->Map, 6, gPciIo, EfiPciIoOperationBusMasterCommonBuffer, BdlVirt, &BdlBytes, &BdlBus, &BdlMap);
  Bdl = (MY_HDA_BDL_ENTRY *)BdlVirt;
  Bdl[0].Address = DmaBus; Bdl[0].Length = (UINT32)DmaBytes; Bdl[0].Flags = 1;
  MemoryFence();

  UINTN Sd = HdaGetFirstOutBase ();
  Print (L"  Output stream at BAR0+0x%x, size %u bytes\n", Sd, (UINT32)DmaBytes);
  HdaStreamReset (Sd);
  HdaWrite8 (Sd+HDA_SDSTS_OFF, 0x1C);
  HdaWrite32 (Sd+HDA_SDCBL_OFF, (UINT32)DmaBytes);
  HdaWrite16 (Sd+HDA_SDLVI_OFF, 0);
  HdaWrite16 (Sd+HDA_SDFMT_OFF, HDA_FMT_DECTALK);
  HdaWrite32 (Sd+HDA_SDBDPL_OFF, (UINT32)(BdlBus & 0xFFFFFFFF));
  HdaWrite32 (Sd+HDA_SDBDPU_OFF, (UINT32)(BdlBus >> 32));
  HdaWrite32 (Sd+HDA_SDCTL_OFF, HDA_SDCTL_STRM(HDA_STREAM_TAG) | BIT2 | HDA_SDCTL_TP);
  Print (L"  Playing...\n");
  uefi_call_wrapper(BS->Stall, 1, 1000);
  HdaWrite32 (Sd+HDA_SDCTL_OFF, HdaRead32(Sd+HDA_SDCTL_OFF) | HDA_SDCTL_RUN);

#define ENABLE_PLAYBACK_CANCEL
#ifdef ENABLE_PLAYBACK_CANCEL
  UINTN IterationLimit = (TotalFrames / 240) * 2; // 240 samples per 5ms iter, 2x limit
  UINTN IterationCount = 0;
  if (IterationLimit < 100) IterationLimit = 100;

  while (!(HdaRead8(Sd+HDA_SDSTS_OFF) & BIT2)) {
    EFI_INPUT_KEY Key;
    if (!EFI_ERROR (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key))) {
      if (Key.UnicodeChar == 0x03) { Print (L" [Canceled]"); break; }
    }
    if (++IterationCount > IterationLimit) { Print (L" [Timeout]"); break; }
    uefi_call_wrapper(BS->Stall, 1, 5000);
  }
#else
  while (!(HdaRead8(Sd+HDA_SDSTS_OFF) & BIT2)) uefi_call_wrapper(BS->Stall, 1, 5000);
#endif

  HdaWrite32 (Sd+HDA_SDCTL_OFF, 0); HdaWrite8 (Sd+HDA_SDSTS_OFF, BIT2);
  Print (L"  Done speaking.\n");

  uefi_call_wrapper(gPciIo->Unmap, 2, gPciIo, BdlMap);
  uefi_call_wrapper(gPciIo->FreeBuffer, 3, gPciIo, 1, BdlVirt);
  uefi_call_wrapper(gPciIo->Unmap, 2, gPciIo, DmaMap);
  uefi_call_wrapper(gPciIo->FreeBuffer, 3, gPciIo, EFI_SIZE_TO_PAGES(DmaBytes), DmaVirt);
  return EFI_SUCCESS;
}

STATIC BOOLEAN IsDellMachine (VOID) {
  SMBIOS_STRUCTURE_TABLE *Smbios;
  if (LibGetSystemConfigurationTable(&SMBIOSTableGuid, (VOID**)&Smbios)) return FALSE;
  UINT8 *Ptr = (UINT8 *)(UINTN)Smbios->TableAddress;
  for (UINTN i=0; i<Smbios->NumberOfSmbiosStructures; i++) {
    SMBIOS_HEADER *Hdr = (SMBIOS_HEADER *)Ptr;
    if (Hdr->Type == 1) { // System Info
      char *Str = (char *)Ptr + Hdr->Length;
      UINT8 Idx = ((SMBIOS_TYPE1 *)Ptr)->Manufacturer;
      if (Idx == 0) return FALSE;
      for (UINT8 j=1; j<Idx; j++) while (*Str++);
      if (strstr(Str, "Dell") || strstr(Str, "DELL")) return TRUE;
      return FALSE;
    }
    Ptr += Hdr->Length; while (*(UINT16*)Ptr != 0) Ptr++; Ptr += 2;
  }
  return FALSE;
}

STATIC BOOLEAN DtShell (VOID) {
  CHAR8 Line[512]; UINTN Len, Index; EFI_INPUT_KEY Key;
  Print (L"\nDECtalk shell. ESC=quit, CTRL-C=cancel, F1=reselect\n");
  for (;;) {
    Print (L"> "); Len = 0;
    for (;;) {
      uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &Index);
      if (EFI_ERROR (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key))) continue;
      if (Key.ScanCode == 0x000B) return TRUE;
      if (Key.ScanCode == 0x0017) return FALSE;
      if (Key.UnicodeChar == 0x0D) { Print (L"\n"); break; }
      if (Key.UnicodeChar == 0x08) { if (Len > 0) { Len--; Print (L"\b \b"); } continue; }
      if (Key.UnicodeChar >= 0x20 && Len < sizeof (Line) - 1) { Line[Len++] = (CHAR8)Key.UnicodeChar; Print (L"%c", Key.UnicodeChar); }
    }
    Line[Len] = '\0'; if (Len == 0) continue;
    if (AsciiStrCmp (Line, "exit") == 0) return FALSE;
    DtStreamSpeak (Line);
  }
}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    uefi_call_wrapper(BS->SetWatchdogTimer, 4, 0, 0, 0, NULL);
    Print (L"DECtalk EFI -- pre-boot speech synthesis\n");
    if (IsDellMachine()) { // fuck DELL
        Print(L"\n  [!] Warning: DELL device detected, hardware support is known to be bad,\n");
        Print(L"      expect CTRL-C to not work and expect audio quality degradation.\n\n");
    }
    gSpeechBuf = (INT16*)malloc(SPEECH_BUF_SAMPLES * sizeof(INT16));
    for (;;) {
        ScanTargets ();
        UINTN Selected = SelectTarget ();
        if (Selected == MAX_TARGETS) continue;
        HDA_TARGET *T = &gTargets[Selected];
        if (EFI_ERROR (HdaFindPciIo (T->CtrlIdx)) || EFI_ERROR (HdaControllerReset ()) || EFI_ERROR (HdaInitCorbRirb ())) continue;
        if (EFI_ERROR (DtCodecInit (&T->Path))) continue;
        TextToSpeechInit (DtAudioCallback, NULL);
        BOOLEAN Restart = DtShell ();
        TextToSpeechReset ();
        if (!Restart) break;
    }
    free(gSpeechBuf);
    return EFI_SUCCESS;
}
