#!/bin/sh
if [ ! -f linuxdeploy ]; then
	wget -O linuxdeploy https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20250213-2/linuxdeploy-x86_64.AppImage
fi
chmod +x linuxdeploy

rm -rf appdir
./linuxdeploy --appdir appdir --executable bin/Native/Release/speak --desktop-file resources/speak.desktop --icon-file resources/paul.png --output appimage
