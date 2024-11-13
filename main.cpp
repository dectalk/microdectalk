//#include <vector>

//static std::vector<short> samples;
short samples[60 * 8000]; // store 60 seconds of speech

extern "C" {
    int total_size = 0;
    #include "epsonapi.h"

    void write_wav(short *iwave, int length) {
        for (int i = 0; i < length; i++) {
            samples[total_size + i] = iwave[i];
        }
        total_size += length;
    }
}

#include <jni.h>
extern "C" JNIEXPORT jshortArray JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_stringFromJNI(JNIEnv* env, jobject obj, jstring text) {
    const char *text_str = env->GetStringUTFChars(text, 0);

    total_size = 0; // reset for new prompts

    TextToSpeechInit();
    TextToSpeechStart((char *)text_str);
    TextToSpeechReset();

    jsize length = total_size;
    jshortArray result = env->NewShortArray(length);

    env->SetShortArrayRegion(result, 0, length, samples);
    return result;
}
