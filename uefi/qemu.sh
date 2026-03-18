#!/bin/bash
# qemu.sh — pack HdaPlayer.efi into ESP and launch QEMU
# Generated for gnu-efi build process

set -e

# Paths
PKG_DIR="$(cd "$(dirname "$0")" && pwd)"
EFI_BIN="$PKG_DIR/HdaPlayer.efi"
ESP_IMG="$PKG_DIR/esp.img"
OVMF_CODE="/usr/share/OVMF/OVMF_CODE.fd"
OVMF_VARS_SRC="/usr/share/OVMF/OVMF_VARS.fd"
OVMF_VARS="$PKG_DIR/OVMF_VARS.fd"

# Colours
G='\033[0;32m'; B='\033[0;34m'; N='\033[0m'
info()    { echo -e "${B}[*]${N} $*"; }
success() { echo -e "${G}[+]${N} $*"; }

if [[ ! -f "$EFI_BIN" ]]; then
    info "EFI binary not found. Attempting to build..."
    make -C "$PKG_DIR"
fi

info "Packing ESP image..."
dd if=/dev/zero of="$ESP_IMG" bs=1M count=64 status=none
mkfs.fat -F 16 "$ESP_IMG" > /dev/null

mmd  -i "$ESP_IMG" ::/EFI
mmd  -i "$ESP_IMG" ::/EFI/BOOT

# Autoboot script
printf 'fs0:\\EFI\\BOOT\\BOOTX64.EFI\r\n' | \
  mcopy -i "$ESP_IMG" - ::/startup.nsh

mcopy -i "$ESP_IMG" "$EFI_BIN" ::/EFI/BOOT/BOOTX64.EFI

success "ESP image ready: $ESP_IMG"

info "Launching QEMU..."
cp "$OVMF_VARS_SRC" "$OVMF_VARS"

qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
  -drive if=pflash,format=raw,readonly=off,file="$OVMF_VARS" \
  -drive format=raw,file="$ESP_IMG" \
  -device ich9-intel-hda \
  -device hda-duplex,audiodev=snd0 \
  -audiodev pipewire,id=snd0 \
  -serial stdio \
  -debugcon file:"$PKG_DIR/debug.log" \
  -global isa-debugcon.iobase=0x402 \
  -vga std || true
