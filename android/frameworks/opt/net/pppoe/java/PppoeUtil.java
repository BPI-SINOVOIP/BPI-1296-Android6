package com.android.server.pppoe;

import android.os.RemoteException;
import android.net.ConnectivityManager;
import android.os.ServiceManager;
import android.content.Context;
import android.util.Log;
import android.net.NetworkFactory;
import android.os.Looper;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.SystemProperties;

import android.os.RemoteCallbackList;
import android.net.IEthernetServiceListener;
import java.util.concurrent.atomic.AtomicBoolean;
import android.content.Intent;

public class PppoeUtil {

    private static boolean DBG = true;
    private static final String TAG = "PppoeUtil";

    private Handler mHandler;
    private PppoeNetworkFactory mTracker;
    private final RemoteCallbackList<IEthernetServiceListener> mListeners =
            new RemoteCallbackList<IEthernetServiceListener>();

    private static Context mCtx;
    private static PppoeUtil mInst = null;
    private final AtomicBoolean mStarted = new AtomicBoolean(false);

    public static PppoeUtil getInstance(Context ctx){
        if(mInst==null){
            mInst = new PppoeUtil();
        }
        mCtx = ctx;
        return mInst;
    }

    private void startFactory(){
        logi("Starting Pppoe Factory");

        mTracker = new PppoeNetworkFactory(mListeners);
        HandlerThread handlerThread = new HandlerThread("PppoeThread");
        handlerThread.start();
        mHandler = new Handler(handlerThread.getLooper());
        mTracker.start(mCtx, mHandler);

        mStarted.set(true);
    }

    private void stopFactory(){
        mStarted.set(false);
        if(mTracker!=null){
            mTracker.stop();
        }
    }

    public int setup(String iface, String user, String password){
        return PppoeNative.setup(iface, user, password);
    }

    public static final String PPPOE_STATE_CHANGED_ACTION = "PPPOE_STATE_CHANGED_ACTION"; //Used for state update.
    public static final String PPPOE_SET_INTERFACE = "PPPOE_SET_INTERFACE"; //Used for EthernetNetworkFactory
    public static final String EXTRA_PPPOE_STATE = "EXTRA_PPPOE_STATE";
    public int start(){
        int res = -1;
        loadDriver();
        res = PppoeNative.start();
        if(res<0) {
            unloadDriver();
            return res;
        }
        // startFactory();
        Intent it = new Intent(PPPOE_SET_INTERFACE);
        it.putExtra(EXTRA_PPPOE_STATE, 1);
        mCtx.sendBroadcast(it);
        it = new Intent(PPPOE_STATE_CHANGED_ACTION);
        it.putExtra(EXTRA_PPPOE_STATE, 1);
        mCtx.sendBroadcast(it);
        return res;
    }
    public int stop(){
        Intent it = new Intent(PPPOE_SET_INTERFACE);
        it.putExtra(EXTRA_PPPOE_STATE, 0);
        mCtx.sendBroadcast(it);
        // stopFactory();
        int ret = PppoeNative.stop();
        unloadDriver();
        it = new Intent(PPPOE_STATE_CHANGED_ACTION);
        it.putExtra(EXTRA_PPPOE_STATE, 1);
        mCtx.sendBroadcast(it);
        return ret;
    }

    private void loadDriver(){
        SystemProperties.set("rtk.net.pppoe.driver", "1");
    }

    private void unloadDriver(){
        SystemProperties.set("rtk.net.pppoe.driver", "0");
    }

    public int isUp(){
        String prop = SystemProperties.get("net.pppoe.status", "");
        logi("pppoe state="+prop);
        if(prop.equals("connected"))
            return 1;
        else
            return 0;
    }

    private static void logi(String s){
        if(DBG)
            Log.i(TAG, s);
    }
}