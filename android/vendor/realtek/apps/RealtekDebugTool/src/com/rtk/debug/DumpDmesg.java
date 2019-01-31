package com.rtk.debug;

import android.content.Context;
import android.os.SystemProperties;
import com.rtk.debug.util.UtilLogger;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by archerho on 2017/2/10.
 */
public class DumpDmesg implements IDump {
    UtilLogger log = new UtilLogger(DumpDmesg.class);
    private static DumpDmesg mIns;

    public static IDump getInstance(Context ctx) {
        if(mIns==null)
            mIns = new DumpDmesg();
        return mIns;
    }

    private static String getLogPath(){
        Date date = new Date();
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String filename = dateFormat.format(date)+".log";
        String logFolder = SystemProperties.get(SettingsFrag.KEY_DUMP_PATH)+ "DumpDmesg";
        File f = new File(logFolder);
        if(!f.exists()) f.mkdirs();

        String logFilePath = logFolder+"/"+filename;
        return logFilePath;
    }

    @Override
    public void on() {
        final String logPath = getLogPath();
        new Thread(new Runnable() {
            @Override
            public void run() {
                String [] commands = { "sh", "-c", "dmesg > "+logPath };
                try {
                    Runtime.getRuntime().exec(commands);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public void off() {

    }

    @Override
    public boolean isOn() {
        return true;
    }
}
