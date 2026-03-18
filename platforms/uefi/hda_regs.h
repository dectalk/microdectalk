//
// hda_regs.h — Intel HDA register map + verb helpers
// Prototype target: QEMU ich9-intel-hda + hda-duplex
//

#pragma once
#include <Uefi.h>

// ---------------------------------------------------------------------------
// MMIO helpers — read/write relative to a UINTN BAR0 base
// ---------------------------------------------------------------------------

#define HDA_READ8(base, off)      (*(volatile UINT8  *)((base) + (off)))
#define HDA_READ16(base, off)     (*(volatile UINT16 *)((base) + (off)))
#define HDA_READ32(base, off)     (*(volatile UINT32 *)((base) + (off)))

#define HDA_WRITE8(base, off, v)  (*(volatile UINT8  *)((base) + (off)) = (v))
#define HDA_WRITE16(base, off, v) (*(volatile UINT16 *)((base) + (off)) = (v))
#define HDA_WRITE32(base, off, v) (*(volatile UINT32 *)((base) + (off)) = (v))

// ---------------------------------------------------------------------------
// Controller registers (BAR0-relative)
// ---------------------------------------------------------------------------

#define HDA_GCAP        0x00    // Global Capabilities         (16-bit)
#define HDA_VMIN        0x02    // Minor Version               (8-bit)
#define HDA_VMAJ        0x03    // Major Version               (8-bit)
#define HDA_GCTL        0x08    // Global Control              (32-bit)
#define   HDA_GCTL_CRST   BIT0    //   Controller Reset (1 = running)
#define   HDA_GCTL_UNSOL  BIT8    //   Unsolicited Response Enable
#define HDA_WAKEEN      0x0C    // Wake Enable                 (16-bit)
#define HDA_STATESTS    0x0E    // State Change Status         (16-bit)
#define HDA_INTCTL      0x20    // Interrupt Control           (32-bit)
#define HDA_INTSTS      0x24    // Interrupt Status            (32-bit)
#define HDA_WALCLK      0x30    // Wall Clock Counter          (32-bit)

// CORB
#define HDA_CORBLBASE   0x40    // CORB Lower Base             (32-bit)
#define HDA_CORBUBASE   0x44    // CORB Upper Base             (32-bit)
#define HDA_CORBWP      0x48    // CORB Write Pointer          (16-bit)
#define HDA_CORBRP      0x4A    // CORB Read Pointer           (16-bit)
#define   HDA_CORBRP_RST  BIT15
#define HDA_CORBCTL     0x4C    // CORB Control                (8-bit)
#define   HDA_CORBCTL_RUN BIT1
#define HDA_CORBSTS     0x4D    // CORB Status                 (8-bit)
#define HDA_CORBSIZE    0x4E    // CORB Size                   (8-bit)

// RIRB
#define HDA_RIRBLBASE   0x50    // RIRB Lower Base             (32-bit)
#define HDA_RIRBUBASE   0x54    // RIRB Upper Base             (32-bit)
#define HDA_RIRBWP      0x58    // RIRB Write Pointer          (16-bit)
#define   HDA_RIRBWP_RST  BIT15
#define HDA_RINTCNT     0x5A    // Response Interrupt Count    (16-bit)
#define HDA_RIRBCTL     0x5C    // RIRB Control                (8-bit)
#define   HDA_RIRBCTL_RUN BIT1
#define HDA_RIRBSTS     0x5D    // RIRB Status                 (8-bit)
#define HDA_RIRBSIZE    0x5E    // RIRB Size                   (8-bit)

// Immediate Command Interface (no CORB setup needed — use for init verbs)
#define HDA_ICA         0x60    // Immediate Command Address   (32-bit write)
#define HDA_IRR         0x64    // Immediate Response Register (32-bit read)
#define HDA_IRS         0x68    // Immediate Response Status   (16-bit)
#define   HDA_IRS_VALID   BIT1    //   Response valid
#define   HDA_IRS_BUSY    BIT0    //   Command in flight

// ---------------------------------------------------------------------------
// Stream Descriptor registers
// ICH9 has 4 input + 4 output streams.
// First output stream = SD4 = 0x80 + (4 * 0x20) = 0x100
// ---------------------------------------------------------------------------

#define HDA_SD_BASE(n)      (0x80 + (UINTN)(n) * 0x20)
#define HDA_SD_OUT0         HDA_SD_BASE(4)      // First output stream

// Offsets within a stream descriptor block
#define HDA_SDCTL_OFF       0x00    // Stream Control / Status (32-bit, lower 24)
#define   HDA_SDCTL_SRST      BIT0    //   Stream Reset
#define   HDA_SDCTL_RUN       BIT1    //   Stream Run
#define   HDA_SDCTL_IOCE      BIT2    //   Interrupt on Completion Enable
#define   HDA_SDCTL_TP        BIT5    //   Traffic Priority (0=Best Effort, 1=Premium)
#define   HDA_SDCTL_STRM(n)   ((UINT32)(n) << 20)  // Stream number tag
#define HDA_SDSTS_OFF       0x03    // Stream Status           (8-bit)
#define   HDA_SDSTS_BCIS      BIT2    //   Buffer Completion Interrupt Status
#define   HDA_SDSTS_FIFOE     BIT3    //   FIFO Error
#define   HDA_SDSTS_DESE      BIT4    //   Descriptor Error
#define HDA_SDLPIB_OFF      0x04    // Link Position in Buffer (32-bit)
#define HDA_SDCBL_OFF       0x08    // Cyclic Buffer Length    (32-bit)
#define HDA_SDLVI_OFF       0x0C    // Last Valid Index        (16-bit)
#define HDA_SDFIFOS_OFF     0x10    // FIFO Size               (16-bit)
#define HDA_SDFMT_OFF       0x12    // Stream Format           (16-bit)
#define HDA_SDBDPL_OFF      0x18    // BDL Lower Base Address  (32-bit)
#define HDA_SDBDPU_OFF      0x1C    // BDL Upper Base Address  (32-bit)

