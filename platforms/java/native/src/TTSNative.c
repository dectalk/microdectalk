#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include "epsonapi.h"

#define MAX_BUFFER (10 * 60) * 11025
static short samples[MAX_BUFFER];
static int total_size = 0;
static int halting = 0;

short *audio_callback(short *iwave, long length, int phoneme) {
    if (halting) {
        return NULL;
    }
    
    // Check if we have room
    if (total_size + length > MAX_BUFFER) {
        // Buffer full, drop samples
        return NULL;
    }
    
    // Append to buffer
    for (int i = 0; i < length; i++) {
        samples[total_size + i] = iwave[i];
    }
    total_size += length;
    
    return NULL;
}

JNIEXPORT jboolean JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_init
  (JNIEnv *env, jclass cls) {
    total_size = 0;
    halting = 0;
    int result = TextToSpeechInit(audio_callback, NULL);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_speak
  (JNIEnv *env, jclass cls, jstring text) {
    const char *native_text = (*env)->GetStringUTFChars(env, text, NULL);
    
    // Reset buffer for new synthesis
    total_size = 0;
    halting = 0;
    
    int result = TextToSpeechStart((char *)native_text, NULL, WAVE_FORMAT_1M16);
    
    (*env)->ReleaseStringUTFChars(env, text, native_text);
    
    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_sync
  (JNIEnv *env, jclass cls) {
    TextToSpeechSync();
}

JNIEXPORT void JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_reset
  (JNIEnv *env, jclass cls) {
    halting = 1;
    TextToSpeechStart("", NULL, WAVE_FORMAT_1M16);
    total_size = 0;
}

JNIEXPORT jint JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_getAvailableSamples
  (JNIEnv *env, jclass cls) {
    return total_size;
}

JNIEXPORT jint JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_readSamples
  (JNIEnv *env, jclass cls, jshortArray output, jint count) {
    int samples_to_read = (count < total_size) ? count : total_size;
    
    if (samples_to_read <= 0) {
        return 0;
    }
    
    jshort *native_output = (*env)->GetShortArrayElements(env, output, NULL);
    
    // Copy from beginning of buffer
    memcpy(native_output, samples, samples_to_read * sizeof(short));
    
    (*env)->ReleaseShortArrayElements(env, output, native_output, 0);
    
    return samples_to_read;
}
