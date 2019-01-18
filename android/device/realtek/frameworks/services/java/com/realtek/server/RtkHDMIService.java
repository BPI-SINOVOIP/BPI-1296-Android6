package com.realtek.server;

import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import android.util.Slog;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.os.Looper;
import android.os.SystemProperties;
import java.lang.ref.WeakReference;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.UEventObserver;
import com.realtek.hardware.IRtkHDMIService;
import com.realtek.server.RtkTVSystem;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.content.ContentResolver;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.widget.Toast;
import com.realtek.hardware.RtkVoutUtilManager;


/*
* The implementation of the RtkHDMIService.
*/
public class RtkHDMIService extends IRtkHDMIService.Stub {
	static {
		System.loadLibrary("realtek_runtime");
	}

	private static final String TAG = "RtkHDMIService";

	/**
	 * Binder context for this service
	 */
 	private Context mContext;
	private IntentFilter hdmiFilter = null;
	private IntentFilter bootFilter = null;
	private boolean hdmiReceiverRegistered = false;
	private boolean bootCompleteRegistered = false;
	private int mode = 0;
	private boolean hdcpEnabledByConfig = false;
	private boolean hdcpKernal = false;
	private boolean bootComplete = false;
    private boolean hotplugOnScreenOn = false;

	private static class MyHandler extends Handler {
		private final WeakReference<RtkHDMIService> mService;
	    public MyHandler(RtkHDMIService service) {
	      mService = new WeakReference<RtkHDMIService>(service);
	    }
	
	   @Override
	    public void handleMessage(Message msg) {
	      RtkHDMIService service = mService.get();
	      if (service != null) {		  	
		  	if (msg.what == 0)
		  	{
				Slog.w(TAG, "HDCP Key doesn't exist!\n");
				CharSequence text = "HDCP Key doesn't exist!";
				int duration = Toast.LENGTH_LONG;
				
				Toast toast = Toast.makeText(service.mContext, text, duration);
				toast.show();
		  	}	
		  	else if (msg.what == 1)
		  	{
				CharSequence text = "invalid HDMI key found, Turn off HDMI!";
				int duration = Toast.LENGTH_LONG;
				
				Toast toast = Toast.makeText(service.mContext, text, duration);
				toast.show();
		  	}	
	        else if (msg.what == 2)
	        {
				service.turnOffHdmi();
	        }
	        else if (msg.what == 3)
	        {
				Slog.w(TAG, "Kernal doesn't enabled HDCP!\n");
				CharSequence text = "Kernal doesn't enabled HDCP!";
				int duration = Toast.LENGTH_LONG;
				
				Toast toast = Toast.makeText(service.mContext, text, duration);
				toast.show();
	        }
	      }
	    }
	  }

