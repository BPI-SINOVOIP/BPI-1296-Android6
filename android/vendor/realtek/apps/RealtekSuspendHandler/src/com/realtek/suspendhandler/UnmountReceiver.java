package com.realtek.suspendhandler;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.util.Slog;

/**
 * Realtek system suspend/resume handler
 * maybe useless in the future
 */
public class UnmountReceiver extends BroadcastReceiver {

    static final String TAG= "RealtekSuspendUnmount" ;

    // use service to manage wakeLock
    private SuspendService mService;
    public UnmountReceiver(SuspendService service){
        mService = service;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Slog.d(TAG, "onReceive :"+intent.getAction()+" "+intent.getDataString());
        // check if is during suspend mode
        if(mService.mIsSuspendMode){
            Slog.d(TAG, "unmount counter : "+mService.mUnmountCounter);
            mService.mUnmountCounter = mService.mUnmountCounter - 1;

            String upath = intent.getDataString();
            // then check if is in sdcard List
            for(String path: mService.mSDCardList){
                if(upath.contains(path)){
                    mService.mUnmountedSDCardList.add(path);
                    Slog.d(TAG, "SDCard "+path+" is unmounted");
                }
            }

            if(mService.mUnmountCounter == 0){
                long tid = SuspendService.getCurrentThreadID();
                Slog.d(TAG, "tid=("+tid+") unmount completed, release wake_lock, remove pending intent? , leave suspend mode");
                mService.removeDelayMessage();
                mService.mIsSuspendMode = false;
                mService.releaseWakeLockDelay(1);
            }
        }
    }
}
