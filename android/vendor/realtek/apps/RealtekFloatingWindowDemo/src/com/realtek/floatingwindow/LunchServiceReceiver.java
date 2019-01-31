package com.realtek.floatingwindow;

import android.os.Bundle;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.media.AudioManager;
import android.os.SystemProperties;

/**
 * Realtek Floating Window Demo
 */
public class LunchServiceReceiver extends BroadcastReceiver {

    static final String TAG= "LunchServiceReceiver" ;

    private static final String LUNCH_ACTION = "android.intent.action.LUNCH_FW_SERVICE";
    private static final String STOP_ACTION = "android.intent.action.STOP_FW_SERVICE";

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        Log.d(TAG, "intent: "+action);

        if(action.equals(LUNCH_ACTION)){
            startUSBCamFloatingWindowService(context);
        }else if(action.equals(STOP_ACTION)){
            stopUSBCamFloatingWindowService(context);
        }

        // start service, 
        //Intent serviceIntent = new Intent(context, com.realtek.floatingwindow.FloatingWindowService.class);
        //context.startService(serviceIntent);

    }

    private Bundle getSettingConfig(){
        Bundle bundle = new Bundle();
        boolean enabled = true;
        int viewType = SettingWidgets.TYPE_SURFACEVIEW;
        boolean record = false;
        int windowSize = SettingWidgets.WINDOW_HD_1280;

        bundle.putBoolean(SettingWidgets.KEY_ENABLED,enabled);
        bundle.putInt(SettingWidgets.KEY_VIEWTYPE,viewType);
        bundle.putBoolean(SettingWidgets.KEY_RECORD,record);
        bundle.putInt(SettingWidgets.KEY_SIZE,windowSize);

        return bundle;
    }

    private void stopUSBCamFloatingWindowService(Context c){
        if(FloatingWindowService2.bServiceIsAlive){
            Intent serviceIntent = new Intent(
                c, com.realtek.floatingwindow.FloatingWindowService2.class);
            c.stopService(serviceIntent);
        }
    }

    private void startUSBCamFloatingWindowService(Context c){

        if(FloatingWindowService2.bServiceIsAlive){
            return;
        }

        Bundle arg = getSettingConfig();
        // start service
        Intent serviceIntent = new Intent(
            c, com.realtek.floatingwindow.FloatingWindowService2.class);

        serviceIntent.putExtra("usbcam",arg);
        c.startService(serviceIntent);
    }
}

