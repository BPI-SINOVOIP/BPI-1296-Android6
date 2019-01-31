#include <utils/Log.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <cutils/properties.h>

#include <jni.h>
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
//#include "Platform_Lib/HDMIControl/hdmi.h"
//#include "Platform_Lib/HDMIControl/HDMIControl.h"
#include <Application/AppClass/VoutUtil.h>

#include "GraphicsJNI.h"
#include "SkRect.h"

namespace android {
static JavaVM * gVM;

static jboolean realtek_server_RtkVoutUtilService_init(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkVoutUtilService_init()\n");
	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setRescaleMode(JNIEnv *env, jobject obj, jint mode) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setRescaleMode(), mode = %d\n", mode);
    VO_RECTANGLE tmpRectangle;
	VoutUtil::instance().SetRescaleMode((VO_VIDEO_PLANE)VO_VIDEO_PLANE_V1, (VO_RESCALE_MODE)mode, tmpRectangle);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setZoomOut(JNIEnv *env, jobject obj, jint onOff) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setZoomOut, onOff = %d\n", onOff);
	//VoutUtil::instance().;

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setFormat3d(JNIEnv *env, jobject obj, jint format3d, jfloat fps) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setFormat3d, format3d = %d, fps = %f\n", format3d, fps);
	VoutUtil::instance().SetFormat3d(format3d, fps);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setShiftOffset3d(JNIEnv *env, jobject obj, jboolean exchange_eyeview, jboolean shift_direction, jint delta_offset) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setShiftOffset3d, exchange_eyeview = %d, shift_direction = %d, delta_offset = %d\n", exchange_eyeview, shift_direction, delta_offset);
	VoutUtil::instance().SetShiftOffset3d(exchange_eyeview, shift_direction, delta_offset, (VO_VIDEO_PLANE)VO_VIDEO_PLANE_V1);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setShiftOffset3dByPlane(JNIEnv *env, jobject obj, jboolean exchange_eyeview, jboolean shift_direction, jint delta_offset, jint targetPlane) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setShiftOffset3dByPlane, exchange_eyeview = %d, shift_direction = %d, delta_offset = %d, targetPlane=%d\n", exchange_eyeview, shift_direction, delta_offset, targetPlane);
	VoutUtil::instance().SetShiftOffset3d(exchange_eyeview, shift_direction, delta_offset, (VO_VIDEO_PLANE)targetPlane);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_set3dto2d(JNIEnv *env, jobject obj, jint srcformat3d) {
	ALOGD("enter realtek_server_RtkVoutUtilService_set3dto2d, srcformat3d = %d\n", srcformat3d);
	VoutUtil::instance().set3Dto2D((VO_3D_MODE_TYPE)srcformat3d);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_set3dSub(JNIEnv *env, jobject obj, jint sub) {
	ALOGD("enter realtek_server_RtkVoutUtilService_set3dSub, sub = %d\n", sub);
	VoutUtil::instance().set3DSub(sub);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_applyVoutDisplayWindowSetup(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkVoutUtilService_applyVoutDisplayWindowSetup()\n");

	VO_COLOR blueBorder;

	blueBorder.c1 = 0;
	blueBorder.c2 = 0;
	blueBorder.c3 = 255;
	blueBorder.isRGB = 1;

	VoutUtil::instance().ApplyVoutDisplayWindowSetup(blueBorder);

	return true;
}



static jboolean realtek_server_RtkVoutUtilService_setOSDWindow(JNIEnv *env, jobject obj, jobject osdWin) {
    SkIRect ir;
    GraphicsJNI::jrect_to_irect(env, osdWin, &ir);
    VO_RECTANGLE rect_osd;
    rect_osd.x = ir.left();
    rect_osd.y = ir.top();
    rect_osd.width = ir.width();
    rect_osd.height = ir.height();
    VO_COLOR borderColor;
    if (S_OK != VoutUtil::instance().ConfigureDisplayWindow((VO_VIDEO_PLANE)VO_VIDEO_PLANE_OSD1, rect_osd, rect_osd, borderColor, false))
        return false;
    return true;
}

static jboolean realtek_server_RtkVoutUtilService_setZoomRect(JNIEnv *env, jobject obj, jobject zoomRect) {
    SkIRect ir;
    GraphicsJNI::jrect_to_irect(env, zoomRect, &ir);
    VO_RECTANGLE rect_zoom;
    rect_zoom.x = ir.left();
    rect_zoom.y = ir.top();
    rect_zoom.width = ir.width();
    rect_zoom.height = ir.height();
    if (S_OK != VoutUtil::instance().Zoom(VO_VIDEO_PLANE_V1, rect_zoom)) {
        return false;
    }
    return true;
}

static jboolean realtek_server_RtkVoutUtilService_setDisplayRatio(JNIEnv *env, jobject obj, jint ratio) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setDisplayRatio, ratio = %d\n", ratio);
	VoutUtil::instance().SetDisplayRatio(ratio);

	return true;
}

static jboolean realtek_server_RtkVoutUtilService_setDisplayPosition(JNIEnv *env, jobject obj, jint x, jint y, jint width, jint height) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setDisplayPosition, x = %d, y = %d, width = %d, height = %d\n", x, y, width, height);
	VoutUtil::instance().SetDisplayPosition(x, y, width, height);

	return true;
}

static jobject realtek_server_RtkVoutUtilService_getHWCV1Rect(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkVoutUtilService_getHWCV1Rect\n");

	VO_RECTANGLE V1Rect;
	VoutUtil::instance().getHWCV1Rect(&V1Rect);

	jclass clsRect = env->FindClass("android/graphics/Rect");
	jmethodID constructorRect = env->GetMethodID(clsRect, "<init>", "()V");

	jobject objRect = env->NewObject(clsRect, constructorRect, "");

	// get the field of RtkTVSystem
	jfieldID rectLeft = env->GetFieldID(clsRect, "left", "I");
	jfieldID rectTop = env->GetFieldID(clsRect, "top", "I");
	jfieldID rectRight = env->GetFieldID(clsRect, "right", "I");
	jfieldID rectBottom = env->GetFieldID(clsRect, "bottom", "I");

	env->SetIntField(objRect, rectLeft, V1Rect.x);
	env->SetIntField(objRect, rectTop, V1Rect.y);
	env->SetIntField(objRect, rectRight, V1Rect.x+V1Rect.width);
	env->SetIntField(objRect, rectBottom, V1Rect.y+V1Rect.height);

	return objRect;
}

/* ENUM_SDRFLAG
 *     AUTOSDR = 0
 *     FORCESDR_ON = 1
 *     FORCESDR_OFF = 2
 */
static void realtek_server_RtkVoutUtilService_setEnhancedSDR(JNIEnv *env, jobject obj, jint flag) {
	ALOGD("enter realtek_server_RtkVoutUtilService_setEnhancedSDR, flag=%d\n", flag);

    VoutUtil::instance().SetEnhancedSDR((ENUM_SDRFLAG)flag);
}
static jboolean realtek_server_RtkVoutUtilService_isHDRtv(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkVoutUtilService_isHDRtv\n");

    VIDEO_RPC_VOUT_QUERYTVCAP tvCap;
    VoutUtil::instance().QueryTVCapability(&tvCap);
    ALOGD("isHDRtv()=%d\n",tvCap.isHDRtv);
    if (tvCap.isHDRtv)
        return true;
    return false;
}
static void realtek_server_RtkVoutUtilService_setHdmiRange(JNIEnv *env, jobject obj, jint rangeMode) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setHdmiRange, rangeMode=%d\n", rangeMode);
    VoutUtil::instance().setHdmiRange(rangeMode);
}

static void realtek_server_RtkVoutUtilService_setCVBSDisplayRatio(JNIEnv *env, jobject obj, jint ratio) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setCVBSDisplayRatio, ratio=%d\n", ratio);
    VoutUtil::instance().SetCVBSDisplayRatio(ratio);
}

