//#include <vector>

//static std::vector<short> samples;
#define MAX_BUFFER (10 * 60) * 8000
short samples[MAX_BUFFER]; // store 60 seconds of speech

extern "C" {
    int total_size = 0;
    #include "epsonapi.h"

    void write_wav(short *iwave, int length) {
        if (total_size + length > MAX_BUFFER) {
            return;
        }
        for (int i = 0; i < length; i++) {
            samples[total_size + i] = iwave[i];
        }
        total_size += length;
    }

}

#include <jni.h>
extern "C" JNIEXPORT void JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechInit(JNIEnv* env, jobject obj) {
    total_size = 0; // reset for new prompts
    TextToSpeechInit();
}
extern "C" JNIEXPORT void JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechReset(JNIEnv* env, jobject obj) {
    total_size = 0; // reset for new prompts
    TextToSpeechReset();
}
extern "C" JNIEXPORT void JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechChangeVoice(JNIEnv* env, jobject obj, jstring name) {
    char *name_str = (char *)env->GetStringUTFChars(name, 0);
    TextToSpeechChangeVoice(name_str);
}
extern "C" JNIEXPORT void JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechSetRate(JNIEnv* env, jobject obj, jint rate) {
    TextToSpeechSetRate(rate);
}
extern "C" JNIEXPORT void JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechSetVoiceParam(JNIEnv* env, jobject obj, jstring cmd, jint value) {
    char *cmd_str = (char *)env->GetStringUTFChars(cmd, 0);
    TextToSpeechSetVoiceParam(cmd_str, value);
}

extern "C" JNIEXPORT jshort JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechGetSpdefValue(JNIEnv* env, jobject obj,  jint index) {
    return TextToSpeechGetSpdefValue(index);
}

extern "C" JNIEXPORT jshortArray JNICALL Java_com_bytesizedfox_microdectalk_MainActivity_TextToSpeechStart(JNIEnv* env, jobject obj, jstring text) {
    const char *text_str = env->GetStringUTFChars(text, 0);
    total_size = 0; // reset for new prompts

    TextToSpeechStart((char *)text_str);

    jsize length = total_size;
    jshortArray result = env->NewShortArray(length);
    env->SetShortArrayRegion(result, 0, length, samples);
    return result;
}
