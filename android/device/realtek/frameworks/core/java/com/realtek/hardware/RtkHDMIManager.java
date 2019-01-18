/*
 * Copyright (c) 2013 Realtek Semi. co. All Rights Reserved.
 * Author Kevin Su
 * Version V0.1
 * Date:  2013-08-09
 * Comment:This class lets you access the HDMI function.
 */

package com.realtek.hardware;

import java.util.StringTokenizer;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import com.realtek.server.RtkTVSystem;
import com.realtek.hardware.IRtkHDMIService;

public class RtkHDMIManager {
	private static final String TAG = "RtkHDMIManager";

	private IRtkHDMIService mRtkHDMIService;

	public RtkHDMIManager() {
		mRtkHDMIService = IRtkHDMIService.Stub.asInterface(ServiceManager.getService("RtkHDMIService"));
		if(mRtkHDMIService != null) {
			Log.i(TAG, mRtkHDMIService.toString());
		}
		else {
			Log.e(TAG, "error! CANNOT get RtkHDMIService!");
		}
	}

	// check if HDMI cable is connected.
	public boolean checkIfHDMIPlugged() {
		boolean result = false;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return false;
		}

		try {
			result = mRtkHDMIService.checkIfHDMIPlugged();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIservice checkIfHDMIPlugged failed!", e);
		}

		return result;
	}

	// check if HDMI is ok (Finish switching TV system)
	public boolean checkIfHDMIReady() {
		boolean result = false;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return false;
		}

		try {
			result = mRtkHDMIService.checkIfHDMIReady();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService checkIfHDMIReady failed!", e);
		}

		return result;
	}

	// If EDID is ready, it means that EDID is valid.
	public boolean getHDMIEDIDReady() {
		boolean result = false;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return false;
		}

		try {
			result = mRtkHDMIService.checkIfHDMIEDIDReady();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService checkIfHDMIEDIDReady failed!", e);
		}

		return result;
	}

	public boolean sendAudioMute(int select) {
		boolean result = false;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return false;
		}

		try {
			result = mRtkHDMIService.sendAudioMute(select);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService SendAudioMute failed!", e);
		}

		return result;
	}

	public void setHDMIDeepColorMode(int mode) {
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
		}

		try {
			mRtkHDMIService.setHDMIDeepColorMode(mode);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService setHDMIDeepColorMode failed!", e);
		}
	}


	public void setHDMIFormat3D(int format3d, float fps) {
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
		}

		try {
			mRtkHDMIService.setHDMIFormat3D(format3d, fps);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService setHDMIFormat3D failed!", e);
		}
	}

	public int setHDCPState(int state) {
		int result = 0;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return -1;
		}

		try {
			result = mRtkHDMIService.setHDCPState(state);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService setHDCPState failed!", e);
		}

		return result;
	}

	public int[] getVideoFormat() {
		int[] arrTVSystem = null;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return null;
		}

		try {
			arrTVSystem = mRtkHDMIService.getVideoFormat();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getVideoFormat failed!", e);
		}

		return arrTVSystem;
	}

	public int getNextNVideoFormat(int nextN) {
		 int vid = 0;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return 0;
		}

		try {
			vid = mRtkHDMIService.getNextNVideoFormat(nextN);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getNextNVideoFormat failed!", e);
		}

		return vid;
	}

	public int getTV3DCapability() {
		int result = 0;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return -1;
		}

		try {
			result = mRtkHDMIService.getTV3DCapability();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getTV3DCapability failed!", e);
		}

		return result;
	}

	public int[] getTVSupport3D() {
		int[] arrResult = {0};
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return null;
		}

		try {
			arrResult = mRtkHDMIService.getTVSupport3D();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getTVSupport3D!", e);
		}

		return arrResult;
	}

	public int[] getTVSupport3DResolution() {
		int[] arrResult = {0};
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return null;
		}

		try {
			arrResult = mRtkHDMIService.getTVSupport3DResolution();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getTVSupport3DResolution!", e);
		}

		return arrResult;
	}

	public int setTVSystem(int tvSystem) {
		int result = 0;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return -1;
		}

		try {
			result = mRtkHDMIService.setTVSystem(tvSystem);
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService setTVSystem failed!", e);
		}

		return result;
	}

	public boolean isTVSupport3D() {
		boolean result = false;

		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return false;
		}

		try {
			result = mRtkHDMIService.isTVSupport3D();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService isTVSupport3D failed!", e);
		}

		return result;
	}

	public byte[] getEDIDRawData() {
		byte[] arrResult = {0};
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return null;
		}

		try {
			arrResult = mRtkHDMIService.getEDIDRawData();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getEDIDRawData!", e);
		}

		return arrResult;
	}

	public int getTVSystem() {
		int tvSystem = 0;
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return -1;
		}

		try {
			tvSystem = mRtkHDMIService.getTVSystem();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getTVSystem!", e);
		}

		return tvSystem;
	}

	public int getTVSystemForRestored() {
		int tvSystem = 0;
		if(mRtkHDMIService == null) {
			Log.e(TAG, "failed to connect to RtkHDMIService!");
			return -1;
		}

		try {
			tvSystem = mRtkHDMIService.getTVSystemForRestored();
		} catch (Exception e) {
			Log.e(TAG, "RtkHDMIService getTVSystemForRestored!", e);
		}

		return tvSystem;
	}
}
