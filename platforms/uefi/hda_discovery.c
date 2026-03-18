#include <uefi_shim.h>
#include "hda_regs.h"
#include "hda_discovery.h"

extern EFI_STATUS EFIAPI HdaSendVerb (UINT32 Verb, UINT32 *Response);
extern UINT16     EFIAPI HdaReadStateSts (VOID);

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

STATIC UINT32
Verb (UINT8 Ca, UINT8 Node, UINT16 VerbId, UINT8 Payload)
{
  UINT32 R = 0xFFFFFFFF;
  HdaSendVerb (HDA_VERB (Ca, Node, VerbId, Payload), &R);
  return R;
}

// Return the NID at index 0 of a node's connection list (short form).
STATIC UINT8
ConnEntry0 (UINT8 Ca, UINT8 Node)
{
  UINT32 R;
  if ((Verb (Ca, Node, HDA_GET_PARAM, HDA_PARAM_CONN_LIST_LEN) & 0x7F) == 0)
    return 0;
  R = Verb (Ca, Node, HDA_GET_CONN_LIST, 0);
  return (R == 0xFFFFFFFF) ? 0 : (UINT8)(R & 0xFF);
}

// ---------------------------------------------------------------------------
// HdaDiscover
// ---------------------------------------------------------------------------

EFI_STATUS
EFIAPI
HdaDiscover (
  OUT HDA_AUDIO_PATH  *Paths,
  IN  UINTN            MaxPaths,
  OUT UINTN           *Found
  )
{
  UINT16  StateSts;
  UINT8   Ca, i;
  UINT32  R;
  UINT8   FgStart, FgCount, Afg;
  UINT8   WStart, WCount, Wid;
  UINT8   Conn0, Conn0Type;
  UINT32  CfgDef;
  UINT8   PortConn, DefDev;
  UINT8   DacNode, MixNode;

  *Found   = 0;
  StateSts = HdaReadStateSts ();
  if (StateSts == 0) StateSts = 0x7FFF;

  for (Ca = 0; Ca < 15 && *Found < MaxPaths; Ca++) {
    if (!(StateSts & (UINT16)(1 << Ca))) continue;

    R = 0;
    for (i = 0; i < 5; i++) {
      R = Verb (Ca, 0, HDA_GET_PARAM, HDA_PARAM_VENDOR_ID);
      if (R != 0 && R != 0xFFFFFFFF) break;
      uefi_call_wrapper(BS->Stall, 1, 50000);
    }
    if (R == 0 || R == 0xFFFFFFFF) continue;

    Print (L"  Codec %d: vendor 0x%08x\n", (UINTN)Ca, (UINTN)R);

    R = Verb (Ca, 0, HDA_GET_PARAM, HDA_PARAM_NODE_COUNT);
    if (R == 0xFFFFFFFF) continue;
    FgStart = (UINT8)((R >> 16) & 0xFF);
    FgCount = (UINT8)( R        & 0xFF);

    Afg = 0;
    for (i = 0; i < FgCount; i++) {
      UINT8 Node = FgStart + i;
      if ((Verb (Ca, Node, HDA_GET_PARAM, HDA_PARAM_FUNC_GROUP_TYPE) & 0xFF) == 0x01) {
        Afg = Node; break;
      }
    }
    if (!Afg) { Print (L"  Codec %d: no AFG\n", (UINTN)Ca); continue; }

    HdaSendVerb (HDA_VERB (Ca, Afg, HDA_SET_POWER_STATE, 0x00), NULL);
    uefi_call_wrapper(BS->Stall, 1, 10000);

    R = Verb (Ca, Afg, HDA_GET_PARAM, HDA_PARAM_NODE_COUNT);
    if (R == 0xFFFFFFFF) continue;
    WStart = (UINT8)((R >> 16) & 0xFF);
    WCount = (UINT8)( R        & 0xFF);

    for (i = 0; i < WCount && *Found < MaxPaths; i++) {
      Wid = WStart + i;

      R = Verb (Ca, Wid, HDA_GET_PARAM, HDA_PARAM_WIDGET_CAP);
      if (R == 0xFFFFFFFF) continue;
      if ((UINT8)HDA_WIDGET_TYPE (R) != HDA_WTYPE_PIN_COMPLEX) continue;
      if (R & BIT9) continue;

      if (!(Verb (Ca, Wid, HDA_GET_PARAM, HDA_PARAM_PIN_CAP) & BIT4))
        continue;

      CfgDef   = Verb (Ca, Wid, HDA_GET_CFG_DEFAULT, 0);
      PortConn = (UINT8)((CfgDef >> 30) & 0x3);
      DefDev   = (UINT8)((CfgDef >> 20) & 0xF);
      if (PortConn == 0x1) continue;
      if (DefDev   >= 0x8) continue;

      DacNode = 0; MixNode = 0;
      Conn0 = ConnEntry0 (Ca, Wid);
      if (!Conn0) continue;

      R = Verb (Ca, Conn0, HDA_GET_PARAM, HDA_PARAM_WIDGET_CAP);
      if (R == 0xFFFFFFFF) continue;
      Conn0Type = (UINT8)HDA_WIDGET_TYPE (R);

      if (Conn0Type == HDA_WTYPE_AUDIO_OUT) {
        DacNode = Conn0;
      } else if (Conn0Type == HDA_WTYPE_AUDIO_MIXER ||
                 Conn0Type == HDA_WTYPE_AUDIO_SEL) {
        UINT8 Via  = Conn0;
        UINT8 Dac2 = ConnEntry0 (Ca, Via);
        if (!Dac2) continue;
        if ((UINT8)HDA_WIDGET_TYPE (Verb (Ca, Dac2, HDA_GET_PARAM, HDA_PARAM_WIDGET_CAP))
            != HDA_WTYPE_AUDIO_OUT) continue;
        DacNode = Dac2;
        MixNode = Via;
      } else {
        continue;
      }

      Paths[*Found].CodecAddr    = Ca;
      Paths[*Found].AfgNode      = Afg;
      Paths[*Found].DacNode      = DacNode;
      Paths[*Found].MixNode      = MixNode;
      Paths[*Found].PinNode      = Wid;
      Paths[*Found].DefaultDevice = DefDev;
      (*Found)++;
    }
  }

  return (*Found > 0) ? EFI_SUCCESS : EFI_NOT_FOUND;
}
