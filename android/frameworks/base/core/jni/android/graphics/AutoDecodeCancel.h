#ifndef _ANDROID_GRAPHICS_AUTO_DECODE_CANCEL_H_
#define _ANDROID_GRAPHICS_AUTO_DECODE_CANCEL_H_

#include <jni.h>
#include "SkImageDecoder.h"
#include "SkPixelRef.h"

class AutoDecoderCancel {
public:
    AutoDecoderCancel(jobject options, SkImageDecoder* decoder);
    AutoDecoderCancel(jobject joptions,SkImageDecoder* decoder, SkPixelRef* pixelRef);
    ~AutoDecoderCancel();

    static bool RequestCancel(jobject options);
    static bool RequestCancel(JNIEnv* env, jobject joptions);
    static bool SetPixelRef(jobject joptions, SkPixelRef* pixelRef);
    static bool SetPixelRef(JNIEnv* env, jobject joptions, SkPixelRef* pixelRef);

private:
    AutoDecoderCancel*  fNext;
    AutoDecoderCancel*  fPrev;
    jobject             fJOptions;  // java options object
    SkImageDecoder*     fDecoder;
    SkPixelRef*         fPixelRef;

#ifdef SK_DEBUG
    static void Validate();
#else
    static void Validate() {}
#endif
};

#endif  // _ANDROID_GRAPHICS_AUTO_DECODE_CANCEL_H_
