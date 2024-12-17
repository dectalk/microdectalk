cd platform/rpi-pico
mkdir -p build
cd build
PICO_SDK_FETCH_FROM_GIT=1 cmake ..
make
cd ../../
