package com.realtek.hardware;


interface IRtkCECService
{
	//Ask OneTouchPlay
	boolean doOneTouchPlay(int Key);
    int SendRemotePressed(int Dev, int Key, boolean SendReleased);
}
