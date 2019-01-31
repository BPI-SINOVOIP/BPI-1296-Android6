package com.android.systemui;

import android.os.Handler;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import android.net.NetworkInfo;
import android.net.ConnectivityManager;
import android.net.wifi.WifiManager;
import android.net.EthernetManager;
import android.widget.Toast;
import com.android.systemui.R;

public class NotificationBroadcastReceiver extends BroadcastReceiver{

    boolean mEthAvailable = true;
    String eth0Str = null;
    //String disconnectStr = null;
    private static final Handler mHandler = new Handler();
    private ToastRunnable mToastRunnable;

    private class ToastRunnable implements Runnable{
        private Context context;

        ToastRunnable(Context c){
            context = c;
        }

        public void run (){
            Log.d("RTK","show toast");
            Toast.makeText(context, eth0Str ,Toast.LENGTH_SHORT).show();
        }
    }

	public NotificationBroadcastReceiver(Context c){
        mToastRunnable = new ToastRunnable(c);
        ConnectivityManager connectMgr = (ConnectivityManager)c.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = null;
        if (connectMgr != null) {
            /*
            networkInfo = connectMgr.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
            if (networkInfo != null) {
                mWifiAvailable = networkInfo.isAvailable();
                Log.d(LOGTAG, "onCreate - mWifiAvailable = " + mWifiAvailable);
            }
            */
            networkInfo = connectMgr.getNetworkInfo(ConnectivityManager.TYPE_ETHERNET);
            if (networkInfo != null) {
                mEthAvailable = networkInfo.isAvailable();
                Log.d("RTK", "onCreate - mEthAvailable = " + mEthAvailable);
            }
        }
        eth0Str = c.getResources().getString(R.string.ethernet_label)+": "+
                c.getResources().getString(R.string.quick_settings_wifi_not_connected);
	}

    public void onReceive (Context context, Intent intent){
        String action = intent.getAction();
        Log.d("RTK","NotificationBroadcastReceiver - onReceive:"+action);
        /*
        if(action.equals("com.android.action.EXPAND_NOTIFICATION")){
            //mPanelView.onPanelShowing();
            if(mPanelView.mStatusBar!=null){
                if(mPanelView.isFullyExpanded()){
                    mPanelView.mStatusBar.animateCollapsePanels();
                }else{
                    //mPanelView.mStatusBar.animateExpandNotificationsPanel();
                    mPanelView.doPeek();
                }
            }
        }else if(action.equals("com.android.action.FORCE_EXPAND_NOTIFICATION")){
            //mPanelView.onPanelShowing();
            if(mPanelView.mStatusBar!=null){
                if(!mPanelView.isFullyExpanded()){
                    //mPanelView.mStatusBar.animateExpandNotificationsPanel();
                    mPanelView.doPeek();
                }
            }
        }else if(action.equals("com.android.action.FORCE_COLLAPSE_NOTIFICATION")){
            if(mPanelView.mStatusBar!=null){
                mPanelView.mStatusBar.animateCollapsePanels();
            }
        }else
        */
        if(action.equals(ConnectivityManager.CONNECTIVITY_ACTION)){
            NetworkInfo networkInfo = intent.getParcelableExtra(ConnectivityManager.EXTRA_NETWORK_INFO);
            if (networkInfo.getType() == ConnectivityManager.TYPE_ETHERNET){
                Log.d("RTK", "ETH0 state:"+networkInfo.getState());
                if (networkInfo.getState() == NetworkInfo.State.DISCONNECTED) {
                    if(mEthAvailable){
                        //Toast.makeText(context, eth0Str ,Toast.LENGTH_SHORT).show();
                        mHandler.postDelayed(mToastRunnable, 800);
                        Log.d("RTK", "post toast runnable");
                    }
                    mEthAvailable=false;
                }else if(networkInfo.getState() == NetworkInfo.State.CONNECTED){
                    mHandler.removeCallbacks(mToastRunnable);
                    Log.d("RTK","remove toast runnable");
                    mEthAvailable=true;
                }
            }
        }
    }
}
