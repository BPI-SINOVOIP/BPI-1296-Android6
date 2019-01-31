package com.rtk.debug;

import android.app.*;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import com.rtk.debug.helper.RuntimeHelper;
import android.support.v4.app.NotificationCompat;

import java.io.IOException;

/**
 * Created by archerho on 2017/2/8.
 */
public class LogcatService extends Service {
    public static final String EXTRA_TASK = "EXTRA_TASK";
    private static final String TAG = "LogcatService";
    int SERVICE_ID = 100;
    CharSequence mContentTitle = "RtkDebugTool";
    private NotificationManager mNM;

    @Override
    public void onCreate() {
        super.onCreate();
        mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
        init();
    }

    void init(){};

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        handleIntent(intent);
//        mContentTitle = getText(R.string.app_name);

        Intent playIntent = new Intent(this, RtkDebugActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
                playIntent, 0);
        // Set the icon, scrolling text and timestamp
        NotificationCompat.Builder builder =
                new NotificationCompat.Builder(this)
                        .setSmallIcon(R.drawable.ic_launcher)
                        .setContentIntent(pendingIntent)
                        .setContentTitle(mContentTitle);

        Notification notification = builder.build();
        log("startForeground "+mContentTitle);
        startForeground(SERVICE_ID, notification);
        return START_REDELIVER_INTENT;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        log("notification cancel: "+ SERVICE_ID);
        mNM.cancel(SERVICE_ID);
        log("destroy "+mContentTitle);
        try {
            Thread.sleep(300);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    protected void log(String s){
        Log.d(TAG, s);
    }

    void handleIntent(Intent intent){
        if(intent==null) return;
        String task = intent.getStringExtra(EXTRA_TASK);
        log("handleIntent, task="+task);
        if(DumpLog.TAG.equals(task)) {
            try {
                DumpLog.getInstance(this).startDaemon();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else if(DumpKernelLog.TAG.equals(task)){
            try {
                DumpKernelLog.getInstance(this).startDaemon();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else if(DumpTcpdump.TAG.equals(task)){
            try {
                DumpTcpdump.getInstance(this).startDaemon();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else if(task.startsWith("Stop")){
            String stopTask = task.substring(4);
            if(stopTask.equals(DumpLog.TAG))
                DumpLog.getInstance(this).stopDaemon();
            else if(stopTask.equals(DumpKernelLog.TAG))
                DumpKernelLog.getInstance(this).stopDaemon();
            else if(stopTask.equals(DumpTcpdump.TAG))
                DumpTcpdump.getInstance(this).stopDaemon();

            if(!DumpLog.getInstance(this).isOn()
                    && !DumpKernelLog.getInstance(this).isOn()
                    && !DumpTcpdump.getInstance(this).isOn())
                stopSelf();
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
