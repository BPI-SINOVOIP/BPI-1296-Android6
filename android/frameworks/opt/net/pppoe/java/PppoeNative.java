package com.android.server.pppoe;

public class PppoeNative {

    private static boolean DBG = false;
    private static final String TAG = "PppoeNative";

    // Hold this lock before calling supplicant - it is required to
    // mutually exclude access from Wifi and P2p state machines
    static final Object mLock = new Object();

    /* Register native functions */

    static {
        /* Native functions are defined in libpppoe-service.so */
        System.loadLibrary("pppoe-native");
        registerNatives();
    }

    /* JNI usage */
    private static native int registerNatives();
    //public native static int setupPppoeNative(String iface, String user, String password, String dns1, String dns2);
    native static int setup(String iface, String user, String password);
    native static int start();
    native static int stop();
    native static int isUp();
}