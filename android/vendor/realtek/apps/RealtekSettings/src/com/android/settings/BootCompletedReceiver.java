package com.android.settings;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.util.Log;

import android.provider.Settings;
import com.realtek.hardware.RtkAoutUtilManager;
import com.realtek.hardware.RtkVoutUtilManager;
import android.provider.Settings.SettingNotFoundException;
import com.rtk.net.openwrt.Server;
import com.rtk.net.openwrt.UbusRpc;
import com.rtk.net.openwrt.DlnaStatus;
import android.os.AsyncTask;

public class BootCompletedReceiver extends BroadcastReceiver {

    private static final String TAG = "BootCompletedReceiver";
    private Context context;
    private RtkAoutUtilManager mRtkAoutUtilManager;
    private RtkVoutUtilManager mRtkVoutUtilManager;

    private static final int AUDIO_DIGITAL_RAW = 0;
    private static final int AUDIO_DIGITAL_LPCM_DUAL_CH = 1;
    private static final int AUDIO_DIGITAL_LPCM_MULTI_CH = 2;
    private static final int AUDIO_DIGITAL_AUTO = 3;

    private static final int FORCE_DTS_2CH = (1 << 0);
    private static final int FORCE_DTSHD_2CH = (1 << 1);
    private static final int FORCE_AC3_2CH = (1 << 2);
    private static final int FORCE_DDP_2CH = (1 << 3);
    private static final int FORCE_MLP_2CH = (1 << 4);
    private static final int FORCE_AAC_2CH = (1 << 5);
    private static final int FORCE_WMAPRO_2CH = (1 << 6);

    private static final String SCREEN_ZOOM_IN_PROPERTY = "persist.rtk.zoomin";
    private static final String PROPERTY_AUDIO_NATIVE_SR = "persist.rtk.audio.native_sr";
    private static final String PROPERTY_HDMI_RANGE_MODE = "persist.rtk.hdmi.rangemode";
    private static final String PROPERTY_CVBS_ZOOM_IN_RATIO = "persist.rtk.vout.cvbszoom.ratio";
    private static final String PROPERTY_HDMI_DP_DIFFERENT_SOURCE = "persist.rtk.hdmitx.dpdiffsource";
    private static final String PROPERTY_HDMI_CVBS_ZOOM_IN_RATIO = "persist.rtk.vout.hdmicvbszoom";
    private static final String PROPERTY_DP_ZOOM_IN_RATIO = "persist.rtk.vout.dpzoom.ratio";
    private static final String PROPERTY_HDR_ONOFF = "persist.rtk.video.hdr";
    private final String PROPERTY_VIDEO_CVBS = "persist.rtk.video.cvbs";
    private static final String PROPERTY_SUB_FIXED = "persist.rtk.video.subfixed";

    @Override
    public void onReceive(Context arg0, Intent arg1) {
        Log.d(TAG, "Set system settings from database...");
        this.context = arg0;

        mRtkAoutUtilManager = new RtkAoutUtilManager();
        if(mRtkAoutUtilManager == null)
            Log.d(TAG, "mRtkAoutUtilManager is null!");

        mRtkVoutUtilManager = new RtkVoutUtilManager();
        if(mRtkVoutUtilManager == null)
            Log.d(TAG, "mRtkVoutUtilManager is null!");

        setDefaultAudioHdmiOutput();
        setDefaultAudioSpdifOutput();
        setDefaultPlaybackEffect();
        setDefaultAudioForceChannelCtrl();  // we set DTS/DTSHD/AC3/DDP/TRUE HD format forcely output 2 channel, excluding AAC/WMAPRO.
        setAudioHdmiFreqMode();
        setDefaultScreenZoom();
        setDefaultAudioSurroundSoundMode();
        if (UbusRpc.isOpenWrt()) {
            new SetInitDMSNameSameWithDeviceName().execute();
        }
        setDefaultAudioNativeSR();
        setDefaultHdmiRangeMode();
        setDefaultCVBSZoomIn();
        setDefaultHDMICVBSZoomIn();
        setDefaultDPZoomIn();
        setDefaultHdrMode();
        setDefaultVideoCVBS();
        setDefaultSubDisplayMode();
    }

