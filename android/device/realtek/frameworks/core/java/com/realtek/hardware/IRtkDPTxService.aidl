package com.realtek.hardware;

interface IRtkDPTxService
{
	/* Get DPTx status */
	boolean checkifDPTxPlugged();
	boolean checkIfDPTxEDIDReady();

	/* Get all the available TV system */
	int[] getVideoFormat();

	int setTVSystem(int inputTvSystem);
	int getTVSystem();
	
}