static void realtek_server_RtkVoutUtilService_setHDMICVBSDisplayRatio(JNIEnv *env, jobject obj, jint ratio) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setHDMICVBSDisplayRatio, ratio=%d\n", ratio);
    VoutUtil::instance().SetHDMICVBSDisplayRatio(ratio);
}

static void realtek_server_RtkVoutUtilService_setDPDisplayRatio(JNIEnv *env, jobject obj, jint ratio) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setDPDisplayRatio, ratio=%d\n", ratio);
    VoutUtil::instance().SetDPDisplayRatio(ratio);
}

static void realtek_server_RtkVoutUtilService_setDPDisplayBCHSSetting(JNIEnv *env, jobject obj, jint brightness, jint contrast, jint hue, jint saturation) {

	
	VIDEO_RPC_VOUT_SET_LPORT_COLOR arg;
	
    ALOGD("enter realtek_server_RtkVoutUtilService_setDPDisplayBCHSSetting, brightness=%d , contrast=%d , hue=%d , saturation=%d\n", brightness,contrast,hue,saturation);


	arg.brightness = brightness;
	arg.contrast = contrast;
	arg.hue = hue;
	arg.saturation = saturation;
	
    VoutUtil::instance().SetDPDisplayBCHSSetting(arg);
}

static void realtek_server_RtkVoutUtilService_SetDPDisplayXYOffset(JNIEnv *env, jobject obj, jint x, jint y) {
    ALOGD("enter realtek_server_RtkVoutUtilService_SetDPDisplayXYOffset, x=%d y=%d\n", x, y);
    VoutUtil::instance().SetDPDisplayXYOffset(x, y);
}

static void realtek_server_RtkVoutUtilService_setHDROff(JNIEnv *env, jobject obj, jint hdr_off) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setHDROff, hdr_off=%d\n", hdr_off);
    VoutUtil::instance().SetHDROff(hdr_off);
}

static jboolean realtek_server_RtkVoutUtilService_isCVBSOn(JNIEnv *env, jobject obj) {
    ALOGD("enter realtek_server_RtkVoutUtilService_isCVBSOn\n");

    return VoutUtil::instance().isCVBSOn();
}

