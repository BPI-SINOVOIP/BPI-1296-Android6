#ifndef __RTK_HWC_HWLAYER_VIDEO_LAYER_H_
#define __RTK_HWC_HWLAYER_VIDEO_LAYER_H_
#include <utils/LayerBase.h>
#include <utils/PLockBase.h>
#include <utils/HDCPMessage.h>
#include "VideoBuffer.h"
#include "CombinedManager.h"

class VideoLayer : 
    public  LayerBase,
    virtual GraphicBufferInfo::Client
{
public:
  using LayerBase::connect;
    VideoLayer();
    virtual ~VideoLayer();

    virtual int                 connect(enum VO_VIDEO_PLANE plane, int BufferCnt);
    virtual int                 disconnect();
    virtual BufferBase *        createBuffer();
    virtual void                removeBuffer(BufferBase * buffer);
    virtual enum VO_VIDEO_PLANE getPlane() {return mPlane;};
    virtual int                 validate(buffer_handle_t handle);
    virtual int                 getMaxBufferCount() {return mBufferCount;};
    virtual void                dump(android::String8& buf, const char* prefix) {return LayerBase::dump(buf, prefix);};
    virtual int                 getBufferWidth() {return mBufferWidth;};
    virtual int                 getBufferHeight() {return mBufferHeight;};
    virtual bool                getHDCPInfo();
private:
    PLockBase * mPLock;
    CombinedManager *   mCombinedManager;
    HDCPMessage *mHDCPMessage;
    GraphicBufferInfo * mGBInfo;
    int mBufferCount;
    enum VO_VIDEO_PLANE mPlane;
    int mBufferWidth;
    int mBufferHeight;
};
#endif /* End of __RTK_HWC_HWLAYER_VIDEO_LAYER_H_ */
