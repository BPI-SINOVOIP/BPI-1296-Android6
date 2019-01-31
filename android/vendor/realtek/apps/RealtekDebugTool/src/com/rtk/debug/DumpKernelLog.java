package com.rtk.debug;

import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.util.Log;
import com.rtk.debug.helper.*;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;

/**
 * Created by archerho on 2015/10/7.
 */
public class DumpKernelLog implements IDump{
    public static final String TAG = "DumpKernelLog";
    private static DumpKernelLog mIns;
    private final Context context;
    private static Process sProc;

    public static DumpKernelLog getInstance(Context ctx){
        if(mIns==null){
            mIns = new DumpKernelLog(ctx);
        }
        return mIns;
    }

    private DumpKernelLog(Context context) {
        this.context = context;
    }

    public static String getLogPath(){
        Date date = new Date();
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String filename = dateFormat.format(date)+".log";
        String logFolder = SystemProperties.get(SettingsFrag.KEY_DUMP_PATH)+ "DumpKernelLog";
        File f = new File(logFolder);
        if(!f.exists()) f.mkdirs();

        String logFilePath = logFolder+"/"+filename;
        return logFilePath;
    }

    Process startDaemon() throws IOException {
        if(sProc==null)
            sProc = LogcatHelper.getLogcatToFileProcess("kernel", getLogPath());
        return sProc;
    }

    void stopDaemon(){
        if(sProc!=null) {
            RuntimeHelper.destroy(sProc);
            sProc = null;
        }
    }

    public void on(){
        try {
            RuntimeHelper.exec(Arrays.asList("logcat", "-c"));
            Thread.sleep(100);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        log("enable, logcat is cleaned");
        Intent intent = new Intent(context, LogcatService.class);
        intent.putExtra(LogcatService.EXTRA_TASK, TAG);
        context.startService(intent);
    }

    public void off(){
        log("disable");
        Intent intent = new Intent(context, LogcatService.class);
        intent.putExtra(LogcatService.EXTRA_TASK, "Stop"+TAG);
        context.startService(intent);
    }

    public boolean isOn() {
        return sProc!=null;
    }

    private void log(String s) {
        Log.d(TAG, s);
    }
}
