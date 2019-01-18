
#include "VOLayer.h"
namespace android {

VOLayer::VOLayer()
    : mFlags(FIRST_RENDER),
    mLayerType(LAYER_NULL),
    mBufferCnt(8),
    mAudioHwSync(-1)
{
    mHWLayer = new VideoLayer();
    mWindow = new VOWindowRPC();
}

VOLayer::~VOLayer() {
    mWindow->setFlags(HIDDEN|REFRESH,HIDDEN|REFRESH);
    mHWLayer->disconnect();
    mHWLayer.clear();
    mWindow.clear();
    if (mAudioHwSync >= 0)
        close(mAudioHwSync);
}

status_t VOLayer::setLayerType(enum layer_type type) {
    if (mFlags & FIRST_RENDER) {
        mLayerType = type;
    }
    return NO_ERROR;
}

status_t VOLayer::setBufferCount(int BufferCnt) {
    if (mFlags & FIRST_RENDER) {
        mBufferCnt = BufferCnt;
    }
    return NO_ERROR;
}

status_t VOLayer::render(buffer_handle_t handle, int *pFenceFd) {
    Mutex::Autolock l(mLock);
    int ret;
    if (mFlags & FIRST_RENDER) {
        ret = mHWLayer->connect((enum VO_VIDEO_PLANE) mLayerType, mBufferCnt, mAudioHwSync);
        if (ret)
            return status_t(ret);
        mWindow->setPlane((enum VO_VIDEO_PLANE) mLayerType);
        mFlags &= ~FIRST_RENDER;
        mWindow->setFlags(VOWindowRPC::FRAME_READY,VOWindowRPC::FRAME_READY);
    }

    ret = mHWLayer->render(handle, pFenceFd);
    return status_t(ret);
}

status_t VOLayer::validate(buffer_handle_t handle) {
    int ret = mHWLayer->validate(handle);
    return status_t(ret);
}

void VOLayer::dump(String8& result, const char* prefix) const {
    String8 mPrefix(prefix);
    result.appendFormat("%s", mPrefix.string());
    result.appendFormat("VOLayer %p mFlags:0x%x mLayerType:%d mBufferCnt:%d mAudioHwSync:%d\n",
            this, mFlags, mLayerType, mBufferCnt, mAudioHwSync);

    mPrefix.append("    ");

    if (mWindow != NULL)
        mWindow->dump(result, mPrefix.string());

    if (mHWLayer != NULL)
        mHWLayer->dump(result, mPrefix.string());
}

status_t VOLayer::freeze(bool enable) {
    Mutex::Autolock l(mLock);
    if (mFlags & FIRST_RENDER) return status_t(-1);
    int ret = mHWLayer->freeze(enable);
    return status_t(ret);
}

status_t VOLayer::setAudioHwSync(int audioHwSync) {
    if (mAudioHwSync > 0) {
        if (mHWLayer->isConnect()) {
            close(audioHwSync);
        } else {
            close(mAudioHwSync);
            mAudioHwSync = audioHwSync;
        }
    } else {
        mAudioHwSync = audioHwSync;
    }
    return NO_ERROR;
};

status_t VOLayer::setCrop(const Rect& crop) {
    int ret = mWindow->setCrop(crop.left, crop.top, crop.right, crop.bottom);
    return status_t(ret);
}

status_t VOLayer::setPosition(float x, float y) {
    int ret = mWindow->setPosition(x, y);
    return status_t(ret);
}

status_t VOLayer::setSize(uint32_t w, uint32_t h) {
    int ret = mWindow->setSize(w, h);
    return status_t(ret);
}

status_t VOLayer::setLayer(int32_t z) {
    int ret = mWindow->setLayer(z);
    return status_t(ret);
}

status_t VOLayer::setFlags(uint32_t flags, uint32_t mask) {
    int ret = mWindow->setFlags(flags, mask);
    return status_t(ret);
}

status_t VOLayer::setAlpha(float alpha) {
    int ret = mWindow->setAlpha(alpha);
    return status_t(ret);
}

bool VOLayer::getHDCPInfo(void) {
    return mHWLayer->getHDCPInfo();
}

}; // namespace android
