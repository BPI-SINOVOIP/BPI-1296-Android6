package com.android.server.pppoe;

import android.util.Log;

import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.List;

public class Utils {

    private static boolean DBG = true;
    private static final String TAG = "Utils";

    public static NetworkInterface getIface(String regex){
        Enumeration<NetworkInterface> ifaces = null;
        try {
            ifaces = NetworkInterface.getNetworkInterfaces();
            while (ifaces.hasMoreElements()) {
                NetworkInterface iface = ifaces.nextElement();
                if (!iface.getDisplayName().matches(regex))
                    continue;
                return iface;
            }
        } catch (SocketException e) {
            e.printStackTrace();
        }
        return null;
    }

    //Used for ppp interface
    public static InterfaceAddress getIfaceIp(String regex) {
        NetworkInterface iface = getIface(regex);
        if(iface==null)
            return null;
        List<InterfaceAddress> addressList = iface.getInterfaceAddresses();
        return addressList.get(addressList.size()-1);
    }

    private static void log(String s){
        if(DBG)
            Log.d(TAG, s);
    }
}

