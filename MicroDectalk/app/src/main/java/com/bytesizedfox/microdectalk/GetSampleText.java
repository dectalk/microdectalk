package com.bytesizedfox.microdectalk;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;

/**
 *
 */
public class GetSampleText extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int result = TextToSpeech.LANG_AVAILABLE;
        Intent returnData = new Intent();

        returnData.putExtra("sampleText", "Hello World, How Are You Today?");

        setResult(result, returnData);
        finish();
    }
}