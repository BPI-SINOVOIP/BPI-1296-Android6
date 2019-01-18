/*
 * IRecordChallenger.h
 */
#ifndef __IRECORDCHALLENGER_H__
#define __IRECORDCHALLENGER_H__

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

class IRecordChallenger: public IInterface
{
public:
  DECLARE_META_INTERFACE(RecordChallenger);
  virtual void challenge(sp<IMemory>& in, sp<IMemory>& out) = 0;
};

/* --- Server side --- */

class BnRecordChallenger: public BnInterface<IRecordChallenger>
{
public:
  virtual status_t    onTransact( uint32_t code,
                                  const Parcel& data,
                                  Parcel* reply,
                                  uint32_t flags = 0);
};

}; // namespace android

#endif //__IRECORDCHALLENGER_H__
