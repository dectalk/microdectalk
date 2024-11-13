
extern "C" {
#include "epsonapi.h"
extern void printSamples();
}

int main() {
    TextToSpeechInit();
    TextToSpeechStart("Hello World, how are you today?");
    TextToSpeechReset();

    printSamples();
}
