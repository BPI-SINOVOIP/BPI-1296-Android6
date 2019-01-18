package com.realtek.hardware;

import android.graphics.Rect;

interface IRtkVoutUtilService
{
    boolean init();
    boolean setRescaleMode(int mode);
    boolean setOSDWindow(in Rect osdWin);
    boolean nextZoom();
    boolean prevZoom();
    boolean isZooming();
    boolean resetZoom();
    boolean moveZoom(int keycode);
    boolean setFormat3d(int format3d, float fps);
    boolean set3dto2d(int srcformat3d);
    boolean set3dSub(int sub3d);
    boolean applyVoutDisplayWindowSetup();
    boolean setDisplayRatio(int ratio);
    boolean setDisplayPosition(int x, int y, int width, int height);
    void setEnhancedSDR(int flag);
    boolean isHDRtv();
    boolean setShiftOffset3dByPlane(boolean exchange_eyeview, boolean shift_direction, int delta_offset, int targetPlane);
    void setHdmiRange(int rangeMode);
    void setCVBSDisplayRatio(int ratio);
    void setHDMICVBSDisplayRatio(int ratio);
    void setDPDisplayRatio(int ratio);
    void setDPDisplayBCHSSetting(int brightness, int contrast, int hue, int saturation) ;
    void SetDPDisplayXYOffset(int x, int y);
    void setHDROff(int hdr_off);
    boolean isCVBSOn();
    void setCVBSOff(int off);
    void setEmbedSubDisplayFixed(int fixed);

}
