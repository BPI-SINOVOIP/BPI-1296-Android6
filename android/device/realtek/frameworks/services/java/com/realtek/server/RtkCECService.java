/*
 * Copyright (c) 2013 Realtek Semi. co. All Rights Reserved.
 * Author Wen
 * Version V0.1
 * Date:  2013-12-30
 * Comment:This class lets you access the CEC function.
 * RtkCECService will be a process as a android service, you should use RtkCECManager to access it. 
 */
package com.realtek.server;

import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Locale;

import android.util.Slog;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.IPowerManager;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.os.SystemProperties;
import com.realtek.hardware.IRtkCECService;
import com.realtek.server.RtkTVSystem;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.content.ContentResolver;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import java.lang.reflect.Method;
import java.lang.reflect.Field;
import android.content.res.Configuration;
import android.content.res.Resources;


public class RtkCECService extends IRtkCECService.Stub {
	static {
                System.loadLibrary("realtek_runtime");
	}

	private static final String TAG = "RtkCECService";
	private Context m_Context;
	private IntentFilter m_CECFilter = null;
	private boolean m_CECReceiverRegistered = false;
    private boolean m_GetTVStandbyCMD = false;
    private ArrayList<Locale> mAvailableLanguage = null;
    private boolean m_hdmi_mode = false;

    private static final int CEC_MENU_LANG_ENG = 0x656e67 ;
    private static final int CEC_MENU_LANG_CHI = 0x636869 ;
    private static final int CEC_MENU_LANG_JPN = 0x6a706e ;
    private static final int CEC_MENU_LANG_SPA = 0x737061 ;
    private static final int CEC_MENU_LANG_FRA = 0x667261 ;
    private static final int CEC_MENU_LANG_FRE = 0x667265 ;
    private static final int CEC_MENU_LANG_DEU = 0x646575 ;
    private static final int CEC_MENU_LANG_GER = 0x676572 ;
    private static final int CEC_MENU_LANG_ITA = 0x697461 ;
    private static final int CEC_MENU_LANG_KOR = 0x6b6f72 ;
    private static final int CEC_MENU_LANG_DUT = 0x647574 ;
    private static final int CEC_MENU_LANG_NLD = 0x6e6c64 ;
    private static final int CEC_MENU_LANG_RUS = 0x727573 ;
    private static final int CEC_MENU_LANG_ZHO = 0x7a686f ;
    private static final int CEC_MENU_LANG_POR = 0x706f72 ;
    private static final int CEC_MENU_LANG_SWE = 0x737765 ;
    private static final int CEC_MENU_LANG_DAN = 0x64616e ;
    private static final int CEC_MENU_LANG_NOR = 0x6e6f72 ;
    private static final int CEC_MENU_LANG_NOB = 0x6e6f62 ;
    private static final int CEC_MENU_LANG_NNO = 0x6e6e6f ;
    private static final int CEC_MENU_LANG_FIN = 0x66696e ;
    private static final int CEC_MENU_LANG_POL = 0x706f6c ;
    private static final int CEC_MENU_LANG_TUR = 0x747572 ;
    private static final int CEC_MENU_LANG_SLK = 0x736c6b ;
    private static final int CEC_MENU_LANG_SLO = 0x736c6f ;
    private static final int CEC_MENU_LANG_SLV = 0x736c76 ;
    private static final int CEC_MENU_LANG_SCR = 0x736372 ;
    private static final int CEC_MENU_LANG_HRV = 0x687276 ;
    private static final int CEC_MENU_LANG_CZE = 0x637a65 ;
    private static final int CEC_MENU_LANG_CES = 0x636573 ;
    private static final int CEC_MENU_LANG_HUN = 0x68756e ;
    private static final int CEC_MENU_LANG_RUM = 0x72756d ;
    private static final int CEC_MENU_LANG_RON = 0x726f6e ;
    private static final int CEC_MENU_LANG_MAY = 0x6d6179 ;
    private static final int CEC_MENU_LANG_MSA = 0x6d7361 ;
    private static final int CEC_MENU_LANG_GRE = 0x677265 ;
    private static final int CEC_MENU_LANG_ELL = 0x656c6c ;
    private static final int CEC_MENU_LANG_ESP = 0x657370 ;
    private static final int CEC_MENU_LANG_SUN = 0x73756e ;
    private static final int CEC_MENU_LANG_SVE = 0x737665 ;
    private static final int CEC_MENU_LANG_HIN = 0x68696e ;
    private static final int CEC_MENU_LANG_THA = 0x746861 ;
    private static final int CEC_MENU_LANG_UKR = 0x756b72 ;

