package com.rtk.net.openwrt;

import android.content.Context;
import android.util.Log;
//import com.jayway.jsonpath.Configuration;
import com.jayway.jsonpath.JsonPath;
import com.jayway.jsonpath.PathNotFoundException;
//import com.jayway.jsonpath.spi.json.JsonProvider;
import net.minidev.json.JSONArray;

public class UbusUtil {
    private static final String TAG = "OpenWrtUtil";
    private static final boolean DEBUG = false;

    private static UbusUtil sUbusUtil;
    private static UbusRpc sRpc = null;
    private static Context mContext;

    //private static JsonProvider sJsonPath = Configuration.defaultConfiguration().jsonProvider();

    private UbusUtil() {}

    public static UbusUtil getInstance(Context context) {
        mContext = context;

        sRpc = UbusRpc.getUbusSession(mContext);

        if(sRpc == null) return null;

        if (sUbusUtil == null) {
            sUbusUtil = new UbusUtil();
        }

        return sUbusUtil;
    }

    private static void log(String s) {
        if (DEBUG)
            Log.d(TAG, s);
    }

    //test if file exists
    public static boolean fileStat(String path) {
        boolean ret = false;

        if (path == null || "".equals(path)) {
            Log.e(TAG, "No path!!");
            return false;
        }

        String json = "\"file\", \"stat\" , {\"path\": \""+ path +"\"}";

        try {
            String jsonResp = sRpc.post(json);
            log("jsonCmd = " + json);
            log("jsonResponse = " + jsonResp);

            if (sRpc.hasUbusError(jsonResp)) return false;

            if (jsonResp.contains("Not found")) {
                log(path + ": Not found!");
            } else {
                ret = true;
            }
        }
        catch(Exception e) {e.printStackTrace();}

        return ret;
    }

    public static boolean format(String targetDisk, String fsType) {
        boolean ret = false;

        if (targetDisk == null || "".equals(targetDisk) ||  fsType == null || "".equals(fsType)) {
            Log.e(TAG, "targetDisk or fsType is null !");
            return false;
        }

        String json = "\"disk-helper\", \"allinone\" , {\"devname\": \""+targetDisk+"\", \"fstype\": \""+fsType+"\"}";

        try {
            String jsonResp = sRpc.post(json);
            log("jsonCmd = " + json);
            log("jsonResponse = " + jsonResp);

            if (sRpc.hasUbusError(jsonResp)) return false;

            if (jsonResp.contains("failed")) {
                Log.e(TAG, "OpenWrt format failed: " + jsonResp);
            } else {
                ret = true;
            }
        }
        catch(Exception e) {e.printStackTrace();}

        return ret;
    }

    public String getServerIp() {
        return sRpc.getRouterHost(mContext);
    }
}
