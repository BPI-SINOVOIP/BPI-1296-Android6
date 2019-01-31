/*
 * Copyright (c) 2014 Realtek Semiconductor Corp.
 */
#ifndef _RECORDBUFFERSERVICE_H_
#define _RECORDBUFFERSERVICE_H_

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/Vector.h>
#include <RtkIpc/IRecordBuffer.h>

//#define RECORD_DATA_SIZE    188*1024
//#define RECORD_DATA_SIZE    (1500 + 4)* 200
// Current 720P encoder bitrate is 700~ 800k , shared memory need store 1 sec, almost 1MB
#define RECORD_DATA_SIZE    (1500 + 4)* 800 // Mark me wait encoder bitrate control done
#define RECORD_INFO_SIZE    32
#define MEMORY_SIZE (RECORD_DATA_SIZE + RECORD_INFO_SIZE)
#define MAX_NUM_INSTANCE    5
#define DEFAULT_NUM_INSTANCE    2

namespace android {

typedef struct RecordBuffer {
    sp<MemoryHeapBase> mMemHeap;
    sp<MemoryBase> mBufferData;
    sp<MemoryBase> mBufferInfo;
} RecordBuffer;

class RecordBufferService : public BnRecordBuffer {
public:
  static void instantiate(int numInstance = DEFAULT_NUM_INSTANCE);

  RecordBufferService(int numInstance);
  virtual ~RecordBufferService();
  virtual sp<IMemory> getBuffer(int id, int handle);
  virtual bool isAlive();

  virtual void registerInterface(int idx, const sp<IBinder>& iface){ if(idx >= 0 && idx < IFACE_MAX_NUM) mIfaces[idx] = iface; }
  virtual sp<IBinder> retrieveInterface(int idx){ return (idx >= 0 && idx < IFACE_MAX_NUM) ? mIfaces[idx] : NULL; }

private:
  int mNumInstance;
  Vector<RecordBuffer> mRecordBuffers;

  sp<IBinder> mIfaces[IFACE_MAX_NUM];
};


} // namespace android

#endif //_RECORDBUFFERSERVICE_H_
