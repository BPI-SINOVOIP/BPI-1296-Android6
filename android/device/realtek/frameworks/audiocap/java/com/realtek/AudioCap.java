package com.realtek; 

import android.util.Log;

final public class AudioCap {
		
	public static final int NORMAL_MODE = 0;
	public static final int CAP_MODE = 1;

	static final String LOG_TAG = "AudioCap";
	
	native void native_set_cap_mode(int mode);
	native void native_start_audio_cap();
	native void native_stop_audio_cap();
	native byte[] native_get_one_pcmbuffer();
	native void native_flush_pcmbufffer_queue();
	
	static {
		System.loadLibrary("audiocap_jni");
	}
	
	public int setCapMode(int mode) {
		if (mode != NORMAL_MODE && mode != CAP_MODE) {
			Log.d(LOG_TAG, "INVALID PARAMETERS,ONLY CAP_MODE OR NORMAL_MODE!");
			return -1;
		}
		native_set_cap_mode(mode);
		return 0;
	}
	
	public void startAudioCap() {
		native_start_audio_cap();
	}
	
	public void stopAudioCap() {
		native_stop_audio_cap();
	}
	
	public byte[] GetOnePCMBuffer() {
		return native_get_one_pcmbuffer();
	} 
	
	public void FlushPCMBufferQueue() {
		native_flush_pcmbufffer_queue();
	}
}