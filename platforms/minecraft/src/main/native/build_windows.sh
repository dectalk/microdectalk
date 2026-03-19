#!/bin/bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd "${SCRIPT_DIR}/../../../../../" && pwd)
OUTPUT_DIR="${SCRIPT_DIR}/../resources/natives/win64"
ENGINE_SRC_DIR="${REPO_ROOT}/src"
ENGINE_INCLUDE_DIR="${REPO_ROOT}/include"

if [ -z "${JAVA_HOME:-}" ] && command -v javac >/dev/null 2>&1; then
    JAVA_HOME=$(cd "$(dirname "$(readlink -f "$(command -v javac)")")/.." && pwd)
fi
JAVA_HOME=${JAVA_HOME:-/usr/lib/jvm/java-21-openjdk}

COMMON_DEFINES=(
    -D_REENTRANT=1
    -DNOMME=1
    -DLTSSIM=1
    -DTTSSIM=1
    -DANSI=1
    -DBLD_DECTALK_DLL=1
    -DENGLISH=1
    -DENGLISH_US=1
    -DACCESS32=1
    -DTYPING_MODE=1
    -DACNA=1
    -DDISABLE_AUDIO=1
    -DSINGLE_THREADED=1
    -DNO_FILESYSTEM=1
    -D__unix__=1
)

ENGINE_SOURCES=("${ENGINE_SRC_DIR}"/*.c)

echo "=== Building Windows x64 DLL with MinGW ==="
echo "Using JAVA_HOME: ${JAVA_HOME}"
echo "Using repo root: ${REPO_ROOT}"

if ! command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    echo "ERROR: x86_64-w64-mingw32-gcc not found"
    exit 1
fi

if [ ! -f "${JAVA_HOME}/include/jni.h" ]; then
    echo "ERROR: jni.h not found at ${JAVA_HOME}/include/jni.h"
    exit 1
fi

JNI_MD_DIR=""
if [ -f "${JAVA_HOME}/include/win32/jni_md.h" ]; then
    JNI_MD_DIR="${JAVA_HOME}/include/win32"
elif [ -f "${JAVA_HOME}/include/linux/jni_md.h" ]; then
    JNI_MD_DIR="${JAVA_HOME}/include/linux"
else
    echo "ERROR: jni_md.h not found"
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"

echo "Building DECtalk engine for Windows x64..."
x86_64-w64-mingw32-gcc -shared -O2 \
    -I"${ENGINE_INCLUDE_DIR}" \
    "${COMMON_DEFINES[@]}" \
    -o "${OUTPUT_DIR}/dtc.dll" \
    "${ENGINE_SOURCES[@]}" \
    -lws2_32 -static-libgcc

echo "Building JNI wrapper for Windows x64..."
x86_64-w64-mingw32-gcc -shared -O2 \
    -I"${JAVA_HOME}/include" \
    -I"${JNI_MD_DIR}" \
    -I"${ENGINE_INCLUDE_DIR}" \
    -I"${SCRIPT_DIR}" \
    -o "${OUTPUT_DIR}/tts_jni_win64.dll" \
    "${SCRIPT_DIR}/TTSNative.c" \
    -L"${OUTPUT_DIR}" -ldtc \
    -lws2_32 -static-libgcc

echo "Checking dependencies:"
x86_64-w64-mingw32-objdump -p "${OUTPUT_DIR}/tts_jni_win64.dll" | grep "DLL Name" || true

echo "Windows x64 build complete: ${OUTPUT_DIR}"
ls -lh "${OUTPUT_DIR}"
