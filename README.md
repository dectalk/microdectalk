A smaller version of dectalk indended for portability between platforms
supports unix-like operating systems and Android

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
