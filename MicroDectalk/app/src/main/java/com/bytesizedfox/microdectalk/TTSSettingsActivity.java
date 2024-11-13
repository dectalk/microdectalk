package com.bytesizedfox.microdectalk;

import android.os.Bundle;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class TTSSettingsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Create a simple TextView to display a message
        TextView textView = new TextView(this);
        textView.setPadding(32, 32, 32, 32);
        textView.setText("Text-to-Speech Settings\n\nNo configurable settings are available at this time.");

        // Set the TextView as the content view
        setContentView(textView);

        // Set the title of the activity
        setTitle("TTS Settings");
    }
}