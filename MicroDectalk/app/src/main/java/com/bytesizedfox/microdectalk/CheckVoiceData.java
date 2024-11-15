package com.bytesizedfox.microdectalk;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Respond to android.speech.tts.engine.CHECK_TTS_DATA
 */
 public class CheckVoiceData extends Activity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ArrayList<String> available = new ArrayList<String>();
        available.add("en-US");


        Intent returnData = new Intent();
        returnData.putStringArrayListExtra(TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES, available);
        returnData.putStringArrayListExtra(TextToSpeech.Engine.EXTRA_UNAVAILABLE_VOICES, new ArrayList<>());
        setResult(TextToSpeech.Engine.CHECK_VOICE_DATA_PASS, returnData);
        finish();
    }
}