/*
 * Copyright (c) 2017 Realtek Semiconductor Corp.
 */
#ifndef _RTK_CAMERA_MEDIA_CODEC_H_
#define _RTK_CAMERA_MEDIA_CODEC_H_

#include <utils/Vector.h>
#include <utils/threads.h>
#include <utils/Mutex.h>
#include <media/stagefright/MediaCodec.h>

#define CAM_BUFFER_NUM 8 // keep in sync with NB_BUFFER defined in CameraHardware.h

using namespace android;

class CameraMediaCodec : public AHandler {
public:
    enum {
        /* Camerahardware --> CameraMediaCodec */
        kWhatStop = 0x200,
        kWhatStart,
        kWhatDebugBuffers,
        //kWhatSetSurface,
        //kWhatCreateDecoder,

        kWhatImportBuffer = 0x210,

        kWhatDecodeBuffer = 0x220,                /* CamearHardware send buffer to decode */
        kWhatGetBuffer,                           /* CamearHardware get buffer from decode */
        kWhatNoMoreData,                          /* CameraHardware want do stopPreview */

        /* MediaCodec --> CameraMediaCodec */
        kWhatCodecNotify = 0x230,                /* Notification from MediaCodec */
    };

    enum {
        CAM_FMT_MJPG,
        CAM_FMT_H264,
    };

    typedef struct _CameraBuffer {
        int32_t             mIndex;
        size_t              mSize;
        void*               mVirtAddr;
        int32_t             mPhysAddr;
        nsecs_t             mTimestamp;
        buffer_handle_t*    mHandle;

        // Default constructor
        _CameraBuffer()
            :mIndex(0),
             mSize(0),
             mVirtAddr(NULL),
             mPhysAddr(0),
             mTimestamp(0),
             mHandle(NULL)
        {}
    } CameraBuffer;

    CameraMediaCodec(int32_t pixFmt, int32_t camFmt, int32_t width, int32_t height, int32_t bufSize);
    bool CheckDecodeOK();
    bool CopyDecodedData(int32_t index, int64_t &timestamp, int32_t &free_buffers);

protected:
    virtual ~CameraMediaCodec();
    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    template <typename T>
    class BufferQueue {
    public:
        BufferQueue(size_t capacity, char *name);
        ~BufferQueue();

        void            EnqueueBuf(T buf);
        T               DequeueBuf();
        size_t          GetSize();
    private:
        size_t          mCapacity;
        Mutex           mLock;
        List<T>         mQueue;
        char *          mName;
    }; // end class BufferQueue

    class DecodeThread : public Thread {
    public:
        DecodeThread(CameraMediaCodec *parent);
        status_t                start();
        status_t                stop();
    protected:
        virtual                 ~DecodeThread();
        void                    siganl();
    private:
        virtual bool            threadLoop();
        CameraMediaCodec*       mParent;
        Condition               mSignal;
        bool                    mRunning;
    }; // end class DecodeThread

    status_t                        onStart();
    status_t                        onStop();
    status_t                        CreateDecoder();
    status_t                        onCameraGetDecodeBuf(int idx, CameraBuffer &buf);
    status_t                        ImportBuffer(int idx, CameraBuffer buf, bool intermediate);
    void                            DumpBuffer(void *buf, int size, char path[]);
    void                            DebugBuffersStatus(bool intermediate);

    CameraBuffer                    mCameraBuffers[CAM_BUFFER_NUM];
    CameraBuffer                    mIntermediateBuffers[CAM_BUFFER_NUM];
    BufferQueue<CameraBuffer *> *   mInputQueue;        // Camera buffer, from V4L2, store index
    BufferQueue<int> *              mDecInputQueue;     // MediaCodec decode input buffer, store index
    BufferQueue<int> *              mDecOutputQueue;    // MediaCodec decode output buffer, store index
    BufferQueue<int> *              mUnusedQueue;       // Camera buffers which are already been copied to mediacodec

    sp<ALooper>                     mLooper;
    sp<MediaCodec>                  mCodec;
    sp<DecodeThread>                mDecodeThread;

    int32_t                         mPixFmt;
    int32_t                         mCamFmt;
    int32_t                         mWidth;
    int32_t                         mHeight;
    int32_t                         mBufSize;
    nsecs_t                         mLastDecodedPts;

}; // end class CameraMediaCodec

#endif // end of ndef _RTK_CAMERA_MEDIA_CODEC_H_
