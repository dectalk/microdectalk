# MicroDectalk Android

## Overview
MicroDectalk Android and Emscripten port

## Prerequisites

### System Requirements
- Java Development Kit (JDK)
- curl
- note: this project does not require the NDK, it compiles raw static shared libraries
## Getting Started

### Cloning the Repository
```bash
git clone --recursive https://github.com/dectalk/microdectalk --branch microdectalk-android
cd microdectalk
```

### Build Process
The project uses an automated build script that:
- Automatically downloads the required toolchain
- Compiles the project
- generates wasm files and index.html
- Generates an Android APK

To build the project, simply run:
```bash
./compile.sh
```

### Build Output
After compilation, you'll find the generated files in the `out/[platform]` directory.

## Contributing
if you make any progress towards a specific goal,
just state the goal in a merge request, and it will be reviewed.