    //HDMI output
    private void setDefaultAudioHdmiOutput() {
        String str;
        str = Settings.System.getString(this.context.getContentResolver(), Settings.System.REALTEK_SETUP_HDMI_OUTPUT_MODE);
        if ( str.equals("0") ) { //RAW
            Log.d(TAG, "Set HDMI Output mode = RAW");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioHdmiOutput(AUDIO_DIGITAL_RAW);
            }
        } else if ( str.equals("1") ) { //LPCM 2CH
            Log.d(TAG, "Set HDMI Output mode = 2CH");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioHdmiOutput(AUDIO_DIGITAL_LPCM_DUAL_CH);
            }
        } else if ( str.equals("2") ) { //LPCM Multi-CH
            Log.d(TAG, "Set HDMI Output mode = MULTI_CH");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioHdmiOutput(AUDIO_DIGITAL_LPCM_MULTI_CH);
            }
        } else if ( str.equals("3") ) { //AUTO
            Log.d(TAG, "Set HDMI Output mode = AUTO");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioHdmiOutput(AUDIO_DIGITAL_AUTO);
            }
        } else {
            Log.d(TAG, "Unsupportd HDMI Output");
        }
        str = null;
    }

    //S/PDIF output
    private void setDefaultAudioSpdifOutput() {
        String str;
        str = Settings.System.getString(this.context.getContentResolver(), Settings.System.REALTEK_SETUP_SPDIF_OUTPUT_MODE);
        if ( str.equals("0") ) { //LPCM
            Log.d(TAG, "Set SPDIF Output mode = LPCM");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioSpdifOutput(AUDIO_DIGITAL_LPCM_DUAL_CH);
            }
        } else if ( str.equals("1") ) { //Raw
            Log.d(TAG, "Set SPDIF Output mode = RAW");
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioSpdifOutput(AUDIO_DIGITAL_RAW);
            }
        } else {
            Log.d(TAG, "Unsupportd S/PDIF Output");
        }
        str = null;
    }

    //Playback effect
    private void setDefaultPlaybackEffect() {
        String str;
        str = Settings.System.getString(this.context.getContentResolver(), Settings.System.REALTEK_SETUP_PLAYBACK_EFFECT_MODE);
        Log.d(TAG, "Set Playback effect mode = " + str);
        int agcMode = 0;
        if ( str.equals("night") ) {
            agcMode=1;
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioAGC(agcMode);
            }
        } else if ( str.equals("comfort") ) {
            agcMode=2;
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioAGC(agcMode);
            }
        } else { //OFF
            agcMode=0;
            if (mRtkAoutUtilManager != null) {
                mRtkAoutUtilManager.setAudioAGC(agcMode);
            }
        }
    }

    // Audio Force 2 channel ctrl, this setting will not affected the RAW output, so set it always.
    private void setDefaultAudioForceChannelCtrl() {
        Log.d(TAG, "Set audio force channel");
        if (mRtkAoutUtilManager != null) {
            int forceCtrl = FORCE_DTS_2CH|FORCE_AC3_2CH|FORCE_DTSHD_2CH|FORCE_DDP_2CH|FORCE_MLP_2CH;
            mRtkAoutUtilManager.setAudioForceChannelCtrl(forceCtrl);
            Log.d(TAG, "Set audio force channel, value = " + forceCtrl);
        }
    }

    // set hdmi frequency to audio.
    private void setAudioHdmiFreqMode() {
        Log.d(TAG, "Set audio hdmi frequency mode");
        if (mRtkAoutUtilManager != null) {
            mRtkAoutUtilManager.setAudioHdmiFreqMode();
        }
    }

    private void setDefaultScreenZoom() {
        Log.d(TAG, "Set default Screen Zoom");
        if ("1".equals(SystemProperties.get(SCREEN_ZOOM_IN_PROPERTY,"0"))) {
            Log.d(TAG, "Set default Screen Zoom to 95%");
            mRtkVoutUtilManager.setDisplayRatio(95);  //95% Zoom
        }
    }

    private void setDefaultAudioSurroundSoundMode() {
        Log.d(TAG, "Set default Audio Surround Sound Mode...");
        if (mRtkAoutUtilManager == null)
            return;

        int AUDIO_UNKNOWN_TYPE = 0; //don't care
        int AUDIO_AC3_DECODER_TYPE = 2; //SD audio
        int AUDIO_MLP_DECODER_TYPE = 9; //HD audio

        int mode = AUDIO_UNKNOWN_TYPE;
        try {
            mode = Settings.System.getInt(this.context.getContentResolver(), Settings.System.REALTEK_SETUP_FORCE_SDAUDIO);
        } catch (SettingNotFoundException snfe) {
            Log.e(TAG, Settings.System.REALTEK_SETUP_FORCE_SDAUDIO + " not found");
        }

        mRtkAoutUtilManager.setAudioSurroundSoundMode(mode);
    }

    public class SetInitDMSNameSameWithDeviceName extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... unused) {
            Server server = new Server(context);
            if (server.isConnected()) {
                String mDeviceName = Settings.Global.getString(context.getContentResolver(), Settings.Global.DEVICE_NAME);
                String mDMSName = server.getDMSName();
                if (!mDeviceName.equals(mDMSName)) {
                    server.setDMSName(mDeviceName);
                    Server.Service sv = null;
                    sv = Server.Service.DLNA;
                    DlnaStatus dlnaStatus = (DlnaStatus) server.getServiceStatus(Server.Service.DLNA);
                    if (dlnaStatus.isRunning) {
                        server.enableService(sv, "disable");
                        server.enableService(sv, "enable");
                    }
                }
            }
            return null;
        }
    }

    private void setDefaultAudioNativeSR() {
        if (mRtkAoutUtilManager == null)
            return;
        if (SystemProperties.get(PROPERTY_AUDIO_NATIVE_SR, "0").equals("1")) {
            Log.d(TAG, "Set default Audio Native Sampling Rate.");
            mRtkAoutUtilManager.setAudioNativeSR(1);
        }
    }

    private void setDefaultHdmiRangeMode() {
        Log.d(TAG, "Set default HDMI Range Mode");
        if (mRtkVoutUtilManager != null) {
            int mode = Integer.parseInt(SystemProperties.get(PROPERTY_HDMI_RANGE_MODE, "0"));
            if (mode != 0)
                mRtkVoutUtilManager.setHdmiRange(mode);
        }
    }

    private void setDefaultCVBSZoomIn() {
        Log.d(TAG, "Set default CVBS Zoom In Mode");
        if (mRtkVoutUtilManager != null) {
            if(SystemProperties.get(PROPERTY_HDMI_DP_DIFFERENT_SOURCE).equals("on")) {
                int mode = Integer.parseInt(SystemProperties.get(PROPERTY_CVBS_ZOOM_IN_RATIO, "100"));
                if (mode == 95)
                    mRtkVoutUtilManager.setCVBSDisplayRatio(95);
            }
        }
    }

    private void setDefaultHDMICVBSZoomIn() {
        Log.d(TAG, "Set default HDMI and CVBS Zoom In Mode");
        if (mRtkVoutUtilManager != null) {
            if(SystemProperties.get(PROPERTY_HDMI_DP_DIFFERENT_SOURCE).equals("on")) {
                int mode = Integer.parseInt(SystemProperties.get(PROPERTY_HDMI_CVBS_ZOOM_IN_RATIO, "100"));
                if (mode == 95)
                    mRtkVoutUtilManager.setHDMICVBSDisplayRatio(95);
            }
        }
    }

    private void setDefaultDPZoomIn() {
        Log.d(TAG, "Set default DP Zoom In Mode");
        if (mRtkVoutUtilManager != null) {
            int mode = Integer.parseInt(SystemProperties.get(PROPERTY_DP_ZOOM_IN_RATIO, "100"));
            if (mode == 95)
                mRtkVoutUtilManager.setDPDisplayRatio(95);
        }
    }

    private void setDefaultHdrMode() {
        Log.d(TAG, "Set default HDR Mode");
        if (mRtkVoutUtilManager != null) {
            String hdrOn = SystemProperties.get(PROPERTY_HDR_ONOFF, "1");
            if(hdrOn.equals("0")) {
                mRtkVoutUtilManager.setHDROff(1);
            }
        }
    }

    private void setDefaultVideoCVBS() {
        if (mRtkVoutUtilManager != null) {
            boolean cvbsOn = new RtkVoutUtilManager().isCVBSOn();
            String cvbsProp = SystemProperties.get(PROPERTY_VIDEO_CVBS, cvbsOn?"1":"0");
            if(cvbsProp.equals("0") && cvbsOn == true) {
                mRtkVoutUtilManager.setCVBSOff(1);
            } else if(cvbsProp.equals("1") && cvbsOn == false) {
                mRtkVoutUtilManager.setCVBSOff(0);
            }
        }
    }

    private void setDefaultSubDisplayMode() {
        Log.d(TAG, "Set default Sub Display Mode");
        if (mRtkVoutUtilManager != null) {
            String subfixed = SystemProperties.get(PROPERTY_SUB_FIXED, "0");
            if(subfixed.equals("1")) {
                mRtkVoutUtilManager.setEmbedSubDisplayFixed(1);
            }
        }
    }

}

