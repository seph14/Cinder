package org.libcinder.android;

import android.app.NativeActivity;
import android.content.Context;
import android.os.PowerManager;
import android.util.Log;

public class AndroidCinderActivity extends NativeActivity
{
    static final String TAG = "AndroidCinderActivity";

    PowerManager.WakeLock mWakeLock;

    @Override
    protected void onResume()
    {
        super.onResume();

		  PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE, TAG);
        mWakeLock.acquire();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        mWakeLock.release();
    }

    public void helloCinder()
    {
        Log.v(TAG, "Hello Cinder!");
    }
}
