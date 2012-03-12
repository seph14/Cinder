package com.expandingbrain.celaudio;

import android.app.NativeActivity;

public class BasicAudioActivity extends NativeActivity
{
    static {
        System.loadLibrary("pdnative");
        System.loadLibrary("BasicAudio");
    }
}

