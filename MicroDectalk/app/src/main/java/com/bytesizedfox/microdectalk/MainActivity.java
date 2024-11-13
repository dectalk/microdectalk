package com.bytesizedfox.microdectalk;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import androidx.appcompat.app.AppCompatActivity;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class MainActivity extends AppCompatActivity {
    public void playWavFile(short[] samples) {
        int bufferSize = AudioTrack.getMinBufferSize(
                8000,
                AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT
        );
        AudioTrack audioTrack = new AudioTrack. Builder()
                .setAudioAttributes(new AudioAttributes. Builder()
                        .setUsage(AudioAttributes. USAGE_MEDIA)
                        .setContentType(AudioAttributes. CONTENT_TYPE_MUSIC)
                        .build())
                .setAudioFormat(new AudioFormat. Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(8000)
                        .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                        .build())
                .setBufferSizeInBytes(bufferSize)
                .build();

        audioTrack.play();

        byte[] bytes = new byte[samples.length * 2];
        ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().put(samples);

        // Write audio data to track
        audioTrack.write(bytes, 0, bytes.length);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        System.loadLibrary("epsonapi");

        EditText inputText = findViewById(R.id.inputText);
        Button speakButton = findViewById(R.id.speakButton);

        speakButton.setOnClickListener(v -> {
            short[] samples = stringFromJNI(inputText.getText().toString());
            Log.w("SAMPLES ", String.valueOf(samples.length));
            playWavFile(samples);
        });

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public static native short[] stringFromJNI(String text);
}