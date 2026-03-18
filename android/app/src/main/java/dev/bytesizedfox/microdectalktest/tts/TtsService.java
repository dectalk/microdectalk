package dev.bytesizedfox.microdectalktest.tts;

import static dev.bytesizedfox.microdectalktest.tts.TTSUtil.nameToNameCode;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.speech.tts.SynthesisCallback;
import android.speech.tts.SynthesisRequest;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeechService;
import android.speech.tts.Voice;
import android.util.Log;
import dev.bytesizedfox.microdectalktest.App;
import dev.bytesizedfox.microdectalktest.MainActivity;

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
    private static final int SAMPLE_RATE = 11025; // Adjust based on your audio sample rate
    private static final Locale DEFAULT_LOCALE = Locale.US;
    public static Voice mDefaultVoice;

    public native void SetCallback();

    public static SynthesisCallback mainCallback;

    public void javaCallback(short[] iwave, int length) {
        // volume adjustment
        for (int i = 0; i < iwave.length; i++) {
            if (App.current_volume < 0) {
                App.current_volume = 0;
            }
            float percent = (App.current_volume / 100.0f);
            //System.out.println("Volume: " + String.valueOf(App.current_volume));
            iwave[i] = (short) ( (float)iwave[i] * percent - 0.5f);
        }

        byte[] audioData = new byte[length * 2];
        ByteBuffer.wrap(audioData)
                .order(ByteOrder.LITTLE_ENDIAN)
                .asShortBuffer()
                .put(iwave);
        mainCallback.audioAvailable(audioData,0, length * 2);
        //System.out.println("Audio Triggered!");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        // load the library for TTS
        //System.loadLibrary("epsonapi");

        Log.d(TAG, "CustomTTSService created");
        Set<String> features = new HashSet<>();
        mDefaultVoice = new Voice("default", DEFAULT_LOCALE, Voice.QUALITY_VERY_HIGH, Voice.LATENCY_NORMAL, false, features);
        App.TextToSpeechInit();

        //new MainActivity().SetCallback();
        SetCallback();
    }

    @Override
    protected String[] onGetLanguage() {
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
        return TextToSpeech.LANG_AVAILABLE;
    }

    @Override
    public String onGetDefaultVoiceNameFor(String language, String country, String variant) {
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
        App.TextToSpeechReset();
    }

    @Override
    protected synchronized void onSynthesizeText(SynthesisRequest request, SynthesisCallback callback) {
        // Get the text to synthesize
        String text = "[:phoneme on] " + request.getCharSequenceText().toString().replaceAll("\\P{ASCII}", "");
        Log.w("pitch", String.valueOf(request.getPitch()));
        Log.w("speech_rate", String.valueOf(request.getSpeechRate()));

        mainCallback = callback;

        System.out.println("Synthesize text");

        ((App) getApplication().getApplicationContext()).loadPrefs(); // load the preferences if they haven't been already

        try {
            System.out.println("A");
            // Set the audio format properties
            callback.start(SAMPLE_RATE, AudioFormat.ENCODING_PCM_16BIT, 1);

            System.out.println("B");
            // reset TTS to avoid bugginess
            App.TextToSpeechReset();

            App.TextToSpeechInit();
            App.TextToSpeechStart("", true);
            App.TextToSpeechChangeVoice(nameToNameCode(App.current_voice));

            System.out.println("C");

            // Get audio samples using native method
            App.TextToSpeechSetRate( 180 + ((App.rate-50)*4) );

            System.out.println("D");

            Log.w("voice AP", String.valueOf(App.TextToSpeechGetSpdefValue(3 /* SP_AP */) ));
            App.TextToSpeechSetVoiceParam("ap", App.TextToSpeechGetSpdefValue(3 /* SP_AP */) + ((App.pitch-50)*2) );
            Log.w("voice AP 1", String.valueOf(App.TextToSpeechGetSpdefValue(3 /* SP_AP */) ));
            Log.w("Pitch", String.valueOf( App.pitch ));

            System.out.println("E");

            // the JNI callback writes the samples to the callback
            App.TextToSpeechStart(text, true);

            System.out.println("F");

            // Signal completion
            callback.done();

            System.out.println("G");

        } catch (Exception e) {
            Log.e(TAG, "Error synthesizing text", e);
            callback.error();
        }
    }

}