#include "IonBuffer.h"

IonBuffer::IonBuffer(enum IonOper oper, long long oper_data, unsigned int heap_mask, unsigned int flag_mask)
    : mIonFd(-1),
    mIonMmapFd(-1),
    mViraddr(NULL),
    mPhyaddr(NULL),
    mSize(0),
    mIonHandle(0),
    mHeapMask(heap_mask),
    mFlagMask(flag_mask)
{
    switch (oper) {
        case AllocBuffer:
            allocBuffer(oper_data);
            break;
        case ImportBuffer:
            importBuffer((int)oper_data);
            break;

    };
}

IonBuffer::~IonBuffer()
{
    freeBuffer();
}

int IonBuffer::sync() {
    int ret = -1;
    if (mIonFd != -1 && mIonMmapFd != -1)
        ret = ion_sync_fd(mIonFd, mIonMmapFd);
    else
        ALOGE("[%s] mIonFd=%d mIonMmapFd=%d", __func__, mIonFd, mIonMmapFd);
    return ret;
}

void IonBuffer::allocBuffer(long long bufferSize) {
    int ret = -1;
    unsigned int ion_heap_mask = 0;
    unsigned int ion_heap_flag = 0;

    if (mSize > 0) {
        ALOGW("[%s] bufferSize=%lld mSize=%lld", __func__, bufferSize, mSize);
        freeBuffer();
    }

    ion_heap_mask |= (mHeapMask & MediaHeapMask )?  RTK_PHOENIX_ION_HEAP_MEDIA_MASK :0;
    ion_heap_mask |= (mHeapMask & AudioHeapMask )?  RTK_PHOENIX_ION_HEAP_AUDIO_MASK :0;

    ion_heap_flag |= (mFlagMask & NONCACHED )?  ION_FLAG_NONCACHED  :0;
    ion_heap_flag |= (mFlagMask & SCPUACC   )?  ION_FLAG_SCPUACC    :0;
    ion_heap_flag |= (mFlagMask & ACPUACC   )?  ION_FLAG_ACPUACC    :0;
    ion_heap_flag |= (mFlagMask & HWIPACC   )?  ION_FLAG_HWIPACC    :0;

    mIonFd = ion_open();

    if (mIonFd >= 0)
        ret = ion_alloc(mIonFd, bufferSize,
                0, /* align */
                ion_heap_mask,
                ion_heap_flag,
                &mIonHandle);

    if (ret == 0) {
        ion_share(mIonFd, mIonHandle, &mIonMmapFd);
        ion_phys(mIonFd, mIonHandle, (unsigned long *)&mPhyaddr, (unsigned int *)&mSize);
        mViraddr = mmap(NULL, mSize, PROT_READ | PROT_WRITE, MAP_SHARED, mIonMmapFd, 0);
    } else {
        ALOGE("[%s] ion alloc failed! heap=0x%x flag=0x%x size=%lld IonFd=%d (ret=%d)", __func__,
                ion_heap_mask, ion_heap_flag, bufferSize, mIonFd, ret);

        if (mIonFd != -1) {
            ion_close(mIonFd);
            mIonFd = -1;
        }
    }

}

void IonBuffer::importBuffer(int import_fd) {
    int ret = -1;

    if (mSize > 0) {
        ALOGW("[%s] import_fd=%d mSize=%lld", __func__, import_fd, mSize);
        freeBuffer();
    }

    mIonFd = ion_open();

    if (mIonFd >= 0)
        ret = ion_import(mIonFd, import_fd, &mIonHandle);

    if (ret == 0) {
        ion_share(mIonFd, mIonHandle, &mIonMmapFd);
        ion_phys(mIonFd, mIonHandle, (unsigned long *)&mPhyaddr, (unsigned int *)&mSize);
        mViraddr = mmap(NULL, mSize, PROT_READ | PROT_WRITE, MAP_SHARED, mIonMmapFd, 0);
    } else {
        ALOGE("[%s] ion import failed! IonFd=%d import_fd=%d (ret=%d)", __func__,
                mIonFd, import_fd, ret);

        if (mIonFd != -1) {
            ion_close(mIonFd);
            mIonFd = -1;
        }
    }

}

void IonBuffer::freeBuffer() {
    if (mSize > 0) {
        close(mIonMmapFd);
        munmap(mViraddr, mSize);
        ion_free(mIonFd, mIonHandle);
        ion_close(mIonFd);
        mIonFd = -1;
        mSize = 0;
    }
}
