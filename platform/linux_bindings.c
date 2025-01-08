#include <stdio.h>
#include <time.h>

#include "epsonapi.h"
#include "wav.c"

int main () {
    init_wav("output.wav");
    TextToSpeechInit();

    TextToSpeechStart("Hello World, engine, radio, to, test, Hello World, describe image.");

    close_wav();
}
