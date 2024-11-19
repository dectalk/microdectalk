package com.bytesizedfox.microdectalk;

import static com.bytesizedfox.microdectalk.tts.TTSUtil.nameToNameCode;

import android.content.SharedPreferences;
import android.os.Bundle;
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

import java.util.Arrays;
import java.util.List;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements TextToSpeech.OnInitListener {

    private TextToSpeech tts;
    Button speakButton;
    Spinner voiceSpinner;
    SeekBar volumeBar;
    AlertDialog dialog;


    // JNI functions
    public static native void TextToSpeechInit();
    public static native short[] TextToSpeechStart(String text);
    public static native void TextToSpeechReset();
    public static native void TextToSpeechChangeVoice(String name);
    public static native void TextToSpeechSetRate(int rate);
    public static native void TextToSpeechSetVoiceParam(String cmd, int value);
    public static native short TextToSpeechGetSpdefValue(int index);

    public static void setVolume(int value) {
        App.current_volume = value;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setSupportActionBar(findViewById(R.id.my_toolbar));

        TextToSpeechInit();

        // Initialize TTS engine
        tts = new TextToSpeech(this, this, "com.bytesizedfox.microdectalk");

        EditText inputText = findViewById(R.id.inputText);
        speakButton = findViewById(R.id.speakButton);
        speakButton.setOnClickListener(v -> {
            tts.speak(inputText.getText().toString(), TextToSpeech.QUEUE_FLUSH, null, null);
        });

        initSettings();
        TextToSpeechChangeVoice(nameToNameCode(App.current_voice));
    }
    @Override
    public boolean onCreateOptionsMenu( Menu menu ) {
        getMenuInflater().inflate(R.menu.menu, menu);

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

        if (id == R.id.activity_settings) {
            Toast.makeText(this, "Settings Pressed!", Toast.LENGTH_SHORT).show();
            this.showSettings();
        } else if (id == R.id.setVoicePaul){
            TextToSpeechChangeVoice(nameToNameCode("paul"));
            App.current_voice = "paul";
            Toast.makeText(this, "voice set to Paul", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceBetty) {
            TextToSpeechChangeVoice(nameToNameCode("betty"));
            App.current_voice = "betty";
            Toast.makeText(this, "voice set to Betty", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceHarry) {
            TextToSpeechChangeVoice(nameToNameCode("harry"));
            App.current_voice = "harry";
            Toast.makeText(this, "voice set to Harry", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceFrank) {
            TextToSpeechChangeVoice(nameToNameCode("frank"));
            App.current_voice = "frank";
            Toast.makeText(this, "voice set to Frank", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceDennis) {
            TextToSpeechChangeVoice(nameToNameCode("dennis"));
            App.current_voice = "dennis";
            Toast.makeText(this, "voice set to Dennis", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoicekit) {
            TextToSpeechChangeVoice(nameToNameCode("kit"));
            App.current_voice = "kit";
            Toast.makeText(this, "voice set to Kit", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceUrsula) {
            TextToSpeechChangeVoice(nameToNameCode("ursula"));
            App.current_voice = "ursula";
            Toast.makeText(this, "voice set to Ursula", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceRita) {
            TextToSpeechChangeVoice(nameToNameCode("rita"));
            App.current_voice = "rita";
            Toast.makeText(this, "voice set to Rita", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.setVoiceWendy) {
            TextToSpeechChangeVoice(nameToNameCode("wendy"));
            App.current_voice = "wendy";
            Toast.makeText(this, "voice set to Wendy", Toast.LENGTH_SHORT).show();
        }

        tts.speak(item.getTitle(), TextToSpeech.QUEUE_FLUSH, null, null);
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

    private void initSettings() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setView(R.layout.settings_overlay);
        View dialogView = LayoutInflater.from(this).inflate(R.layout.settings_overlay, null);
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
        builder.setPositiveButton("Continue", (dialog, which) -> {
            dialog.dismiss();
        });

        dialog = builder.create();
        dialog.setCancelable(false);
        dialog.setCanceledOnTouchOutside(false);
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

        // change ok to Apply
        Button button_positive = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
        button_positive.setText("Apply");
    }

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