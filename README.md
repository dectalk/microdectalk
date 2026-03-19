A version of dectalk intended for portability between platforms
supports windows, unix-like operating systems (such as Linux, MacOS, BSD, and others), and Android
depending on the platform, it's nearly identical to the main [DECtalk](https://github.com/dectalk/dectalk) repo with the exception of loadv and setv commands and minor platform-specific fixes

try it out [Here](https://bytesizedfox.dev)

dependencies for gui:
```
libXpm-devel
motif-devel
libXt-devel
```

to build:
```
premake5 gmake
make
```

for speak gui demo:
```
premake5 gmake --build-speak=yes
make 
```

to compile for windows
```
premake5 gmake
make config=release_win64
```

to compile for windows with speak gui demo
```
premake5 gmake --build-speak=yes
make config=release_win64
```

to compile for android, open `platforms/android` in Android Studio or run:
```
cd platforms/android
./gradlew assembleDebug
```

to compile for minecraft, install a linux JDK/runtime setup that matches `platforms/minecraft/gradle.properties`, make sure MinGW is installed if you want the bundled Windows natives too, then run:
```
cd platforms/minecraft
./gradlew assemble
```
and check build/libs

to compile for rpi pico, install the pico SDK and use cmake in the platforms/pico folder

to compile for gameboy advance, install gba-dev from pacman following [this guide](https://devkitpro.org/wiki/Getting_Started) and use make in the platforms/gba folder

for webassembly go into the platforms/wasm folder, make sure you have the emscripten sdk in your home folder and activated and run bash compile.sh, launch it in a webserver for CORS compatability

to compile for uefi, install gnu-efi and run make in the platforms/uefi folder (creates HdaPlayer.efi)

note unsupported: if your device isn't on this list, try native anyways, most platforms with a compiler should be able to compile this
