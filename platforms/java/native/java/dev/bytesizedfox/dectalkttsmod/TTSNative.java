package dev.bytesizedfox.dectalkttsmod;

public class TTSNative {
    private static boolean loaded = false;

    static {
        try {
            // Load both the TTS library and JNI wrapper
            NativeLoader.loadLibrary("tts_jni");
            loaded = true;
        } catch (Exception e) {
            System.err.println("Failed to load TTS native libraries: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static boolean isLoaded() {
        return loaded;
    }

    public static native boolean init();
    public static native boolean speak(String text);
    public static native void sync();
    public static native void reset();
    public static native int getAvailableSamples();
    public static native int readSamples(short[] output, int count);
}