	private final MyHandler mHandler = new MyHandler(this);
	/**
	 * Constructs a new RtkHDMIService instance.
	 *
	 */
	public RtkHDMIService(Context context) {
		mContext = context;
		Slog.w(TAG, "RtkHDMIService is constructed!");

		// check if HDMI or MHL device is activated by drive.
		// decide to initialize HDMI controller and MHL controller.
		// and set the service to the different mode.
		if (new File("/sys/devices/virtual/switch/hdmi/state").exists()) {
			// HDMI Mode
			Slog.w(TAG, "initialize media link to HDMI Mode");
			mode = RtkTVSystem.HDMI_MODE;
		} else {
			Slog.w(TAG, "initialize media link to MHL Mode");
			mode = RtkTVSystem.MHL_MODE;
		}

		initMediaLinkController(mode);

		// Initialize HDCP
		if("true".equals(SystemProperties.get("ro.config.enablehdcp"))) {
			Slog.w(TAG, "HDCP is initialized by the config \"ro.config.enablehdcp\"");
			hdcpEnabledByConfig = true;
			_setHdcpEnabledByConfig(true);
			initializeHDCPState();
		} else {
			Slog.w(TAG, "*** HDCP is NOT initialized by the config \"ro.config.enablehdcp\"");
			hdcpEnabledByConfig = false;
			_setHdcpEnabledByConfig(false);
		}

		//sync the setting
		int TVSystemUser = 0;
		int tv_sys = -1;
		int tv_std = -1;
		// sync with the setting value REALTEK_SETUP_TV_SYSTEM_USER.
		TVSystemUser = getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM_USER);
		Slog.w(TAG, "TV_SYSTEM_USER: " + TVSystemUser + "\n");
		if(TVSystemUser == RtkTVSystem.TV_SYS_HDMI_AUTO_DETECT) {
			if(getTVSystemAutoModeFromSettings() != RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID) {
				// update the TV system
				Slog.w(TAG, "Update Auto Mode to RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID\n");
				setTVSystemAutoModeFromSettings(RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID);
			}
		} else {
			if(getTVSystemAutoModeFromSettings() == RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID)
			{
				Slog.w(TAG, "Update Auto Mode to RtkTVSystem.TV_SYSTEM_AUTO_MODE_OFF\n");
				setTVSystemAutoModeFromSettings(RtkTVSystem.TV_SYSTEM_AUTO_MODE_OFF);
			}

			for(int ii = 0; ii < RtkTVSystem.tv_map.length; ii++)
			{
				if(RtkTVSystem.tv_map[ii][0] == TVSystemUser)
				{
					tv_sys = RtkTVSystem.tv_map[ii][1];
					tv_std = RtkTVSystem.tv_map[ii][2];
					if(tv_sys != getVideoSystemFromSettings()
						|| tv_std != getVideoStandardFromSettings()) {
						Slog.w(TAG, "Update SetTVSystemAndStandardToSetup" + tv_sys + ", " +  tv_std + "\n");
						SetTVSystemAndStandardToSetup(tv_sys, tv_std);
					}
					break;
				}
			}

		}

		_updateHDMISetting(getVideoSystemFromSettings(), getVideoStandardFromSettings(), getTVSystemAutoModeFromSettings());

