/*
 * Copyright (c) 2013 Realtek Semi. co. All Rights Reserved.
 * Author Kevin Su
 * Version V0.1
 * Date:  2013-08-09
 * Comment:This class lets you access the DPTx function.
 */

package com.realtek.hardware;

import java.util.StringTokenizer;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import com.realtek.server.RtkTVSystem;
import com.realtek.hardware.IRtkDPTxService;

public class RtkDPTxManager {
	private static final String TAG = "RtkDPTxManager";

	private IRtkDPTxService mRtkDPTxService;

	public RtkDPTxManager() {
		mRtkDPTxService = IRtkDPTxService.Stub.asInterface(ServiceManager.getService("RtkDPTxService"));
		if(mRtkDPTxService != null) {
			Log.i(TAG, mRtkDPTxService.toString());
		}
		else {
			Log.e(TAG, "error! CANNOT get RtkDPTxService!");
		}
	}

	// check if DPTx cable is connected.
	public boolean checkIfDPTxPlugged() {
		boolean result = false;

		if(mRtkDPTxService == null) {
			Log.e(TAG, "failed to connect to RtkDPTxService!");
			return false;
		}

		try {
			result = mRtkDPTxService.checkifDPTxPlugged();
		} catch (Exception e) {
			Log.e(TAG, "RtkDPTxservice checkIfDPTxPlugged failed!", e);
		}

		return result;
	}

	// If EDID is ready, it means that EDID is valid.
	public boolean getDPTxEDIDReady() {
		boolean result = false;

		if(mRtkDPTxService == null) {
			Log.e(TAG, "failed to connect to RtkDPTxService!");
			return false;
		}

		try {
			result = mRtkDPTxService.checkIfDPTxEDIDReady();
		} catch (Exception e) {
			Log.e(TAG, "RtkDPTxService checkIfDPTxEDIDReady failed!", e);
		}

		return result;
	}

	public int[] getVideoFormat() {
		int[] arrTVSystem = null;

		if(mRtkDPTxService == null) {
			Log.e(TAG, "failed to connect to RtkDPTxService!");
			return null;
		}

		try {
			arrTVSystem = mRtkDPTxService.getVideoFormat();
		} catch (Exception e) {
			Log.e(TAG, "RtkDPTxService getVideoFormat failed!", e);
		}

		return arrTVSystem;
	}

	public int setTVSystem(int tvSystem) {
		int result = 0;

		if(mRtkDPTxService == null) {
			Log.e(TAG, "failed to connect to RtkDPTxService!");
			return -1;
		}

		try {
			result = mRtkDPTxService.setTVSystem(tvSystem);
		} catch (Exception e) {
			Log.e(TAG, "RtkDPTxService setTVSystem failed!", e);
		}

		return result;
	}

	public int getTVSystem() {
		int tvSystem = 0;
		if(mRtkDPTxService == null) {
			Log.e(TAG, "failed to connect to RtkDPTxService!");
			return -1;
		}

		try {
			tvSystem = mRtkDPTxService.getTVSystem();
		} catch (Exception e) {
			Log.e(TAG, "RtkDPTxService getTVSystem!", e);
		}

		return tvSystem;
	}

}
