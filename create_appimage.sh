#!/bin/sh
if [ ! -f appimagetool ]; then
	wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage -O appimagetool
fi
chmod +x appimagetool

rm -rf appdir
mkdir -p appdir/usr/bin
mkdir -p appdir/usr/lib

cp bin/Native/Release/speak appdir/usr/bin/
cp bin/Native/Release/libdtc.so appdir/usr/bin/
cp resources/speak.desktop appdir/
cp resources/paul.png appdir/
cp resources/AppRun appdir/
cp DECtalk.conf appdir/
cp -rf dic appdir/
chmod +x appdir/AppRun

ldd appdir/usr/bin/speak | grep "=>" | awk '{print $3}' | while read a; do
	if [ -f "$a" ]; then
		cp "$a" appdir/usr/lib/
	fi
done

strip appdir/usr/lib/* appdir/usr/bin/*

ARCH=x86_64
export ARCH
./appimagetool appdir Speak-x86_64.AppImage
