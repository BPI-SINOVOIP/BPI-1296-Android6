package com.realtek.record;

import android.app.Application;
import android.content.Context;
import android.util.Log;
import android.view.WindowManager;

public class MyApplication extends Application {
    public static final String TAG = "MyApplication";

	private WindowManager.LayoutParams wmParams = new WindowManager.LayoutParams();

    
	public WindowManager.LayoutParams getMywmParams() {
        Log.d(TAG, "get WindowManager.LayoutParams");
		return wmParams;
	}  
}
