/*************************************************************************
	> File Name: RtkAudioStore.h
	> Author: keyon_li
	> Mail: keyon_li@realsil.com.cn 
	> Created Time: Fri 22 Sep 2017 02:38:33 PM CST
 ************************************************************************/
#ifndef _AUDIOSTORE_H
#define _AUDIOSTORE_H

#include "RtkAudioCommon.h"
namespace android {
class RtkAudioStore : public AHandler
{
public:
    RtkAudioStore();
	
	enum {
	kWhatQueueBuffer = 0x4000,
	kWhatDequeBuffer,
	kWhatFlush
	};
	
protected:
    virtual ~RtkAudioStore();
    virtual void onMessageReceived(const sp <AMessage> &msg);

private:
    List< sp<ABuffer> > mAudioQueueForUser;
    Mutex mAQueueLockForUser;
	
	DISALLOW_EVIL_CONSTRUCTORS(RtkAudioStore);
};

}
#endif //_AUDIORENDER_H