		hdmiFilter = new IntentFilter();
		hdmiFilter.addAction("android.intent.action.HDMI_PLUGGED");
		hdmiFilter.addAction("android.intent.action.SCREEN_OFF");
		hdmiFilter.addAction("android.intent.action.SCREEN_ON");
        hdmiFilter.addAction("rtk.intent.action.HDMI_OFF");
        hdmiFilter.addAction("rtk.intent.action.HDMI_ON");
        hdmiFilter.addAction("rtk.intent.action.DP_OFF");
        hdmiFilter.addAction("rtk.intent.action.DP_ON");
		hookHDMIPlugStatus(true);
		bootFilter = new IntentFilter("android.intent.action.BOOT_COMPLETED");
		hookBootComplete(true);
	}

    private boolean getPlatformScreenOffMode() {
        int mode = SystemProperties.getInt("persist.rtk.screenoff.suspend",0);
        if(mode == 1){
            //Slog.d(TAG, "ScreenOff mode: SCREEN_OFF_SUSPEND_ON");
            return true;
        }else{
            //Slog.d(TAG, "ScreenOff mode: SCREEN_OFF_SUSPEND_OFF");
            return false;
        }
    }

    private int getHDMIState() {
        try{
            BufferedReader br = new BufferedReader(new FileReader("/sys/devices/virtual/switch/hdmi/state"));
            String line = br.readLine().trim();
            return Integer.parseInt(line);
        }catch (FileNotFoundException e){
            e.printStackTrace();
            return -1;
        }catch(NumberFormatException e){
            e.printStackTrace();
            return -1;
        }catch (IOException e){
            e.printStackTrace();
            return -1;
        }
    }

	public final static String EXTRA_HDMI_PLUGGED_STATE = "state";

	private BroadcastReceiver hdmiHotPlugReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			boolean hdmiPlugged;
			boolean bHDCP = true;
			boolean bKernal = true;
			RtkVoutUtilManager mgr = new RtkVoutUtilManager();
			int state = 0;

			if(action.equals("android.intent.action.SCREEN_OFF")){

                if(getPlatformScreenOffMode()){
                    hdmiPlugged = getHDMIState()==1?true:false;
                    if(!hdmiPlugged){
                        Slog.d(TAG, "SCREEN_OFF hotplugOnScreenOn");
                        hotplugOnScreenOn = true;
                    }
                    return;
                }else{
				    Slog.w(TAG, "handle android.intent.action.SCREEN_OFF");
				    hdmiPlugged = false;
                }
			}
			else if(action.equals("android.intent.action.SCREEN_ON")){

                if(getPlatformScreenOffMode()){
                    // real suspend flow, do nothing
                    hdmiPlugged = getHDMIState()==1?true:false;

                    if(!hdmiPlugged) {
                        if(hotplugOnScreenOn) {
                            // no HDMI is because no cable is plugged
                            turnOffHdmi();
                            return;
                        }
                    }else{
                        // let the flow continue...
                        Slog.d(TAG, "SCREEN_ON hdmiPlugged:"+hdmiPlugged);
                    }

                }else{
				    Slog.w(TAG, "handle android.intent.action.SCREEN_ON");
				    hdmiPlugged = true;
                }
			}
			else if(action.equals("android.intent.action.HDMI_PLUGGED")){
				Slog.w(TAG, "handle android.intent.action.HDMI_PLUGGED");
				hdmiPlugged = intent.getBooleanExtra(EXTRA_HDMI_PLUGGED_STATE, false);
			}
            else if(action.equals("rtk.intent.action.HDMI_OFF")){
                Slog.w(TAG, "receive rtk.intent.action.HDMI_OFF");
                _writeHDMIEnabledNandTVSystem(1, false);
                return;
            }
            else if(action.equals("rtk.intent.action.HDMI_ON")){
                Slog.w(TAG, "receive rtk.intent.action.HDMI_ON");
                _writeHDMIEnabledNandTVSystem(1, true);
                return;
            }
            else if(action.equals("rtk.intent.action.DP_OFF")){
                Slog.w(TAG, "receive rtk.intent.action.DP_OFF");
                _writeHDMIEnabledNandTVSystem(0, false);
                return;
            }
            else if(action.equals("rtk.intent.action.DP_ON")){
                Slog.w(TAG, "receive rtk.intent.action.DP_ON");
                _writeHDMIEnabledNandTVSystem(0, true);
                return;
            }
			else
			{
				Slog.w(TAG, "Not support intent " + action + "\n");
				return;
			}

			// call the function handling hot plug.
			if(hdmiPlugged) {
				state = 1;
			}

			//for Testing, HDMI work without cable
			if (Settings.System.getInt(mContext.getContentResolver(),Settings.System.REALTEK_YST_MODE, 0) == 1)
			{
				if(state == 0) {
					return;
				}
			}

			handleHotPlug(state);
			Slog.w(TAG, "handleHotPlug hdcpEnabledByConfig =" + hdcpEnabledByConfig + "\n");
			if (hdcpEnabledByConfig)
			{
				bHDCP = checkIfHDCPKeyExist(); 
				bKernal = hdcpKernal;
			} 
			Slog.w(TAG, "handleHotPlug bHDCP =" + bHDCP + "\n");
			if(state == 1) {
				if(mgr.isZooming()) {
					// reset zoom
					mgr.resetZoom();
				}
				if (bHDCP == false && bootComplete == true)
				{
					mHandler.sendEmptyMessage(0);
					mHandler.sendEmptyMessageDelayed(2,10000);
					/*
					Context myContext = context.getApplicationContext();
					Slog.w(TAG, "HDCP Key doesn't exist!\n");
					CharSequence text = "HDCP Key doesn't exist!";
					int duration = Toast.LENGTH_LONG;
					
					Toast toast = Toast.makeText(mContext, text, duration);
					toast.show();

					Thread thread = new Thread(){
					             @Override
					            public void run() {
					                 try {
					                    Thread.sleep(10000); // As I am using LENGTH_LONG in Toast
					                    turnOffHdmi();
					                } catch (Exception e) {
					                    e.printStackTrace();
					                }
					             }  
					           };

					thread.start();
					*/
				}
				else if (bKernal == false && bootComplete == true)
				{
					mHandler.sendEmptyMessage(3);
					mHandler.sendEmptyMessageDelayed(2,10000);
				}
			}
			else
			{
				mHandler.removeMessages(0);
				mHandler.removeMessages(1);
				mHandler.removeMessages(2);
				mHandler.removeMessages(3);
			}
		}
	};

	private void hookHDMIPlugStatus(boolean bEnable) {
		if(bEnable) {
			if(!hdmiReceiverRegistered) {
				mContext.registerReceiver(hdmiHotPlugReceiver, hdmiFilter);
				hdmiReceiverRegistered = true;
				Slog.w(TAG, "hookHDMIPlugStatus register");
			}
		}
		else {
			if(hdmiReceiverRegistered) {
				mContext.unregisterReceiver(hdmiHotPlugReceiver);
				hdmiReceiverRegistered = false;
				Slog.w(TAG, "hookHDMIPlugStatus unregister");
			}
		}
	}

	private BroadcastReceiver bootCompleteReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			boolean bHDCP = true;
			boolean bKernal = true;

			bootComplete = true;
			
			if (hdcpEnabledByConfig)
			{
				bHDCP = checkIfHDCPKeyExist(); 
				bKernal = hdcpKernal;
			}

			Slog.w(TAG, "bootCompleteReceiver bHDCP =" + bHDCP + "\n");

			if (bHDCP == false)
			{
				mHandler.sendEmptyMessage(0);
				mHandler.sendEmptyMessageDelayed(2,10000);
				/*
				Context myContext = context.getApplicationContext();
				Slog.w(TAG, "HDCP Key doesn't exist!\n");
				CharSequence text = "HDCP Key doesn't exist!";
				int duration = Toast.LENGTH_LONG;
				
				Toast toast = Toast.makeText(mContext, text, duration);
				toast.show();

				Thread thread = new Thread(){
				             @Override
				            public void run() {
				                 try {
				                    Thread.sleep(10000); // As I am using LENGTH_LONG in Toast
				                    turnOffHdmi();
				                } catch (Exception e) {
				                    e.printStackTrace();
				                }
				             }  
				           };

				thread.start();
				*/
			}
			else if (bKernal == false)
			{
				mHandler.sendEmptyMessage(3);
				mHandler.sendEmptyMessageDelayed(2,10000);
			}
		}
	};

	private void hookBootComplete(boolean bEnable) {
		if(bEnable) {
			if(!bootCompleteRegistered) {
				mContext.registerReceiver(bootCompleteReceiver, bootFilter);
				bootCompleteRegistered = true;
				Slog.w(TAG, "bootCompleteRegistered register");
			}
		}
		else {
			if(bootCompleteRegistered) {
				mContext.unregisterReceiver(bootCompleteReceiver);
				bootCompleteRegistered = false;
				Slog.w(TAG, "bootCompleteRegistered unregister");
			}
		}
	}


	private UEventObserver hdcpObserver = new UEventObserver() {
		@Override
		public void onUEvent(UEventObserver.UEvent event) {
			dealHDCPState(Integer.parseInt(event.get("HDCP1x_STATE")));
		}
	};

	void dealHDCPState(int hdcpState) {
		Slog.w(TAG, "hdcpState: " + hdcpState + "\n");
		switch(hdcpState) {
			case 0: //HDCP_STATE_DISABLED
				{
				Slog.w(TAG, "hdcp disabled, turn off hdmi\n");
				mHandler.sendEmptyMessage(1);
				mHandler.sendEmptyMessageDelayed(2,10000);
				/*
				Context myContext = mContext.getApplicationContext();
				CharSequence text = "invalid HDMI key found, Turn off HDMI!";
				int duration = Toast.LENGTH_LONG;

				Looper.prepare(); 
				Toast toast = Toast.makeText(mContext, text, duration);
				toast.show();

				Thread thread = new Thread(){
							 @Override
							public void run() {
								 try {
									Thread.sleep(10000); // As I am using LENGTH_LONG in Toast
									turnOffHdmi();
								} catch (Exception e) {
									e.printStackTrace();
								}
							 }	
						   };
				
				thread.start();
				*/
				}
				break;
			case 1: //HDCP_STATE_INIT
				break;
			case 2: //HDCP_STATE_AUTH_1ST_STEP
				// save the HDCP information for Widi
				break;
			case 3: //HDCP_STATE_AUTH_2ND_STEP
				break;
			case 4: //HDCP_STATE_AUTH_3RD_STEP
				_saveHDCPToTmp();
				break;
			case 5: //HDCP_STATE_AUTH_FAIL_RESTARTING
				break;
			case 6: //HDCP_STATE_AUTH_FAILURE
				{
				Slog.w(TAG, "hdcp key fail, turn off hdmi\n");
				mHandler.sendEmptyMessage(1);
				mHandler.sendEmptyMessageDelayed(2,10000);
				/*
				Context myContext = mContext.getApplicationContext();
				CharSequence text = "invalid HDMI key found, Turn off HDMI!";
				int duration = Toast.LENGTH_LONG;
				
				Toast toast = Toast.makeText(mContext, text, duration);
				toast.show();				
				
				Thread thread = new Thread(){
							 @Override
							public void run() {
								 try {
									Thread.sleep(10000); // As I am using LENGTH_LONG in Toast
									turnOffHdmi();
								} catch (Exception e) {
									e.printStackTrace();
								}
							 }	
						   };
				
				thread.start();
				*/
				}
				break;
			default:
				break;
		}
	}

	void initializeHDCPState() {
		int hdcpState = 0;

		if(new File("/sys/devices/virtual/misc/hdcp/state").exists()) {
			hdcpKernal = true;
			Slog.w(TAG, "Initialize HDCP State");
			hdcpObserver.startObserving("DEVPATH=/devices/virtual/misc/hdcp");

			final String filename = "/sys/class/misc/hdcp/state";
			FileReader reader = null;
			try {
				reader = new FileReader(filename);
				char[] buf = new char[15];
				int n = reader.read(buf);
				if (n > 0) {
					hdcpState = Integer.parseInt(new String(buf, 0, 1));
				}
			} catch (IOException ex) {
				Slog.w(TAG, "Couldn't read hdcp state from " + filename + ": " + ex);
			} catch (NumberFormatException ex) {
				Slog.w(TAG, "Couldn't read hdcp state from " + filename + ": " + ex);
			} finally {
				if (reader != null) {
					try {
						reader.close();
					} catch (IOException ex) {
					}
				}
			}
		} else {
			hdcpKernal = false;
			Slog.w(TAG, "\"/sys/devices/virtual/misc/hdcp/state\" doesn't exist!!!");
		}

		//dealHDCPState(hdcpState);
	}

	public boolean turnOffHdmi() {
		synchronized (this) {
			return _turnOffHdmi();
		}
	}

	public boolean checkIfHDCPKeyExist() {
		synchronized (this) {
			return _checkIfHDCPKeyExist();
		}
	}

	public boolean checkIfHDMIPlugged() {
		synchronized (this) {
			return _checkIfHDMIPlugged();
		}
	}

	public boolean checkIfHDMIReady() {
		synchronized(this) {
			return _checkIfHDMIReady();
		}
	}

	/* Check if the box gets an valid EDID from the Sink (getHDMIEDIDReady() in Casablanca) */
	public boolean checkIfHDMIEDIDReady() {
		synchronized(this) {
			return _checkIfHDMIEDIDReady();
		}
	}

	public boolean sendAudioMute(int select) {
		synchronized(this) {
			return _sendAudioMute(select);
		}
	}

	public void setHDMIDeepColorMode(int mode) {
		synchronized(this) {
			_setHDMIDeepColorMode(mode);
		}
	}

	public void setHDMIFormat3D(int format3d, float fps) {
		synchronized(this) {
			_setHDMIFormat3D(format3d, fps);
		}
	}

	public int setHDCPState(int state) {
		synchronized(this) {
			return _setHDCPState(state);
		}
	}

	public int [] getVideoFormat() {
		synchronized(this) {
			int setupID = 0;
			int[] supportVideoFormat = new int[RtkTVSystem.TV_SYS_NUM];
			RtkTVSystem[] videoFormat = _getVideoFormat();

			for(int i = 0; i < videoFormat.length; i++) {
				setupID = convertRtkTVSystemToSetupID(videoFormat[i]);
				Slog.w(TAG, "setupID: " + setupID + "\n");
				if(setupID != -1) {
					supportVideoFormat[setupID] = 1;
				}
			}

			return supportVideoFormat;
		}
	}

	public int getNextNVideoFormat(int nextN) {
		synchronized(this) {
			int vid = 0;
			RtkTVSystem videoFormat = _getNextNVideoFormat(nextN);
			vid = convertRtkTVSystemToSetupID(videoFormat);
			Slog.w(TAG, "vid: " + vid + "\n");
			return vid;
		}
	}

	public int getTV3DCapability() {
		synchronized(this) {
			return _getTV3DCapability();
		}
	}

	public int[] getTVSupport3D() {
		synchronized(this) {
			return _getTVSupport3D();
		}
	}

	public int[] getTVSupport3DResolution() {
		synchronized(this) {
			return _getTVSupport3DResolution();
		}
	}

	public boolean isTVSupport3D() {
		synchronized(this) {
			return nativeIsTVSupport3D();
		}
	}

	public byte[] getEDIDRawData() {
		synchronized(this) {
			return nativeGetEDIDRawData();
		}
	}
	public int setTVSystem(int inputTvSystem) {
			final int tvSystem = inputTvSystem;
			Thread thread = new Thread(){
				@Override
				public void run()  {
					changeTVSystem(tvSystem);
				}
			};

			thread.start();

			return 0;
	}

	public int getTVSystem() {
		int videoSystem = getVideoSystemFromSettings();
		int videoStandard = getVideoStandardFromSettings();

		for(int i = 0; i < RtkTVSystem.tv_map.length; i++) {
			if(videoSystem != RtkTVSystem.tv_map[i][1]
				|| videoStandard != RtkTVSystem.tv_map[i][2]) {
				continue;
			} else {
				Slog.w(TAG, "getTVSystem, return value: " + RtkTVSystem.tv_map[i][0]);
				return RtkTVSystem.tv_map[i][0];
			}
		}

		return -1;
	}

	public int getTVSystemForRestored() {
		// override the restored value if it is "Auto"
		if(getTVSystemAutoModeFromSettings() == RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID) {
			Slog.w(TAG, "getTVSystemForRestored, return value: " + RtkTVSystem.TV_SYS_HDMI_AUTO_DETECT);
			return RtkTVSystem.TV_SYS_HDMI_AUTO_DETECT;
		} else {
			return getTVSystem();
		}
	}

	private int changeTVSystem(int tvSystem) {
		synchronized(this) {
			Slog.w(TAG, Thread.currentThread().getName() + ": change TV System: " + tvSystem + "!!\n");
			// Get the current TV system
			int intVideoSystem = getVideoSystemFromSettings();
			int intVideoStandard = getVideoStandardFromSettings();
			int tv_sys = -1;
			int tv_std = -1;
			boolean isCopyToVideoSystemManual = true;
			int intTVSystem = RtkTVSystem.VIDEO_SYSTEM_NUM;
			RtkVoutUtilManager mgr = new RtkVoutUtilManager();

			if(tvSystem != RtkTVSystem.TV_SYS_HDMI_AUTO_DETECT)
			{
				if(checkIfHDMIPlugged() && getTVSystemAutoModeFromSettings() == RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID)
				{
					setTVSystemAutoModeFromSettings(RtkTVSystem.TV_SYSTEM_AUTO_MODE_OFF);
				}
			}
			else
			{
				//Set to TV_SYSTEM_AUTO_MODE_HDMI_EDID
				int tvs[] = {RtkTVSystem.VIDEO_NTSC, RtkTVSystem.VIDEO_PAL, 
					RtkTVSystem.VIDEO_HD720_50HZ, RtkTVSystem.VIDEO_HD720_60HZ, 
					RtkTVSystem.VIDEO_HD1080_50HZ, RtkTVSystem.VIDEO_HD1080_60HZ,
					RtkTVSystem.VIDEO_HD4096_2160_24HZ, RtkTVSystem.VIDEO_HD4096_2160_25HZ, RtkTVSystem.VIDEO_HD4096_2160_30HZ, RtkTVSystem.VIDEO_HD4096_2160_50HZ,RtkTVSystem.VIDEO_HD4096_2160_60HZ,
					RtkTVSystem.VIDEO_HD2160_24HZ, RtkTVSystem.VIDEO_HD2160_25HZ, RtkTVSystem.VIDEO_HD2160_30HZ, RtkTVSystem.VIDEO_HD2160_50HZ, RtkTVSystem.VIDEO_HD2160_60HZ};

				int len;
				int validIDFound = 0;
				setTVSystemAutoModeFromSettings(RtkTVSystem.TV_SYSTEM_AUTO_MODE_HDMI_EDID);
				if(checkIfHDMIPlugged())
				{
					RtkTVSystem arrTVSystem[] = _getVideoFormat();
					if(arrTVSystem.length > 0)
					{
						// No need to backup HDMI_EDID setting value to VideoSystemManual when HDMI_AUTO=ON
						isCopyToVideoSystemManual = false;
						// Search the valid TV system from the highest resolution.
						for(int ii=tvs.length-1; ii>=0 && (validIDFound == 0); ii--)
						{
							for(int jj=1; jj>=0 && (validIDFound == 0); jj--)
							{
								for(int kk=0; kk<arrTVSystem.length; kk++)
								{
									if(tvs[ii] == arrTVSystem[kk].intVideoSystem && jj == arrTVSystem[kk].intVideoStandard)
									{
										tv_sys = tvs[ii];
										tv_std = jj;
										validIDFound = 1;
										Slog.w(TAG, "tv_sys: " + tv_sys + "; tv_std: " +  tv_std + "\n");
										break;
									}
								}
							}
						}
					}
				}

				if(tv_sys == -1 || tv_std == -1)
				{
					tv_sys = RtkTVSystem.VIDEO_NTSC;
					tv_std = RtkTVSystem.VIDEO_PROGRESSIVE;
				}
			}

			if(tv_sys != -1 && tv_std != -1)
			{
				SetTVSystemAndStandardToSetup(tv_sys, tv_std);
			}
			else
			{
				for(int ii = 0; ii < RtkTVSystem.tv_map.length; ii++)
				{
					if(RtkTVSystem.tv_map[ii][0] == tvSystem)
					{
						tv_sys = RtkTVSystem.tv_map[ii][1];
						tv_std = RtkTVSystem.tv_map[ii][2];
						SetTVSystemAndStandardToSetup(tv_sys, tv_std);
						break;
					}
				}
			}

			//Fix bug#25989 (HDMI Spec change)
			if(isCopyToVideoSystemManual)
			{
				SetTVSystemManualAndStandardManualToSetup(tv_sys, tv_std);
			}

			SetTVSystemAndStandardToSetup(tv_sys, tv_std);

			_updateHDMISetting(tv_sys, tv_std, getTVSystemAutoModeFromSettings());
			// call ReloadTVSystem to reload TV system
			reloadTVSystem(false, true, false, true, RtkTVSystem.VIDEO_SYSTEM_NUM);

			// reset zoom
			if(mgr.isZooming() == true) {
				// reset zoom
				mgr.resetZoom();
			}
			Slog.w(TAG, Thread.currentThread().getName() + ": change TV System: " + tvSystem + " done!!\n");
		}
		return 0;
	}

	private void reloadTVSystem(boolean bResetVideoRect, boolean saveTvSystemToNand, boolean bCheckNand, boolean bUpdateVideo, int vSystem) {
		Slog.w(TAG, "reloadTVSystem is called!");
		nativeReloadTVSystem(bResetVideoRect, saveTvSystemToNand, bCheckNand, bUpdateVideo, vSystem);
	}

	private int getItemValueFromSettings(String itemName) {
		int itemValue = 0;

		try {
			itemValue = Settings.System.getInt(mContext.getContentResolver(), itemName);
		} catch (SettingNotFoundException snfe) {
			// error handling
			itemValue = -1;
		}
		Slog.w(TAG, "getItemValueFromSettings itemName: " + itemName + " itemValue: " + itemValue);
		return itemValue;
	}

	private int getVideoSystemFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM);
	}

	private int getVideoStandardFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_STANDARD);
	}

	private int getAspectRatioFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_ASPECT_RATIO);
	}

	private int getTVSystemAutoModeFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM_AUTO_MODE);
	}

	private int getTVSystemManualFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM_MANUAL);
	}

	private int getTVStandardManualFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_TV_STANDARD_MANUAL);
	}

	private int getVideoOutputFromSettings() {
		return getItemValueFromSettings(Settings.System.REALTEK_SETUP_VIDEO_OUTPUT);
	}

	private void setItemValueToSettings(String itemName, int itemValue) {
		Slog.w(TAG, "setItemValue to Settings itemName: " + itemName + " itemValue: " + itemValue);
		Settings.System.putInt(mContext.getContentResolver(), itemName, itemValue);
	}

	public void SetTVSystemAndStandardToSetup(int tvSystem, int tvStandard) {
		setItemValueToSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM, tvSystem);
		setItemValueToSettings(Settings.System.REALTEK_SETUP_TV_STANDARD, tvStandard);
	}

	public void setTVSystemAutoModeFromSettings(int autoMode) {
		setItemValueToSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM_AUTO_MODE, autoMode);
	}

	public void SetTVSystemManualAndStandardManualToSetup(int tvSystemManual, int tvStandardManual) {
		setItemValueToSettings(Settings.System.REALTEK_SETUP_TV_SYSTEM_MANUAL, tvSystemManual);
		setItemValueToSettings(Settings.System.REALTEK_SETUP_TV_STANDARD_MANUAL, tvStandardManual);
	}

	private int convertRtkTVSystemToSetupID(RtkTVSystem rtkTVSystem) {
		for(int i = 0; i < RtkTVSystem.tv_map.length; i++) {
			if(rtkTVSystem.intVideoSystem != RtkTVSystem.tv_map[i][1]
				|| rtkTVSystem.intVideoStandard != RtkTVSystem.tv_map[i][2]) {
				continue;
			} else {
				return  RtkTVSystem.tv_map[i][0];
			}
		}
		// NOT found, return -1;
		return -1;
	}

	private native boolean _turnOffHdmi();
	private native boolean _checkIfHDCPKeyExist();
	private native boolean _checkIfHDMIPlugged();
	private native boolean _checkIfHDMIReady();
	private native boolean _checkIfHDMIEDIDReady();
	private native boolean _sendAudioMute(int select);
	private native void _setHDMIDeepColorMode(int mode);
	private native void _setHDMIFormat3D(int format3d, float fps);
	private native int _setHDCPState(int state);
	private native RtkTVSystem[] _getVideoFormat();
	private native RtkTVSystem _getNextNVideoFormat(int nextN);
	private native int _getTV3DCapability();
	private native int[] _getTVSupport3D();
	private native int[] _getTVSupport3DResolution();
	private native int initMediaLinkController(int mode);
	private native void nativeReloadTVSystem(boolean bResetVideoRect, boolean saveTvSystemToNand, boolean bCheckNand, boolean bUpdateVideo, int vSystem);
	private native void handleHotPlug(int hdmiPlugged);
	private native boolean nativeIsTVSupport3D();
	private native byte[] nativeGetEDIDRawData();
	private native boolean _saveHDCPToTmp();
	private native void _setHdcpEnabledByConfig(boolean enabled);
	private native void _updateHDMISetting(int videoSystem, int videoStandard, int autoMode);
    private native void _writeHDMIEnabledNandTVSystem(int interfaceType, boolean enabled);
}
