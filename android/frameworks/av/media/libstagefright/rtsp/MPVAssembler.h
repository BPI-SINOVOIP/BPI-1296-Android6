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

#ifndef A_MPV_ASSEMBLER_H_

#define A_MPV_ASSEMBLER_H_

#include "ARTPAssembler.h"

namespace android {

struct AMessage;
struct AString;
class MetaData;

struct MPVAssembler : public ARTPAssembler {
    MPVAssembler(
            const sp<AMessage> &notify,
            const char *desc, const AString &params);

protected:
    virtual ~MPVAssembler();

    virtual AssemblyStatus assembleMore(const sp<ARTPSource> &source);
    virtual void onByeReceived();
    virtual void packetLost();

private:
    sp<AMessage> mNotifyMsg;
    bool mNextExpectedSeqNoValid;
    uint32_t mNextExpectedSeqNo;
    int32_t mCurSeqNo;

    AssemblyStatus addPacket(const sp<ARTPSource> &source);
    int32_t getFourBytetoInt(uint8_t *buf);

    DISALLOW_EVIL_CONSTRUCTORS(MPVAssembler);
};

}  // namespace android

#endif  // A_MPV_ASSEMBLER_H_

