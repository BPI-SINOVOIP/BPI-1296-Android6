package com.realtek.record;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

/**
 *  MainActivity class
 *  used to start or stop TopFloatService
 */
public class MainActivity extends Activity {
    public static final String TAG = "MainActivity";
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate");
        if(!TopFloatService.mServiceIsAlive) {
            // start service
            Intent serviceIntent = new Intent(MainActivity.this, com.realtek.record.TopFloatService.class);
            MainActivity.this.startService(serviceIntent);
            Log.d(TAG, "start TopFloatService");
            Toast.makeText(MainActivity.this, "start PIP record demo", Toast.LENGTH_SHORT).show();
        }
        else {
            // stop service
            Intent serviceIntent = new Intent(MainActivity.this, com.realtek.record.TopFloatService.class);
            MainActivity.this.stopService(serviceIntent);
            Log.d(TAG, "stop TopFloatService");   
            Toast.makeText(MainActivity.this, "stop PIP record demo", Toast.LENGTH_SHORT).show();            
        }
        finish();
    }
}