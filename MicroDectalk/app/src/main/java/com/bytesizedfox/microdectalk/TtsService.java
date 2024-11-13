package com.bytesizedfox.microdectalk;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.speech.tts.SynthesisCallback;
import android.speech.tts.SynthesisRequest;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeechService;
import android.speech.tts.Voice;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;

@SuppressLint("NewApi")
public class TtsService extends TextToSpeechService {
    private static final String TAG = "CustomTTSService";
    private static final int SAMPLE_RATE = 8000; // Adjust based on your audio sample rate
    private static final Locale DEFAULT_LOCALE = Locale.US;
    public static Voice mDefaultVoice;


    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "CustomTTSService created");

        // Initialize default voice
        Set<String> features = new HashSet<>();

        mDefaultVoice = new Voice("default", DEFAULT_LOCALE, Voice.QUALITY_NORMAL, Voice.LATENCY_NORMAL, false, features);
    }

    @Override
    protected String[] onGetLanguage() {
        Log.w("DEFAULT", DEFAULT_LOCALE.getLanguage());
        // Return default language, country, and variant
        return new String[] {
                DEFAULT_LOCALE.getLanguage(),
                DEFAULT_LOCALE.getCountry(),
                DEFAULT_LOCALE.getVariant()
        };
    }

    @Override
    protected int onIsLanguageAvailable(String lang, String country, String variant) {
        return TextToSpeech.LANG_AVAILABLE;
    }

    @Override
    protected int onLoadLanguage(String lang, String country, String variant) {
        // Load the requested language
        Log.w("onLoadLanguage", lang);
        return TextToSpeech.LANG_AVAILABLE;
    }

    @Override
    protected Set<String> onGetFeaturesForLanguage(String lang, String country, String variant) {
        return new HashSet<String>();
    }

    @Override
    public String onGetDefaultVoiceNameFor(String language, String country, String variant) {
        Log.w("onGetDefaultVoiceNameFor", language);
        return mDefaultVoice.getName();
    }

    @Override
    public List<android.speech.tts.Voice> onGetVoices() {
        List<android.speech.tts.Voice> voices = new ArrayList<android.speech.tts.Voice>();
        voices.add(mDefaultVoice);
        return voices;
    }

    @Override
    public int onIsValidVoiceName(String name) {
        return TextToSpeech.SUCCESS;
    }

    @Override
    public int onLoadVoice(String name) {
        return TextToSpeech.SUCCESS;
    }



    @Override
    protected void onStop() {
        // Clean up resources when TTS is stopped
        Log.d(TAG, "TTS service stopped");
    }

    @Override
    protected synchronized void onSynthesizeText(SynthesisRequest request, SynthesisCallback callback) {
        // Get the text to synthesize
        String text = request.getCharSequenceText().toString();

        try {
            // Set the audio format properties
            callback.start(SAMPLE_RATE, AudioFormat.ENCODING_PCM_16BIT, 1);

            // Get audio samples using native method
            short[] samples = MainActivity.stringFromJNI(text);

            // Convert samples to bytes
            byte[] audioData = new byte[samples.length * 2];
            ByteBuffer.wrap(audioData)
                    .order(ByteOrder.LITTLE_ENDIAN)
                    .asShortBuffer()
                    .put(samples);


            final int maxBytesToCopy = callback.getMaxBufferSize();

            int offset = 0;

            while (offset < audioData.length) {
                final int bytesToWrite = Math.min(maxBytesToCopy, (audioData.length - offset));
                callback.audioAvailable(audioData, offset, bytesToWrite);
                offset += bytesToWrite;
            }


            // Signal completion
            callback.done();

        } catch (Exception e) {
            Log.e(TAG, "Error synthesizing text", e);
            callback.error();
        }
    }
}