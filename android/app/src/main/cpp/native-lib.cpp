#include <jni.h>
#include <string>

#define MAX_BUFFER (10 * 60) * 11025
short samples[MAX_BUFFER]; // store 60 seconds of speech

extern "C" {
int total_size = 0;
#include "epsonapi.h"

// Global variables to store JVM reference and callback information
typedef struct {
    JavaVM* jvm;
    jobject callbackObject;    // Global reference to the Java object
    jclass callbackClass;      // Global reference to the Java class
    jmethodID callbackMethod;  // Method ID for the callback
} CallbackInfo;

static CallbackInfo g_callbackInfo = {NULL, NULL, NULL, NULL};
typedef void (*CCallbackFunc)(short* iwave, int length);
static CCallbackFunc g_registeredCallback = NULL;

int halting = 0;
int callback = 0;

short *write_wav(short *iwave, long length, int phoneme) {
    (void)phoneme;

    if (halting) {
        return iwave;
    }

    if (total_size + length > MAX_BUFFER) {
        return iwave;
    }
    for (int i = 0; i < length; i++) {
        samples[total_size + i] = iwave[i];
    }
    total_size += length;

    if (callback && g_registeredCallback != NULL) {
        g_registeredCallback(iwave, length);
    }

    return iwave;
}

}

extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechInit(JNIEnv* env, jobject obj) {
    total_size = 0; // reset for new prompts
    halting = 0;
    callback = 0;
    TextToSpeechInit(write_wav, nullptr);
}
extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechReset(JNIEnv* env, jobject obj) {
    total_size = 0; // reset for new prompts
    TextToSpeechReset();
}
extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechChangeVoice(JNIEnv* env, jobject obj, jstring name) {
    const char *name_str = env->GetStringUTFChars(name, 0);
    TextToSpeechChangeVoice(name_str);
    env->ReleaseStringUTFChars(name, name_str);
}
extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechSetRate(JNIEnv* env, jobject obj, jint rate) {
    TextToSpeechSetRate(rate);
}
extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechSetVoiceParam(JNIEnv* env, jobject obj, jstring cmd, jint value) {
    const char *cmd_str = env->GetStringUTFChars(cmd, 0);
    TextToSpeechSetVoiceParam(cmd_str, value);
    env->ReleaseStringUTFChars(cmd, cmd_str);
}

extern "C" JNIEXPORT jshort JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechGetSpdefValue(JNIEnv* env, jobject obj,  jint index) {
    return TextToSpeechGetSpdefValue(index);
}
extern "C" JNIEXPORT jshortArray JNICALL Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechStart(JNIEnv* env, jobject obj, jstring text, jboolean enableCallback) {
    const char *text_str = env->GetStringUTFChars(text, 0);

    total_size = 0;
    callback = enableCallback;

    TextToSpeechStart((char *)text_str, nullptr, WAVE_FORMAT_1M16);
    TextToSpeechSync();
    env->ReleaseStringUTFChars(text, text_str);

    jsize length = total_size;
    jshortArray result = env->NewShortArray(length);
    env->SetShortArrayRegion(result, 0, length, samples);
    return result;
}

void cCallbackHandler(short *iwave, int length) {
    JNIEnv* env;
    int attachResult;
    int detachNeeded = 0;

    if (g_callbackInfo.jvm == NULL) {
        return;
    }

    // Attach current thread to JVM if needed
    attachResult = g_callbackInfo.jvm->GetEnv((void**)&env,
                                                 JNI_VERSION_1_6);

    if (attachResult == JNI_EDETACHED) {
        // Thread not attached, need to attach and later detach
        attachResult = g_callbackInfo.jvm->AttachCurrentThread(&env, NULL);

        if (attachResult != JNI_OK) {
            return;
        }
        detachNeeded = 1;
    } else if (attachResult != JNI_OK) {
        return;
    }

    jshortArray result = env->NewShortArray(length);
    env->SetShortArrayRegion(result, 0, length, iwave);

    // 1. Call instance method if we have a valid object reference
    if (g_callbackInfo.callbackObject != NULL && g_callbackInfo.callbackMethod != NULL) {
        printf("Calling Java instance callback method\n");
        env->CallVoidMethod(g_callbackInfo.callbackObject,
                               g_callbackInfo.callbackMethod, result, length);
    } else {
        printf("No valid callback method available\n");
    }

    // Detach thread if we attached it
    if (detachNeeded) {
        printf("Detaching thread from JVM\n");
        g_callbackInfo.jvm->DetachCurrentThread();
    }
}

extern "C" JNIEXPORT void JNICALL Java_dev_bytesizedfox_microdectalktest_tts_TtsService_SetCallback(JNIEnv *env, jobject obj) {
    env->GetJavaVM(&g_callbackInfo.jvm);
    jclass localClass = env->GetObjectClass(obj);
    g_callbackInfo.callbackClass = (jclass) env->NewGlobalRef(localClass);
    g_callbackInfo.callbackObject = env->NewGlobalRef(obj);
    g_callbackInfo.callbackMethod = env->GetMethodID(
            g_callbackInfo.callbackClass,
            "javaCallback", "([SI)V");

    g_registeredCallback = cCallbackHandler;
}

extern "C"
JNIEXPORT void JNICALL
Java_dev_bytesizedfox_microdectalktest_App_TextToSpeechSync(JNIEnv *env, jclass clazz,
                                                                     jint rate) {
    (void)env;
    (void)clazz;
    (void)rate;
    halting = 1;
    TextToSpeechReset();
}