static void realtek_server_RtkVoutUtilService_setCVBSOff(JNIEnv *env, jobject obj, jint off) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setCVBSOff, off=%d\n", off);

    VoutUtil::instance().setCVBSOff(off);
}

static void realtek_server_RtkVoutUtilService_setEmbedSubDisplayFixed(JNIEnv *env, jobject obj, jint fixed) {
    ALOGD("enter realtek_server_RtkVoutUtilService_setEmbedSubDisplayFixed, fixed=%d\n", fixed);

    VoutUtil::instance().setEmbedSubDisplayFixed(fixed);
}

/*
 * Array of methods.
 *
 * Each entry has three fields: the name of the method,
 * the method signature, and a pointer to the native implementation.
 */
static const JNINativeMethod RtkVoutUtilServiceMethods[] = {
		/* name, signature, funcPtr */
	{"_init", "()Z", (void*) realtek_server_RtkVoutUtilService_init },
	{"_setRescaleMode", "(I)Z", (void*) realtek_server_RtkVoutUtilService_setRescaleMode },
	{"_setOSDWindow", "(Landroid/graphics/Rect;)Z", (void*) realtek_server_RtkVoutUtilService_setOSDWindow },
	{"_setZoomRect", "(Landroid/graphics/Rect;)Z", (void*) realtek_server_RtkVoutUtilService_setZoomRect },
	{"_setFormat3d", "(IF)Z", (void*) realtek_server_RtkVoutUtilService_setFormat3d },
	{"_setShiftOffset3d", "(ZZI)Z", (void*) realtek_server_RtkVoutUtilService_setShiftOffset3d },
	{"_set3dto2d", "(I)Z", (void*) realtek_server_RtkVoutUtilService_set3dto2d },
	{"_set3dSub", "(I)Z", (void*) realtek_server_RtkVoutUtilService_set3dSub },
	{"_applyVoutDisplayWindowSetup", "()Z", (void*) realtek_server_RtkVoutUtilService_applyVoutDisplayWindowSetup },
	{"_setDisplayRatio", "(I)Z", (void*) realtek_server_RtkVoutUtilService_setDisplayRatio },
	{"_setDisplayPosition", "(IIII)Z", (void*) realtek_server_RtkVoutUtilService_setDisplayPosition },
	{"_getHWCV1Rect", "()Landroid/graphics/Rect;", (void*) realtek_server_RtkVoutUtilService_getHWCV1Rect },
	{"_setEnhancedSDR", "(I)V", (void*) realtek_server_RtkVoutUtilService_setEnhancedSDR },
	{"_isHDRtv", "()Z", (void*) realtek_server_RtkVoutUtilService_isHDRtv },
	{"_setShiftOffset3dByPlane", "(ZZII)Z", (void*) realtek_server_RtkVoutUtilService_setShiftOffset3dByPlane },
    {"_setHdmiRange", "(I)V", (void*) realtek_server_RtkVoutUtilService_setHdmiRange },
    {"_setCVBSDisplayRatio", "(I)V", (void*) realtek_server_RtkVoutUtilService_setCVBSDisplayRatio },
    {"_setHDMICVBSDisplayRatio", "(I)V", (void*) realtek_server_RtkVoutUtilService_setHDMICVBSDisplayRatio },
    {"_setDPDisplayRatio", "(I)V", (void*) realtek_server_RtkVoutUtilService_setDPDisplayRatio },
    {"_setDPDisplayBCHSSetting", "(IIII)V", (void*) realtek_server_RtkVoutUtilService_setDPDisplayBCHSSetting },
    {"_SetDPDisplayXYOffset", "(II)V", (void*) realtek_server_RtkVoutUtilService_SetDPDisplayXYOffset },
    {"_setHDROff", "(I)V", (void*) realtek_server_RtkVoutUtilService_setHDROff },
    {"_isCVBSOn", "()Z", (void*) realtek_server_RtkVoutUtilService_isCVBSOn },
    {"_setCVBSOff", "(I)V", (void*) realtek_server_RtkVoutUtilService_setCVBSOff },
    {"_setEmbedSubDisplayFixed", "(I)V", (void*) realtek_server_RtkVoutUtilService_setEmbedSubDisplayFixed },
};

/*
* This is called by the VM when the shared library is first loaded.
*/
int register_android_server_RtkVoutUtilService(JNIEnv *env) {

	jclass clazz = env->FindClass("com/realtek/server/RtkVoutUtilService");

	if (clazz == NULL)
	{
		ALOGE("Can't find com/realtek/server/RtkVoutUtilService");
		return -1;
	}

	// get the VM pointer
	env->GetJavaVM(&gVM);

	if(jniRegisterNativeMethods(env, "com/realtek/server/RtkVoutUtilService",
		RtkVoutUtilServiceMethods, NELEM(RtkVoutUtilServiceMethods)) != 0) {
		ALOGD("Register method to com/realtek/server/RtkVoutUtilService failed!");
		return -1;
	}

	return 0;
}


} /* namespace android */
