#ifndef __RTK_HWC_HWLAYER_UTILS_ION_BUFFER_INFO_H_
#define __RTK_HWC_HWLAYER_UTILS_ION_BUFFER_INFO_H_
#include "hwlayer_utils.h"
#include <ion.h>

class IonBuffer {
public:
    enum IonOper {
        AllocBuffer,
        ImportBuffer
    };

    enum IonHeapMask {
        MediaHeapMask = 0x1U << 0,
        AudioHeapMask = 0x1U << 1,
    };

    enum IonFlagMask {
        NONCACHED       = 0x1U << 0,
        SCPUACC         = 0x1U << 1,
        ACPUACC         = 0x1U << 2,
        HWIPACC         = 0x1U << 3,
    };

    IonBuffer(enum IonOper oper, long long oper_data,
            unsigned int heap_mask = MediaHeapMask,
            unsigned int flag_mask = (NONCACHED|SCPUACC|HWIPACC));

    virtual ~IonBuffer();
    void * getVirAddr() {return mViraddr;};
    void * getPhyAddr() {return mPhyaddr;};
    int sync();
    long long getSize() {return mSize;};

private:
    virtual void allocBuffer(long long bufferSize);
    virtual void importBuffer(int import_fd);
    virtual void freeBuffer();

    int     mIonFd;
    int     mIonMmapFd;
    void *  mViraddr;
    void *  mPhyaddr;
    long long mSize;
    ion_user_handle_t mIonHandle;
    unsigned int mHeapMask;
    unsigned int mFlagMask;
};

#endif /* End of __RTK_HWC_HWLAYER_UTILS_ION_BUFFER_INFO_H_ */
