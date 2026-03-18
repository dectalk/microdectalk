package dev.bytesizedfox.microdectalktest.tts;

import android.content.Intent;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

import dev.bytesizedfox.microdectalktest.App;
import dev.bytesizedfox.microdectalktest.MainActivity;

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