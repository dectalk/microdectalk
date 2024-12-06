package com.bytesizedfox.microdectalk;

import android.app.Application;
import android.content.SharedPreferences;
import android.util.Log;

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
}
