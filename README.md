A smaller version of dectalk intended for portability between platforms
supports windows, unix-like operating systems, and Android

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
premake5 gmake --build-speak=no # speak is currently unsupported on windows
make config=release_win64
```
