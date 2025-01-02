package com.bytesizedfox.microdectalk;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import androidx.core.content.FileProvider;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

// application entrypoint to allow TTS access regardless of activity
public class App extends Application {
    // global states
    public static String current_voice = "paul";
    public static float current_volume = 0.0f;
    public static int rate = 200;
    public static int pitch = 120;
    public static int last_system_pitch = 0;
    public static int last_system_rate = 0;
    public static boolean supportsAccessibility = true;
    public static boolean openSettings = false;



    @Override
    public void onCreate() {
        super.onCreate();
        Log.w("TtsApp","starting TTS API!");
        System.loadLibrary("epsonapi");
        SharedPreferences pref = this.getSharedPreferences("settings", MODE_PRIVATE);

        try {
            App.current_voice = pref.getString("voice", "paul");
            App.current_volume = pref.getFloat("volume", 50.0f);
            App.pitch = pref.getInt("pitch", 50);
            App.rate = pref.getInt("rate", 50);
            App.last_system_pitch = pref.getInt("last_pitch", 0);
            App.last_system_rate = pref.getInt("last_rate", 0);
        } catch (Exception e) { // if something goes wrong, reset preferences
            pref.edit().clear().commit();
            App.current_voice = "paul";
            App.current_volume = 90;
            App.pitch = 50;
            App.rate = 50;
        }
    }

    public static File writeWavFile(Context context, short[] audioData) throws IOException {
        File outputFile = File.createTempFile("audio_", ".wav", context.getCacheDir());
        int dataSize = audioData.length * 2;  // 2 bytes per sample

        try (DataOutputStream out = new DataOutputStream(
                new BufferedOutputStream(new FileOutputStream(outputFile)))) {

            // RIFF header
            out.writeBytes("RIFF");
            out.writeInt(Integer.reverseBytes(36 + dataSize));
            out.writeBytes("WAVE");

            // fmt chunk
            out.writeBytes("fmt ");
            out.writeInt(Integer.reverseBytes(16));
            out.writeShort(Short.reverseBytes((short) 1));  // PCM
            out.writeShort(Short.reverseBytes((short) 1));  // Mono
            out.writeInt(Integer.reverseBytes(11025));      // Sample rate
            out.writeInt(Integer.reverseBytes(11025 * 2));  // Byte rate
            out.writeShort(Short.reverseBytes((short) 2));  // Block align
            out.writeShort(Short.reverseBytes((short) 16)); // Bits per sample

            // Data chunk
            out.writeBytes("data");
            out.writeInt(Integer.reverseBytes(dataSize));

            // Audio data
            for (short sample : audioData) {
                out.writeShort(Short.reverseBytes(sample));
            }

            return outputFile;
        }
    }
    public static void shareAudioFile(Context context, File audioFile) {
        try {
            Uri fileUri = FileProvider.getUriForFile(
                    context,
                    context.getApplicationContext().getPackageName() + ".fileprovider",
                    audioFile
            );

            Intent shareIntent = new Intent(Intent.ACTION_SEND);
            shareIntent.setType("audio/wav");
            shareIntent.putExtra(Intent.EXTRA_STREAM, fileUri);
            shareIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

            // Start the share dialog
            context.startActivity(Intent.createChooser(shareIntent, "Share Audio File"));

        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            Toast.makeText(context, "Error sharing file", Toast.LENGTH_SHORT).show();
        }
    }

    public static File currentAudioFile;
    public static void saveAudioFile(Activity activity, File audioFile) {
        currentAudioFile = audioFile;
        Intent intent = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("audio/wav");
        intent.putExtra(Intent.EXTRA_TITLE, "recording.wav");
        activity.startActivityForResult(intent, 1001);
    }
}