    //CEC_USR_CTRL_xxx are copy from CEC_Def.h 
    public static final int CEC_USR_CTRL_SELECT                     = 0x00;
    public static final int CEC_USR_CTRL_UP                         = 0x01;
    public static final int CEC_USR_CTRL_DOWN                       = 0x02;
    public static final int CEC_USR_CTRL_LEFT                       = 0x03;
    public static final int CEC_USR_CTRL_RIGHT                      = 0x04;
    public static final int CEC_USR_CTRL_RIGHT_UP                   = 0x05;
    public static final int CEC_USR_CTRL_RIGHT_DOWN                 = 0x06;    
    public static final int CEC_USR_CTRL_LEFT_UP                    = 0x07;
    public static final int CEC_USR_CTRL_LEFT_DOWN                  = 0x08;
    public static final int CEC_USR_CTRL_ROOT_MENU                  = 0x09;
    public static final int CEC_USR_CTRL_SETUP_MENU                 = 0x0A;
    public static final int CEC_USR_CTRL_CONTENTS_MENU              = 0x0B;
    public static final int CEC_USR_CTRL_FAVORITE_MENU              = 0x0C;
    public static final int CEC_USR_CTRL_EXIT                       = 0x0D;
    public static final int CEC_USR_CTRL_NUMBER_0                   = 0x20;
    public static final int CEC_USR_CTRL_NUMBER_1                   = 0x21;
    public static final int CEC_USR_CTRL_NUMBER_2                   = 0x22;
    public static final int CEC_USR_CTRL_NUMBER_3                   = 0x23;
    public static final int CEC_USR_CTRL_NUMBER_4                   = 0x24;
    public static final int CEC_USR_CTRL_NUMBER_5                   = 0x25;
    public static final int CEC_USR_CTRL_NUMBER_6                   = 0x26;
    public static final int CEC_USR_CTRL_NUMBER_7                   = 0x27;
    public static final int CEC_USR_CTRL_NUMBER_8                   = 0x28;
    public static final int CEC_USR_CTRL_NUMBER_9                   = 0x29;
    public static final int CEC_USR_CTRL_DOT                        = 0x2A;
    public static final int CEC_USR_CTRL_ENTER                      = 0x2B;
    public static final int CEC_USR_CTRL_CLEAR                      = 0x2C;    
    public static final int CEC_USR_CTRL_NEXT_FAVORITE              = 0x2F; 
    public static final int CEC_USR_CTRL_CHANNEL_UP                 = 0x30;
    public static final int CEC_USR_CTRL_CHANNEL_DOWN               = 0x31;
    public static final int CEC_USR_CTRL_PREVIOUS_CHANNEL           = 0x32;
    public static final int CEC_USR_CTRL_SOUND_SELECT               = 0x33;
    public static final int CEC_USR_CTRL_INPUT_SELECT               = 0x34;
    public static final int CEC_USR_CTRL_DISPLAY_INFORMATION        = 0x35;
    public static final int CEC_USR_CTRL_HELP                       = 0x36;
    public static final int CEC_USR_CTRL_PAGE_UP                    = 0x37;
    public static final int CEC_USR_CTRL_PAGE_DOWN                  = 0x38;
    public static final int CEC_USR_CTRL_POWER                      = 0x40;
    public static final int CEC_USR_CTRL_VOLUME_UP                  = 0x41;
    public static final int CEC_USR_CTRL_VOLUME_DOWN                = 0x42;
    public static final int CEC_USR_CTRL_MUTE                       = 0x43;
    public static final int CEC_USR_CTRL_PLAY                       = 0x44;
    public static final int CEC_USR_CTRL_STOP                       = 0x45;
    public static final int CEC_USR_CTRL_PAUSE                      = 0x46;
    public static final int CEC_USR_CTRL_RECORD                     = 0x47;
    public static final int CEC_USR_CTRL_REWIND                     = 0x48;
    public static final int CEC_USR_CTRL_FAST_FORWARD               = 0x49;
    public static final int CEC_USR_CTRL_EJECT                      = 0x4A;
    public static final int CEC_USR_CTRL_FORWARD                    = 0x4B;
    public static final int CEC_USR_CTRL_BACKWARD                   = 0x4C;
    public static final int CEC_USR_CTRL_STOP_RECORD                = 0x4D;
    public static final int CEC_USR_CTRL_PAUSE_RECORD               = 0x4E;
    public static final int CEC_USR_CTRL_ANGLE                      = 0x50;
    public static final int CEC_USR_CTRL_SUB_PICTURE                = 0x51;
    public static final int CEC_USR_CTRL_VIDEO_ON_DEMAND            = 0x52;
    public static final int CEC_USR_CTRL_GUIDE                      = 0x53;
    public static final int CEC_USR_CTRL_TIMER                      = 0x54;
    public static final int CEC_USR_CTRL_INIT_CONFIGURATION         = 0x55;
    public static final int CEC_USR_CTRL_PLAY_FUNCTION              = 0x60; 
    public static final int CEC_USR_CTRL_PAUSE_PLAY_FUNCTION        = 0x61;
    public static final int CEC_USR_CTRL_RECORD_FUNCTION            = 0x62;
    public static final int CEC_USR_CTRL_PAUSE_RECORD_FUNCTION      = 0x63;
    public static final int CEC_USR_CTRL_STOP_FUNCTION              = 0x64;
    public static final int CEC_USR_CTRL_MUTE_FUNCTION              = 0x65;
    public static final int CEC_USR_CTRL_RESTORE_VOLUME_FUNCTION    = 0x66;
    public static final int CEC_USR_CTRL_TUNE_FUNCTION              = 0x67;
    public static final int CEC_USR_CTRL_SELECT_MEDIA_FUNCTION      = 0x68;
    public static final int CEC_USR_CTRL_SELECT_AV_INPUT_FUNCTION   = 0x69;
    public static final int CEC_USR_CTRL_SELECT_AUDIO_INPUT_FUNCTION= 0x6A;
    public static final int CEC_USR_CTRL_POWER_TOGGLE               = 0x6B;
    public static final int CEC_USR_CTRL_POWER_OFF                  = 0x6C;
    public static final int CEC_USR_CTRL_POWER_ON                   = 0x6D;
    public static final int CEC_USR_CTRL_F1                         = 0x71;  // BLUE
    public static final int CEC_USR_CTRL_F2                         = 0x72;  // RED
    public static final int CEC_USR_CTRL_F3                         = 0x73;  // GREEN
    public static final int CEC_USR_CTRL_F4                         = 0x74;  // YELLOW
    public static final int CEC_USR_CTRL_F5                         = 0x75;    
    public static final int CEC_USR_CTRL_DATA                       = 0x76;    
    public static final int CEC_USR_CTRL_UNKNOWN                    = 0xFF;    

