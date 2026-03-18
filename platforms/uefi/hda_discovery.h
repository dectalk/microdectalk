//
// hda_discovery.h — HDA codec graph walker
//

#pragma once
#include <Uefi.h>

// ---------------------------------------------------------------------------
// Audio path — one usable analog output discovered on a codec
// ---------------------------------------------------------------------------

typedef struct {
  UINT8  CodecAddr;
  UINT8  AfgNode;
  UINT8  DacNode;
  UINT8  MixNode;        // 0 = direct DAC→Pin connection
  UINT8  PinNode;
  UINT8  DefaultDevice;  // GET_CFG_DEFAULT [23:20]: 0=LineOut,1=Speaker,2=HPOut,...
} HDA_AUDIO_PATH;

// ---------------------------------------------------------------------------
// Discovery API
// ---------------------------------------------------------------------------

/**
  Walk the HDA codec graph on the current controller (gPciIo) and collect
  every usable analog output path (digital/HDMI/NC pins are skipped).
  Must be called after HdaInitCorbRirb().

  @param[out] Paths      Array to fill.
  @param[in]  MaxPaths   Capacity of Paths[].
  @param[out] Found      Number of paths written into Paths[].
  @retval EFI_SUCCESS    At least one path found.
  @retval EFI_NOT_FOUND  No usable analog outputs on this controller.
**/
EFI_STATUS
EFIAPI
HdaDiscover (
  OUT HDA_AUDIO_PATH  *Paths,
  IN  UINTN            MaxPaths,
  OUT UINTN           *Found
  );
