#ifndef _SIDEBAND_BUFFER_
#define _SIDEBAND_BUFFER_

#include <gui/Surface.h>
#include <android/native_window.h>

using namespace android;

// structure to keep buffer info, init by HdmiHwSideband,
// refered by both HdmiHwSideband and V4L2DeviceSideband
typedef struct buf_info_t {
    ANativeWindowBuffer *pWinBuf;
    void *vAddr;
    int w;
    int h;
    unsigned int size;
    int index;
    bool bDequeued;
    // more..
} BufInfo;

#endif

