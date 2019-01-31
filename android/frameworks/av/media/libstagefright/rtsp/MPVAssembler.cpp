/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "MPVAssembler"
#include <utils/Log.h>

#include "MPVAssembler.h"

#include "ARTPSource.h"
#include "ASessionDescription.h"

#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/Utils.h>

namespace android {

MPVAssembler::MPVAssembler(
        const sp<AMessage> &notify,
        const char * /* desc */,
        const AString & /* params */)
    : mNotifyMsg(notify),
      mNextExpectedSeqNoValid(false),
      mNextExpectedSeqNo(0) {
          mCurSeqNo = -1;
}

MPVAssembler::~MPVAssembler() {
}

ARTPAssembler::AssemblyStatus MPVAssembler::assembleMore(
        const sp<ARTPSource> &source) {
    return addPacket(source);
}

int32_t MPVAssembler::getFourBytetoInt(uint8_t *buf)
{
    int32_t dSeqNo = 0;

    dSeqNo += buf[0] << 24;
    dSeqNo += buf[1] << 16;
    dSeqNo += buf[2] << 8;
    dSeqNo += buf[3] ;

    return dSeqNo;
}

ARTPAssembler::AssemblyStatus MPVAssembler::addPacket(
        const sp<ARTPSource> &source) {
    List<sp<ABuffer> > *queue = source->queue();

    if (queue->empty()) {
        return NOT_ENOUGH_DATA;
    }

    if (mNextExpectedSeqNoValid) {
        List<sp<ABuffer> >::iterator it = queue->begin();
        while (it != queue->end()) {
            if ((uint32_t)(*it)->int32Data() >= mNextExpectedSeqNo) {
                break;
            }

            it = queue->erase(it);
        }

        if (queue->empty()) {
            return NOT_ENOUGH_DATA;
        }
    }

    sp<ABuffer> buffer = *queue->begin();

    if (!mNextExpectedSeqNoValid) {
        mNextExpectedSeqNoValid = true;
        mNextExpectedSeqNo = (uint32_t)buffer->int32Data();
    }
    sp<ABuffer> fmBuffer;

    int32_t dSeqNo = 0;
    List<sp<ABuffer> >::iterator it = queue->begin();
    int32_t dSize = queue->size();
    while (dSize > 1)
    {
        ++it;
        dSize--;
    }
    uint8_t *buf =(*it)->data();
    dSeqNo = getFourBytetoInt(buf);

    if(mCurSeqNo == -1 )
    {
        mCurSeqNo = dSeqNo;
    }

    if(mCurSeqNo == dSeqNo)
        return NOT_ENOUGH_DATA;

    //mCurSeqNo != dSeqNo case
    ALOGV("[%s][%d] need merge buffer\n",__FUNCTION__,__LINE__);

    it = queue->begin();
    dSize = queue->size();
    int CurIdx = 0;

    int bufSize = 0;
    //Calculate buffer size
    while (dSize > 0)
    {
        buf =(*it)->data();
        dSeqNo = getFourBytetoInt(buf);
        //ALOGI("[%s][%d] dSeqNo :: %d mCurSeqNo::  %d \n",__FUNCTION__,__LINE__, dSeqNo, mCurSeqNo);

        bufSize += (*it)->size();
        if(mCurSeqNo != dSeqNo)
            break;
        ++it;
        dSize--;
        CurIdx++;
    }

    if(mCurSeqNo != dSeqNo)
    {
        ALOGV("[%s][%d] dSeqNo :: %d mCurSeqNo:: %d.\n",__FUNCTION__,__LINE__, dSeqNo, mCurSeqNo);
        ALOGV("[%s][%d] dSize::%d CurIdx::%d, queue->size()::%d, bufSize::%d\n",__FUNCTION__,__LINE__,dSize, CurIdx, (int)queue->size(), bufSize);

        //set bufer
        fmBuffer = new ABuffer(bufSize);
        fmBuffer->setRange(0, 0);
        it = queue->begin();
        int i = 0 ;
        int32_t rtpTime = 0;
        for(i = 0 ; i < CurIdx;i++)
        {
            memcpy(fmBuffer->data() + fmBuffer->size(), (*it)->data() + 4 , (*it)->size() - 4);
            fmBuffer->setRange(0, fmBuffer->size() + (*it)->size() - 4);

            if(i == 0)
                CHECK((*it)->meta()->findInt32("rtp-time", (int32_t *)&rtpTime));
            it = queue->erase(it);

        }
        //after check dSeqNo;
        buf =(*it)->data();
        dSeqNo = getFourBytetoInt(buf);
        ALOGV("[%s][%d] dSeqNo :: %d mCurSeqNo::  %d \n",__FUNCTION__,__LINE__, dSeqNo, mCurSeqNo);
        mCurSeqNo = dSeqNo;

        sp<AMessage> msg = mNotifyMsg->dup();
        fmBuffer->meta()->setInt32("rtp-time",rtpTime);
        msg->setBuffer("access-unit", fmBuffer);
        msg->post();
        ++mNextExpectedSeqNo;
        return OK;
    }

    return OK;
}

void MPVAssembler::packetLost() {
    CHECK(mNextExpectedSeqNoValid);
    ++mNextExpectedSeqNo;
}

void MPVAssembler::onByeReceived() {
    sp<AMessage> msg = mNotifyMsg->dup();
    msg->setInt32("eos", true);
    msg->post();
}

}  // namespace android


