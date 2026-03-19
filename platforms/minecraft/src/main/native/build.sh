#!/bin/bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd "${SCRIPT_DIR}/../../../../../" && pwd)
OUTPUT_DIR="${SCRIPT_DIR}/../resources/natives"
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

echo "Using JAVA_HOME: ${JAVA_HOME}"
echo "Using repo root: ${REPO_ROOT}"

if [ ! -f "${JAVA_HOME}/include/jni.h" ]; then
    echo "ERROR: jni.h not found at ${JAVA_HOME}/include/jni.h"
    exit 1
fi

build_linux() {
    local out_dir="${OUTPUT_DIR}/linux64"
    local engine_lib="${out_dir}/libdectalk.so"
    local jni_lib="${out_dir}/libtts_jni_linux64.so"

    mkdir -p "${out_dir}"

    echo "Building DECtalk engine for Linux x64..."
    gcc -shared -fPIC -O2 \
        -I"${ENGINE_INCLUDE_DIR}" \
        "${COMMON_DEFINES[@]}" \
        -Wl,--version-script="${SCRIPT_DIR}/hide_symbols.map" \
        -o "${engine_lib}" \
        "${ENGINE_SOURCES[@]}" \
        -lpthread -lm

    echo "Building JNI wrapper for Linux x64..."
    gcc -shared -fPIC -O2 \
        -I"${JAVA_HOME}/include" \
        -I"${JAVA_HOME}/include/linux" \
        -I"${ENGINE_INCLUDE_DIR}" \
        -I"${SCRIPT_DIR}" \
        -o "${jni_lib}" \
        "${SCRIPT_DIR}/TTSNative.c" \
        -L"${out_dir}" -ldectalk \
        -Wl,-rpath,'$ORIGIN' \
        -lpthread

    echo "Checking dependencies:"
    ldd "${jni_lib}" || true
    echo "Linux build complete: ${out_dir}"
}

build_macos() {
    local out_dir="${OUTPUT_DIR}/macos"
    local engine_lib="${out_dir}/libdectalk.dylib"
    local jni_lib="${out_dir}/libtts_jni_macos.dylib"

    mkdir -p "${out_dir}"

    echo "Building DECtalk engine for macOS..."
    cc -dynamiclib -fPIC -O2 \
        -I"${ENGINE_INCLUDE_DIR}" \
        "${COMMON_DEFINES[@]}" \
        -o "${engine_lib}" \
        "${ENGINE_SOURCES[@]}" \
        -lpthread -lm

    echo "Building JNI wrapper for macOS..."
    cc -dynamiclib -fPIC -O2 \
        -I"${JAVA_HOME}/include" \
        -I"${JAVA_HOME}/include/darwin" \
        -I"${ENGINE_INCLUDE_DIR}" \
        -I"${SCRIPT_DIR}" \
        -o "${jni_lib}" \
        "${SCRIPT_DIR}/TTSNative.c" \
        -L"${out_dir}" -ldectalk \
        -Wl,-rpath,@loader_path \
        -lpthread

    echo "macOS build complete: ${out_dir}"
}

case "$(uname -s)" in
    Linux)
        build_linux
        if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
            echo "MinGW detected, also building Windows x64 natives..."
            bash "${SCRIPT_DIR}/build_windows.sh"
        else
            echo "MinGW not found, skipping Windows x64 native build"
        fi
        ;;
    Darwin)
        build_macos
        ;;
    *)
        echo "ERROR: Unsupported host OS: $(uname -s)"
        exit 1
        ;;
esac