    private static final int HDMI_PLUG_IN =1;
    private static final int HDMI_PLUG_OUT =0;
	private final Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case HDMI_PLUG_IN:
                JNI_HDMIHotPlug(true);
                break;
            case HDMI_PLUG_OUT:
                JNI_HDMIHotPlug(false);
                break;
            default:
                break;
            }
        }
    };
		
	public RtkCECService(Context context)
	{
		m_Context = context;
		Slog.w(TAG,"RtkCECService is constructed!");
		if (new File("/sys/devices/virtual/switch/hdmi/state").exists()) {
			// HDMI Mode
		    InitCEC();
            m_hdmi_mode = true;
        }else{
            m_hdmi_mode = false;
        }
	}
	//Wen_Issue: When system will kill me (this service)?	
	private void InitCEC()
	{
		//Wen: We should do someting while HDMI is plug IN/OUT
		//Wen Issue: How to got HDMI Rx signal?
		//m_CECFilter = new IntentFilter("android.intent.action.HDMI_PLUGGED");//It's HDMI Tx
		m_CECFilter = new IntentFilter();
        m_CECFilter.addAction("android.intent.action.HDMI_PLUGGED");//It's HDMI Tx
        m_CECFilter.addAction(Intent.ACTION_SHUTDOWN);//It's System Shutdown
		hookCECPlugStatus(true);
		JNI_Init();
	}
	public final static String EXTRA_HDMI_PLUGGED_STATE = "state";

	private BroadcastReceiver CECHotPlugReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals("android.intent.action.HDMI_PLUGGED"))
            {
                HDMIPlugHandle(context, intent);
            }
            else if (intent.getAction().equals(Intent.ACTION_SHUTDOWN))
            {
                SystemShutdownHandle(context, intent);
            }
            
		}
        private void HDMIPlugHandle(Context context, Intent intent) {
			boolean HDMIPlugged = intent.getBooleanExtra(EXTRA_HDMI_PLUGGED_STATE, false);
			Slog.i(TAG, "Got HDMI plug event ["+HDMIPlugged+"]");
			// call the function handling hot plug.
            if (HDMIPlugged)//HDMI Plug IN
            {
                handler.removeCallbacksAndMessages(null);
                handler.sendEmptyMessageDelayed(HDMI_PLUG_IN,1500);//Delay 1500ms, some TV will send hot plug in twice 
            }
            else//HDMI Plug OUT
            {
                handler.removeCallbacksAndMessages(null);
                handler.sendEmptyMessage(HDMI_PLUG_OUT); 
            }
        }
        private void SystemShutdownHandle(Context context, Intent intent) {
            //Wen: Should Add info for asking all CEC device go to standby or not. 
            boolean UserSpaceOnly = intent.getBooleanExtra(Intent.EXTRA_SHUTDOWN_USERSPACE_ONLY,false);
            boolean Reboot = intent.getBooleanExtra(Intent.EXTRA_SHUTDOWN_FOR_REBOOT,false);
            Slog.i(TAG, "Got System Shutdown UserSpaceOnly["+UserSpaceOnly+"] Reboot["+Reboot+"]");
            if ((!UserSpaceOnly) && (!Reboot))
            {
                if (m_GetTVStandbyCMD){
                    JNI_SystemStandby(false);// This standby request is comed from TV, so we don't need to Ask CEC Standby again. 
                    m_GetTVStandbyCMD = false;
                }
                else{
                    JNI_SystemStandby(true);//Ask All CEC device go to standby
                }
            }
        }
	};

	private void hookCECPlugStatus(boolean bEnable) {
		if(bEnable) {
			if(!m_CECReceiverRegistered) {
				m_Context.registerReceiver(CECHotPlugReceiver, m_CECFilter);
				m_CECReceiverRegistered = true;
				Slog.w(TAG, "hookHDMI_TX_PlugStatus register");
			}
		}
		else {
			if(m_CECReceiverRegistered) {
				m_Context.unregisterReceiver(CECHotPlugReceiver);
				m_CECReceiverRegistered = false;
				Slog.w(TAG, "hookHDMI_TX_PlugStatus unregister");
			}
		}
	}
    //Load all available language
    private void doLanguageItemPreparation() {
        String[] str = Resources.getSystem().getAssets().getLocales();  // get languages that we support (Display in the Settings), zh_CN/en_US/zh_TW/en.
        Locale[] l = Locale.getAvailableLocales();  // get all languages Android support

        for (Locale ll : l) {
            //Slog.d(TAG, "locale = " + ll.toString());
            for (String s : str) {
                // Locale, Android 4.2.2 => zh_CN, zh_TW, en_US
                // Locale, Android 4.4 => zh_HANS_CN, zh_HANT_TW, en_US
                // Since the text is changed from 4.4, we should do manual comparison.
                String[] langCode = ll.toString().split("_");
                String newLangCode = "en";
                if(langCode.length == 1) {  // uk
                    newLangCode = langCode[0];
                } else if(langCode.length == 2) {  // en_US
                    newLangCode = langCode[0] + "_" + langCode[1];
                } else if(langCode.length == 3) {  // zh_HANT_TW
                    newLangCode = langCode[0] + "_" + langCode[2];
                }
                //Log.d(TAG, "newLangCode = " + newLangCode);

                //if(ll.toString().equals(s) && s.length() > 2) {  // only working on Android 4.2.2
                if(newLangCode.equals(s) && s.length() > 2) { // working on Android 4.4
                    if(langCode.length == 3){
                        if (ll.toString().equals("zh_HANS_CN")){
                            mAvailableLanguage.add(Locale.CHINA);
                        }else if (ll.toString().equals("zh_HANT_TW")){
                            mAvailableLanguage.add(Locale.TAIWAN);
                        }else{
                            mAvailableLanguage.add(ll);
                        }
                    }
                    else{
                        mAvailableLanguage.add(ll);
                    }
                    Slog.d(TAG, "locale cmp, " + ll.toString() + " = " + s + ", DisplayName = " + ll.getDisplayName() + " v = " + ll.getDisplayVariant());
                }
            }
        }
        Slog.d(TAG, "mAvailableLanguage count =  " + mAvailableLanguage.size());
    }
    private boolean IsMatched(Locale TestLocale)
    {
        boolean rlt = false;
        for (Locale ll : mAvailableLanguage)
        {
            if (ll.equals(TestLocale))
            {
                rlt = true;
                Slog.d(TAG, "Locale is match :" + TestLocale.toString() + " DisplayName = " + TestLocale.getDisplayName() + " v = " + TestLocale.getDisplayVariant());
            }
        }
        return rlt;
    }
    private void doLanguageSetting(Locale locale )
    {
        try {
            Configuration conf = null;
            Object amnObj = null;

            // This is the way what LocalePicker did from com.android.internal.app package.
            Class activitymanagernClass = Class.forName("android.app.ActivityManagerNative");
            Method methodGetConfiguration = activitymanagernClass.getMethod("getConfiguration");
            methodGetConfiguration.setAccessible(true);

            Method methodGetDefault = activitymanagernClass.getMethod("getDefault");
            methodGetDefault.setAccessible(true);
            amnObj = methodGetDefault.invoke(activitymanagernClass);

            conf = (Configuration) methodGetConfiguration.invoke(amnObj);

            Class configClass = conf.getClass();
            Field field = configClass.getField("userSetLocale");
            field.setBoolean(conf, true);

            conf.locale = locale;  //locale;

            Method methodUpdateConfiguration = activitymanagernClass.getMethod("updateConfiguration", Configuration.class);
            methodUpdateConfiguration.setAccessible(true);
            methodUpdateConfiguration.invoke(amnObj, conf);
        } catch (Exception e) {

        }
    }
