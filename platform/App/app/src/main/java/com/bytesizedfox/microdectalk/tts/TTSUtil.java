package com.bytesizedfox.microdectalk.tts;

import static com.bytesizedfox.microdectalk.MainActivity.TextToSpeechChangeVoice;

public class TTSUtil {
    public static String nameToNameCode(String name) {
        String voice = name.toLowerCase();
        switch (voice) {
            case "betty":
                return "nb";
            case "harry":
                return "nh";
            case "frank":
                return "nf";
            case "dennis":
                return "nd";
            case "kit":
                return "nk";
            case "ursula":
                return "nu";
            case "rita":
                return "nr";
            case "wendy":
                return "nw";
            default:
                return "np";
        }
    }
}
