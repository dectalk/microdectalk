cd platform/rpi-pico
mkdir -p build
cd build
PICO_SDK_FETCH_FROM_GIT=1 PICO_EXTRAS_FETCH_FROM_GIT=1 cmake ..
make

cd ../../../
mkdir -p out/pico/
cp platform/rpi-pico/build/hello_usb_cpp.uf2 out/pico/microdectalk-pico.uf2
