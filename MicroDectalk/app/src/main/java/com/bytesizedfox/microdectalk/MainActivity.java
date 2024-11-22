package com.bytesizedfox.microdectalk;

import static com.bytesizedfox.microdectalk.tts.TTSUtil.nameToNameCode;

import android.content.SharedPreferences;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.provider.Settings;
import android.speech.tts.TextToSpeech;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements TextToSpeech.OnInitListener {
    private TextToSpeech tts;

    // JNI functions
    public static native void TextToSpeechInit();
    public static native short[] TextToSpeechStart(String text);
    public static native void TextToSpeechReset();
    public static native void TextToSpeechChangeVoice(String name);
    public static native void TextToSpeechSetRate(int rate);
    public static native void TextToSpeechSetVoiceParam(String cmd, int value);
    public static native short TextToSpeechGetSpdefValue(int index);

    // Activity Views
    public Button speakButton;
    public Spinner voiceSpinner;
    public SeekBar volumeBar;
    public SeekBar pitchBar;
    public SeekBar rateBar;
    public AlertDialog dialog;

    public static void setVolume(int value) {
        App.current_volume = value;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        tts = new TextToSpeech(this, this, "com.bytesizedfox.microdectalk");
        setContentView(R.layout.activity_main);
        setSupportActionBar(findViewById(R.id.my_toolbar));

        TextToSpeechInit();

        EditText inputText = findViewById(R.id.inputText);
        speakButton = findViewById(R.id.speakButton);
        speakButton.setOnClickListener(v -> {
            tts.speak(inputText.getText().toString(), TextToSpeech.QUEUE_FLUSH, null, null);
        });



        initSettings();
        TextToSpeechChangeVoice(nameToNameCode(App.current_voice));

        if (App.openSettings) { // if requested to open settings
            this.showSettings();
        }


    }

    @Override
    public boolean onCreateOptionsMenu( Menu menu ) {
        getMenuInflater().inflate(R.menu.menu, menu);
        // fill menu
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this,
                R.array.voice_names,
                android.R.layout.simple_spinner_item
        );
        for (int i = 0; i < adapter.getCount(); i++) {
            menu.findItem(R.id.voiceSettings).getSubMenu().add(adapter.getItem(i));
        }

        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        return true;
    }

    // methods to control the operations that will
    // happen when user clicks on the action buttons
    @Override
    public boolean onOptionsItemSelected( @NonNull MenuItem item ) {
        int id = item.getItemId();

        if (id == R.id.voiceSettings) {
            return super.onOptionsItemSelected(item);
        }

        if (this.voiceList.contains(item.getTitle().toString().toLowerCase())) {
            String name = item.getTitle().toString();
            TextToSpeechChangeVoice(nameToNameCode(name));
            App.current_voice = name.toLowerCase();
            Toast.makeText(this, "voice set to " + name, Toast.LENGTH_SHORT).show();
            tts.speak(item.getTitle(), TextToSpeech.QUEUE_FLUSH, null, null);
        }

        if (id == R.id.activity_settings) {
            this.showSettings();
        }


        return super.onOptionsItemSelected(item);
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

    List<String> voiceList = Arrays.asList(
            "paul",
            "betty",
            "harry",
            "frank",
            "dennis",
            "kit",
            "ursula",
            "rita",
            "wendy"
    );
    View dialogView;
    private void initSettings() {
        AlertDialog.Builder builder = new MaterialAlertDialogBuilder(this);
        builder.setView(R.layout.settings_overlay);

        dialogView = LayoutInflater.from(this).inflate(R.layout.settings_overlay, null);
        builder.setView(dialogView);

        voiceSpinner = dialogView.findViewById(R.id.VoiceSpinner);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this,
                R.array.voice_names,
                android.R.layout.simple_spinner_item
        );
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Set the adapter to the Spinner
        voiceSpinner.setAdapter(adapter);

        // for immediate settings save
        SharedPreferences pref = this.getSharedPreferences("settings", MODE_PRIVATE);

        voiceSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                updateBars();

                String voiceID = (String) parent.getItemAtPosition(parent.getSelectedItemPosition());
                TextToSpeechChangeVoice(nameToNameCode(voiceID));

                tts.speak(voiceID, TextToSpeech.QUEUE_FLUSH, null, null);

                // sync to disk
                App.current_voice = voiceID.toLowerCase();
                SharedPreferences.Editor editor = pref.edit();
                editor.putString("voice", App.current_voice);
                editor.apply();
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });


        volumeBar = dialogView.findViewById(R.id.volumeBar);
        volumeBar.setProgress((int)App.current_volume);
        volumeBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                MainActivity.setVolume(progress);
                // sync to disk
                SharedPreferences.Editor editor = pref.edit();
                editor.putFloat("volume", progress);
                editor.apply();
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        pitchBar = dialogView.findViewById(R.id.pitchBar);
        pitchBar.setProgress(App.pitch);
        pitchBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                SharedPreferences.Editor editor = pref.edit();
                App.pitch = progress;
                editor.putInt("pitch", progress);
                editor.apply();
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        rateBar = dialogView.findViewById(R.id.rateBar);
        rateBar.setProgress(App.rate);
        rateBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                SharedPreferences.Editor editor = pref.edit();
                App.rate = progress;
                editor.putInt("rate", progress);
                editor.apply();
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        builder.setPositiveButton("Continue", (dialog, which) -> {
            dialog.dismiss();
        });

        dialog = builder.create();
        dialog.setCancelable(false);
        dialog.setCanceledOnTouchOutside(false);

        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));

        Button applySettingsButton = dialogView.findViewById(R.id.ApplySettingsButton);
        applySettingsButton.setOnClickListener(v -> {
            App.openSettings = false;
            dialog.dismiss();
            if (App.openSettings) {
                App.openSettings = false;
                finish();
            }
        });


    }



    void updateBars() {
        if (!App.supportsAccessibility) {
            return;
        }
        try {
            int pitch = Settings.Secure.getInt(getContentResolver(), Settings.Secure.TTS_DEFAULT_PITCH);
            int rate = Settings.Secure.getInt(getContentResolver(), Settings.Secure.TTS_DEFAULT_RATE);

            SharedPreferences pref = this.getSharedPreferences("settings", MODE_PRIVATE);

            if (rate != App.last_system_rate) {
                App.last_system_rate = rate;
                App.rate = rate / 4;
                pref.edit().putInt("last_rate", rate).apply();
                rateBar.setProgress(rate / 4);
            }
            if (pitch != App.last_system_pitch) {
                App.last_system_pitch = pitch;
                App.pitch = pitch / 4;
                pref.edit().putInt("last_pitch", pitch).apply();
                pitchBar.setProgress(pitch / 4);
            }
        } catch (Exception e) {
            App.supportsAccessibility = false;
        }
    }

    // use these to update values when changing accessability
    @Override
    public void onResume() {
        super.onResume();
        updateBars();
    }
    @Override
    public void onPause() {
        super.onPause();
        updateBars();
    }

    private void showSettings() {
        // load user voice setting
        dialog.show();

        voiceSpinner.setSelection(voiceList.indexOf(App.current_voice));
        volumeBar.setProgress((int)App.current_volume);

        // magic code that hides the cancel button
        Button button_negative = dialog.getButton(AlertDialog.BUTTON_NEGATIVE);
        button_negative.setEnabled(false);
        button_negative.setAlpha(0);
        button_negative.setVisibility(View.GONE);

        // change ok to Apply
        Button button_positive = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
        button_positive.setEnabled(false);
        button_positive.setAlpha(0);
        button_positive.setVisibility(View.GONE);
    }


    @Override
    public void onInit(int status) {
        if (status == TextToSpeech.SUCCESS) {
            // Set language to US English
            int result = tts.setLanguage(Locale.US);
            speakButton.setEnabled(true);
            if (result == TextToSpeech.LANG_MISSING_DATA || result == TextToSpeech.LANG_NOT_SUPPORTED) {
                speakButton.setEnabled(false);
            }
        } else {
            speakButton.setEnabled(false);
        }
    }
}