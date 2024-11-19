package com.bytesizedfox.microdectalk;

import android.app.Application;
import android.content.SharedPreferences;
import android.util.Log;

// application entrypoint to allow TTS access regardless of activity
public class App extends Application {
    public static String current_voice = "paul";
    public static float current_volume = 0.0f;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.w("TtsApp","starting TTS API!");
        System.loadLibrary("epsonapi");
        SharedPreferences pref = this.getSharedPreferences("settings", MODE_PRIVATE);
        App.current_voice = pref.getString("voice", "paul");
        App.current_volume = pref.getFloat("volume", 50.0f);


    }


}
