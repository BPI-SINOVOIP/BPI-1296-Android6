/*
 * Copyright (c) 2013 Realtek Semi. co. All Rights Reserved.
 * Author Wen
 * Version V0.1
 * Date:  2013-12-30
 * Comment:This class lets you access the CEC function.
 */

package com.realtek.hardware;

import java.util.StringTokenizer;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.view.KeyEvent;

import com.realtek.hardware.IRtkCECService;

public class RtkCECManager {
	private static String TAG = "RtkCECManager";
	private IRtkCECService mRtkCECService;
	
	public RtkCECManager()
	{
		//Connect to RtkCECService process
		mRtkCECService = IRtkCECService.Stub.asInterface(ServiceManager.getService("RtkCECService"));
		if (!IsCECServerAlive())
		{
			Log.e(TAG, "Can not get CEC service, please check CEC service is register into AndroidService");
		}
	}
	private boolean IsCECServerAlive()
	{
		boolean ret;
		if (mRtkCECService != null)
		{
			Log.i(TAG,"Got CEC Service"+mRtkCECService.toString());
			ret = true;
		}
		else
		{
            //Try to Get CEC service again
            mRtkCECService = IRtkCECService.Stub.asInterface(ServiceManager.getService("RtkCECService"));
            if (mRtkCECService == null)
            {
			    Log.e(TAG, "error! CANNOT get RtkCECService!");
			    ret = false;
            }
            else
            {
                ret = true;
            }
		}
		return ret;
	}

	public boolean doOneTouchPlay(int Key)
	{
		boolean ret =false;
        if (Key == KeyEvent.KEYCODE_HOME) 
        {
	        if (IsCECServerAlive())
	        {
	        	try {
	        		ret = mRtkCECService.doOneTouchPlay(Key);
	        	}catch (Exception e){
	        		Log.e(TAG,"Exception Error! call mRtkCECService.doOneTouchPlay()");
	        	}
	        }
        }
		return ret;
	}
    public int SendRemotePressed(int Dev, int Key, boolean SendReleased)
    {
        int ret = -3;
	    if (IsCECServerAlive())
	    {
	    	try {
	    		ret = mRtkCECService.SendRemotePressed(Dev, Key, SendReleased);//return for JNI: 0:OK, -1:Send Fail, -2:Can not find Dev.
	    	}catch (Exception e){
	    		Log.e(TAG,"Exception Error! call mRtkCECService.doOneTouchPlay()");
                ret = -3;
	    	}
	    }
        return ret;
	}
}//End of RtkCECManager
