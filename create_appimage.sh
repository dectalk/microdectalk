#!/bin/sh
if [ ! -f appimagetool ]; then
	wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage -O appimagetool
fi
chmod +x appimagetool

rm -rf appdir
mkdir -p appdir/usr/bin
mkdir -p appdir/usr/lib/x86_64-linux-gnu

ln -s usr/lib appdir/lib
ln -s usr/lib appdir/lib64
ln -s lib appdir/usr/lib64

cp bin/Native/Release/speak appdir/usr/bin/
cp resources/speak.desktop appdir/
cp resources/paul.png appdir/
cp resources/AppRun appdir/
cp DECtalk.conf appdir/
cp -rf dic appdir/
chmod +x appdir/AppRun

ldd appdir/usr/bin/speak | while read a; do
	path=`echo "$a" | grep -oE '/[^ ]+'`
	if [ ! "x$path" = "x" ]; then
		cp $path appdir/$path
	fi
done

strip appdir/usr/lib/* appdir/usr/lib/*/* appdir/usr/bin/*

ARCH=x86_64
export ARCH
./appimagetool appdir Speak-x86_64.AppImage
