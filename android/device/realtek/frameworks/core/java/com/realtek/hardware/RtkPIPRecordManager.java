/*
 * Copyright (c) 2017 Realtek Semi. co. All Rights Reserved.
 * Comment: This class lets you access Realtek Transcode Service functions.
 */

package com.realtek.hardware;

import java.util.StringTokenizer;
import java.io.IOException;

import android.app.ActivityThread;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.ParcelFileDescriptor;
import android.os.SystemProperties;
import android.util.Log;

public class RtkPIPRecordManager {
	static {
        System.loadLibrary("realtek_runtime");
	}

	private static final String TAG = "RtkPIPRecordManager";

    public static final int TRACK_VIDEO_AND_AUDIO = 0;
    public static final int TRACK_AUDIO_ONLY = 1;
    public static final int TRACK_VIDEO_ONLY = 2;
      
    public static final int OUTPUT_FILE_FORMAT_TS  = 0;
    public static final int OUTPUT_FILE_FORMAT_MP4 = 1;

    public static final int TRANSCODED_TYPE_MIXER1 = 4;
    public static final int TRANSCODED_TYPE_V1 = 5;

	public RtkPIPRecordManager() {
        Log.d(TAG, "construct RtkPIPRecordManager instance");
        native_initiate();
	}

    public int prepare(int type) {
        String packageName = ActivityThread.currentPackageName();
		Log.d(TAG, "prepare, package name:" + packageName);
		return native_prepare(packageName, type);
    }

    public void configureTargetFormat(RtkPIPRecordManager.VideoConfig vConfig,
                                      RtkPIPRecordManager.AudioConfig aConfig) {
        Log.d(TAG, "configureTargetFormat");
        native_configureTargetFormat(vConfig.width, vConfig.height, vConfig.fps,
                                     vConfig.bitrate, vConfig.iframeInterval, vConfig.rateControl, 
                                     vConfig.aspectRatio, vConfig.interlace, vConfig.shareWBbuf,
                                     aConfig.channelCount, aConfig.channelMode,
                                     aConfig.sampleRate, aConfig.bitrate);
    }
    
    public void setTargetFd(ParcelFileDescriptor fd, int fileFormat) {
        Log.d(TAG, "setTargetFd fd:"+fd);
        native_setTargetFd(fd.getFd(), fileFormat);
    }
    
    public int start() {
		Log.d(TAG, "start");
		int result = native_startTranscode();
		return result;
    }
    
    public int stop() {
		Log.d(TAG, "stop");
		int result = native_stopTranscode();
		return result;
    }
    
    public void release() {
		Log.d(TAG, "release");
		native_release();
    }

    //------------------------------------------------------------------- 
    private native void native_initiate();
    private native int native_prepare(String packageName, int type);
    private native void native_configureTargetFormat(// video setting
                                                     int w, int h, int fps,
                                                     int vbitrate, int iframeInterval,
                                                     int rateControl, int aspectRatio,
                                                     int interlace, int shareWBbuf,
                                                     // audio setting
                                                     int channelCount, int channelMode, int sampleRate, int abitrate);    
    private native void native_setTargetFd(int fd, int format);
    private native int native_startTranscode();
    private native int native_stopTranscode();
    private native void native_release();
    
    /**
     * A simple class to put all video config parameters
     */
    public static class VideoConfig {

        public int width;
        public int height;
        public int fps;
        public int bitrate;
        public int iframeInterval;
        public int rateControl;
        public int aspectRatio;
        public int interlace;
        public int shareWBbuf;

        public VideoConfig(
                int w,
                int h,
                int fps,
                int bitrate,
                int iframeInterval,
                int rateControl,
                int aspectRatio,
                int interlace,
                int shareWBbuf) {

            this.width = w;
            this.height = h;
            this.fps = fps;
            this.bitrate = bitrate;
            this.iframeInterval = iframeInterval;
            this.rateControl = rateControl;
            this.aspectRatio = aspectRatio;
            this.interlace = interlace;
            this.shareWBbuf = shareWBbuf;
        }
    }

    /**
     * A simple class to put all audio config parameters
     */
    public static class AudioConfig {

        public int channelCount;
        public int channelMode;
        public int sampleRate;
        public int bitrate;

        public AudioConfig(
                int channelCount,
                int channelMode,
                int sampleRate,
                int bitrate) {
            this.channelCount = channelCount;
            this.channelMode = channelMode;
            this.sampleRate = sampleRate;
            this.bitrate = bitrate;
        }
    }
}