// Convenience macros for SD4 (our output stream)
#define SD4(off)            (HDA_SD_OUT0 + (off))

// ---------------------------------------------------------------------------
// HDA Stream Format word  (Intel HDA spec §3.7.1)
//
// Bit 15:     non-PCM flag  (0=PCM)
// Bit 14:     BASE          (0=48kHz, 1=44.1kHz)
// Bits[13:11]:MULT          (000=×1, 001=×2, 010=×3, 011=×4)
// Bits[10:8]: DIV           (000=÷1, 001=÷2, 010=÷3, 011=÷4)
// Bits[7:4]:  channels-1    (0000=mono)
// Bits[3:0]:  bits/sample   (0000=8-bit, 0001=16-bit)
//
// 48000 Hz, Mono, 16-bit PCM:
//   BASE=0 (48kHz), MULT=000 (×1), DIV=000 (÷1)
//   0 | 0 | 000 | 000 | 0000 | 0001  =  0x0001
// ---------------------------------------------------------------------------

#define HDA_FMT_48KHZ_MONO_16   0x0001

// ---------------------------------------------------------------------------
// Verb construction
//
// 32-bit verb layout:
//   [31:28]  codec address   (0..14)
//   [27:20]  node ID
//   [19:8]   verb ID (12-bit)
//   [7:0]    parameter / payload (8-bit)
//
// For SET_STREAM_FORMAT the payload is 16-bit, packed into [19:0]:
//   [19:8] = 0x200, [15:0] = format word  → use HDA_VERB16 below
// ---------------------------------------------------------------------------

#define HDA_VERB(codec, node, verb, param) \
  ( ((UINT32)(codec) << 28) \
  | ((UINT32)(node)  << 20) \
  | ((UINT32)(verb)  <<  8) \
  | ((UINT32)(param) &  0xFF) )

// 16-bit payload verb (SET_STREAM_FORMAT)
#define HDA_VERB16(codec, node, verb4, payload16) \
  ( ((UINT32)(codec)   << 28) \
  | ((UINT32)(node)    << 20) \
  | ((UINT32)(verb4)   << 16) \
  | ((UINT32)(payload16) & 0xFFFF) )

// ---------------------------------------------------------------------------
// Verb opcodes
// ---------------------------------------------------------------------------

// GET_PARAMETER (F00h) — always 12-bit verb
#define HDA_GET_PARAM           0xF00

// Parameter selectors for GET_PARAMETER
#define HDA_PARAM_VENDOR_ID     0x00
#define HDA_PARAM_REVISION_ID   0x02
#define HDA_PARAM_NODE_COUNT    0x04    // [23:16]=start, [7:0]=count
#define HDA_PARAM_FUNC_GROUP_TYPE 0x05  // [7:0]=type: 0x01=Audio, 0x02=Modem
#define HDA_PARAM_WIDGET_CAP    0x09    // [20:16]=widget type
#define HDA_PARAM_PCM_RATES     0x0A
#define HDA_PARAM_PIN_CAP       0x0C    // bit4=output capable
#define HDA_PARAM_AMP_CAP_OUT   0x12
#define HDA_PARAM_CONN_LIST_LEN 0x0E
#define HDA_PARAM_POWER_STATES  0x0F

// GET verbs
#define HDA_GET_CONN_LIST       0xF02   // connection list entries (payload=start offset)
#define HDA_GET_CFG_DEFAULT     0xF1C   // pin configuration default

// SET verbs
#define HDA_SET_STREAM_FMT      0x2     // 4-bit verb, 16-bit payload
#define HDA_SET_AMP_GAIN        0x3     // 4-bit verb, 16-bit payload
#define HDA_SET_CONN_SELECT     0xF01   // connection select
#define HDA_SET_POWER_STATE     0x705   // D0=0x00
#define HDA_SET_PIN_CTRL        0x707   // output enable flags
#define HDA_SET_EAPD_BTL        0x70C   // EAPD/BTL enable

// Amp gain/mute payload bits (SET_AMP_GAIN 16-bit payload)
// [15]=set output amp, [14]=set input amp, [13]=left, [12]=right,
// [11:8]=input index (for input amps), [7]=mute, [6:0]=gain step
#define HDA_AMP_OUT_UNMUTE      0xB07F  // output, both ch, gain=max(127 clamped), unmuted
#define HDA_AMP_IN_UNMUTE(idx)  (0x7000 | ((UINT16)(idx) << 8) | 0x7F)
                                        // input, both ch, given index, gain=max, unmuted

// Pin control bits
#define HDA_PIN_OUT_EN          BIT6
#define HDA_PIN_HP_EN           BIT7

// Widget types (bits [20:16] of WIDGET_CAP response)
#define HDA_WIDGET_TYPE(cap)    (((cap) >> 20) & 0xF)
#define HDA_WTYPE_AUDIO_OUT     0x0     // DAC
#define HDA_WTYPE_AUDIO_IN      0x1     // ADC
#define HDA_WTYPE_AUDIO_MIXER   0x2
#define HDA_WTYPE_AUDIO_SEL     0x3
#define HDA_WTYPE_PIN_COMPLEX   0x4

// Stream tag — arbitrary 1..15, embedded in verb and SD control
#define HDA_STREAM_TAG          1
