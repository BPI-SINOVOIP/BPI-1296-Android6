/*
 * Copyright (c) 2014 Realtek Semiconductor Corp.
 */
#ifndef _CAMERAREADER_H_
#define _CAMERAREADER_H_

#include <utils/Log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "RtkTranscodeCommon.h"

namespace android {

struct ALooper;
struct AHandler;
class Statistics;
class Camera;
class CameraParameters;

class CameraReader : public RtkFilter
{
public:
    CameraReader();

    enum {
        /* Player --> CameraReader */
        kWhatSetCameraSource = 0x700,
        kWhatSetCameraSource2,
        kWhatConfigure,
        kWhatPrepare,
        kWhatSetPlayback,
        kWhatSetEncode,
        kWhatStart,
        kWhatGetSnapshot,
        kWhatGetCameraParam,
        kWhatConfigurePreview,
        kWhatStop = WHAT_STOP,
        kWhatSetSurface = WHAT_SET_SURFACE,

        /* Agent --> CameraReader */
        kWhatDrainBufferDone = 0x710,
    };

    status_t setPlayerHandler(AHandler *pHandler) {
        mPlayerHandler = pHandler;
        return OK;
    }

protected:
    virtual ~CameraReader();
    virtual void onMessageReceived(const sp<AMessage> &msg);

    // Callback called when still camera raw data is available.
    virtual void dataCallback(int32_t msgType, const sp<IMemory> &data);
    virtual void dataCallbackTimestamp(int64_t timestampUs, int32_t msgType, const sp<IMemory> &data);

private:
    friend class CameraCallbackHandler;

    enum State {
        UNINITIALIZED,
        WAIT_SURFACE,
        INITIALIZED,
        CONFIGURED,
        PREPARED,
        STARTED,
        STOPPED,
    };

    State mState;

    bool mPreviewCamera;
    bool mEncodeCamera;
    bool mHdmiRx;

    String16 mClientName;
    int mCameraId;
    int mSrcHdmiRx;
    int mSrcReady;
    int mSrcProgressive;
    int mSrcColor;
    int mSrcWidth;
    int mSrcHeight;
    int mSrcPreviewWidth;
    int mSrcPreviewHeight;
    int mSrcFps;
    unsigned int mFrameCount;
    unsigned int mEncodeCount;
    int mTargetWidth;
    int mTargetHeight;
    int mTargetFps;
    sp<Camera> mCamera;
    sp<Surface> mSurface;
    CameraParameters mParameters;
    sp<IMemory> mSnapshotBuffer;
    pthread_mutex_t mSnapMtx;
    pthread_cond_t mSnapCnd;
    CAMERA_SNAPSHOT mSnapParam;
    Statistics mDropStat;
    sp<AHandler> mPlayerHandler;

    status_t setPreviewStatus(bool preview);
    status_t setPlayback(int previewCamera);
    status_t setEncode(int encodeCamera);
    status_t start();
    status_t stop();
    status_t prepare();
    status_t getSnapshot(const sp<AMessage> &msg, sp<AMessage> &response);
    status_t retrieveHdmiRxVideoInfo();

    sp<IBinder> mRecBufSvc;

    DISALLOW_EVIL_CONSTRUCTORS(CameraReader);
};

} // namespace android

#endif //_CAMERAREADER_H_
