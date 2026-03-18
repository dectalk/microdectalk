#!/bin/bash
set -e

# Configuration
APP_NAME="HdaPlayer.efi"
IMG_NAME="ctrltest.img"
IMG_SIZE_MB=64

echo "Creating bootable UEFI image: ${IMG_NAME}"

# 1. Create a zeroed image file
truncate -s ${IMG_SIZE_MB}M ${IMG_NAME}

# 2. Format as FAT32 (Superfloppy format, works for UEFI boot)
# We use mformat from mtools
mformat -i ${IMG_NAME} -F -v "UEFISOUND" ::

# 3. Create standard UEFI boot directory structure
mmd -i ${IMG_NAME} ::/EFI
mmd -i ${IMG_NAME} ::/EFI/BOOT

# 4. Copy the application to the boot path
if [ -f "${APP_NAME}" ]; then
    mcopy -i ${IMG_NAME} ${APP_NAME} ::/EFI/BOOT/BOOTX64.EFI
    echo "Success: ${APP_NAME} installed to /EFI/BOOT/BOOTX64.EFI"
else
    echo "Error: ${APP_NAME} not found. Build it first!"
    exit 1
fi

echo ""
echo "Done. You can now write this image to a USB drive using dd:"
echo "  sudo dd if=${IMG_NAME} of=/dev/sdX bs=4M status=progress"
echo "  (Replace /dev/sdX with your actual USB device node)"
