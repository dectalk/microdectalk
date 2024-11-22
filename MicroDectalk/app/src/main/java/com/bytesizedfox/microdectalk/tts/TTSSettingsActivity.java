package com.bytesizedfox.microdectalk.tts;

import android.content.Intent;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

import com.bytesizedfox.microdectalk.App;
import com.bytesizedfox.microdectalk.MainActivity;

public class TTSSettingsActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) { // just redirect to MainActivity
        super.onCreate(savedInstanceState);
        Intent myIntent = new Intent(this, MainActivity.class);
        App.openSettings = true;
        startActivity(myIntent);
        finish();
    }
}