package com.bytesizedfox.microdectalk;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import androidx.appcompat.app.AppCompatActivity;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements TextToSpeech.OnInitListener {

    private TextToSpeech tts;
    Button speakButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        System.loadLibrary("epsonapi");

        TextToSpeechInit();

        // Initialize TTS engine
        tts = new TextToSpeech(this, this);

        EditText inputText = findViewById(R.id.inputText);
        speakButton = findViewById(R.id.speakButton);

        speakButton.setOnClickListener(v -> {
            //short[] samples = TextToSpeechStart(inputText.getText().toString());
            //Log.w("SAMPLES ", String.valueOf(samples.length));
            //playWavFile(samples);
            tts.speak(inputText.getText().toString(), TextToSpeech.QUEUE_FLUSH, null, null);
        });

    }

    @Override
    protected void onDestroy() {
        TextToSpeechReset();

        // Release TTS resources
        if (tts != null) {
            tts.stop();
            tts.shutdown();
        }
        super.onDestroy();
    }

    public static native void TextToSpeechInit();
    public static native short[] TextToSpeechStart(String text);
    public static native void TextToSpeechReset();

    @Override
    public void onInit(int status) {
        if (status == TextToSpeech.SUCCESS) {
            // Set language to US English
            int result = tts.setLanguage(Locale.US);

            if (result == TextToSpeech.LANG_MISSING_DATA ||
                    result == TextToSpeech.LANG_NOT_SUPPORTED) {
                speakButton.setEnabled(false);
            }
        } else {
            speakButton.setEnabled(false);
        }
    }

}