/*followed API are use for Application level*/
	public boolean doOneTouchPlay(int Key)
	{
		Slog.i(TAG, "doOneTouchPlay Key:"+Key);
        if (m_hdmi_mode){
		    return JNI_doOneTouchPlay(Key);
        }else{
            return false;
        }
		
	}

/*followed API are use for Application level*/
	public int SendRemotePressed(int Dev, int Key, boolean SendReleased)
	{
		Slog.i(TAG, "SendRemotePressed Dev:"+Dev+" Key:"+Key);
        if (m_hdmi_mode){
		    return JNI_SendRemotePressed(Dev, Key, SendReleased);//return for JNI: 0:OK, -1:Send Fail, -2:Can not find Dev.
        }else{
            return -3;
        }
		
	}

/*followed API are use for JNI call back*/
	public void SetMenuLanguage(int Code)
	{
 		Slog.i(TAG, "SetMenuLanguage Code:"+Code);
        Locale AskLocale;
        AskLocale = Locale.TRADITIONAL_CHINESE;
        if (mAvailableLanguage == null)
        {
            mAvailableLanguage = new ArrayList<Locale>();
            doLanguageItemPreparation();
        }
        switch(Code)
        {
            case CEC_MENU_LANG_ENG:
                 AskLocale = Locale.US;
            break;
            case CEC_MENU_LANG_CHI:
                 AskLocale = Locale.TAIWAN;
            break;
            case CEC_MENU_LANG_JPN:
                 AskLocale = Locale.JAPANESE;
            break;
            case CEC_MENU_LANG_FRA:
                 AskLocale = Locale.FRENCH;
            break;
            case CEC_MENU_LANG_FRE:
                 AskLocale = Locale.FRENCH;
            break;
            case CEC_MENU_LANG_DEU:
                 AskLocale = Locale.GERMAN;
            break;
            case CEC_MENU_LANG_GER:
                 AskLocale = Locale.GERMAN;
            break;
            case CEC_MENU_LANG_ITA:
                 AskLocale = Locale.ITALIAN;
            break;
            case CEC_MENU_LANG_KOR:
                 AskLocale = Locale.KOREA;
            break;
            case CEC_MENU_LANG_ZHO:
                 AskLocale = Locale.SIMPLIFIED_CHINESE;
            break;
            case CEC_MENU_LANG_SPA:
            case CEC_MENU_LANG_DUT:
            case CEC_MENU_LANG_NLD:
            case CEC_MENU_LANG_RUS:
            case CEC_MENU_LANG_POR:
            case CEC_MENU_LANG_SWE:
            case CEC_MENU_LANG_DAN:
            case CEC_MENU_LANG_NOR:
            case CEC_MENU_LANG_NOB:
            case CEC_MENU_LANG_NNO:
            case CEC_MENU_LANG_FIN:
            case CEC_MENU_LANG_POL:
            case CEC_MENU_LANG_TUR:
            case CEC_MENU_LANG_SLK:
            case CEC_MENU_LANG_SLO:
            case CEC_MENU_LANG_SLV:
            case CEC_MENU_LANG_SCR:
            case CEC_MENU_LANG_HRV:
            case CEC_MENU_LANG_CZE:
            case CEC_MENU_LANG_CES:
            case CEC_MENU_LANG_HUN:
            case CEC_MENU_LANG_RUM:
            case CEC_MENU_LANG_RON:
            case CEC_MENU_LANG_MAY:
            case CEC_MENU_LANG_MSA:
            case CEC_MENU_LANG_GRE:
            case CEC_MENU_LANG_ELL:
            case CEC_MENU_LANG_ESP:
            case CEC_MENU_LANG_SUN:
            case CEC_MENU_LANG_SVE:
            case CEC_MENU_LANG_HIN:
            case CEC_MENU_LANG_THA:
            case CEC_MENU_LANG_UKR:
                 AskLocale = Locale.ENGLISH;
            break;
            default:
                 AskLocale = Locale.ENGLISH;
            break;
        }
        if (IsMatched(AskLocale))
        {
            doLanguageSetting(AskLocale);
        }

	}
    public void GetSystemStandby()
    {
 		Slog.i(TAG, "GetSystemStandby");
        m_GetTVStandbyCMD = true;
        IBinder b = ServiceManager.getService(Context.POWER_SERVICE);
        IPowerManager pm = IPowerManager.Stub.asInterface(b);
        try{
        pm.shutdown(false,false);
        }catch(RemoteException e){
        }
    }
    
/* JNI API */	
	private native boolean JNI_Init();
	private native boolean JNI_doOneTouchPlay(int key);
	private native int JNI_SendRemotePressed(int DevAddr, int Key, boolean SendReleased);
    private native void JNI_HDMIHotPlug(boolean HotPlugIn);
    private native void JNI_SystemStandby(boolean AllStandby);

}//End of RtkCECService
