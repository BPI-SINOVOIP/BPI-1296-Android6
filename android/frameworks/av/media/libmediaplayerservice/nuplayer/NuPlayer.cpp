/*
 * Copyright (C) 2010 The Android Open Source Project
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
#define LOG_TAG "NuPlayer"
#include <utils/Log.h>

#include "NuPlayer.h"

#include "HTTPLiveSource.h"
#include "NuPlayerCCDecoder.h"
#include "NuPlayerDecoder.h"
#include "NuPlayerDecoderBase.h"
#include "NuPlayerDecoderPassThrough.h"
#include "NuPlayerDriver.h"
#include "NuPlayerRenderer.h"
#include "NuPlayerSource.h"
#include "RTSPSource.h"
#include "StreamingSource.h"
#include "GenericSource.h"
#include "TextDescriptions.h"

#include "ATSParser.h"

#include <cutils/properties.h>

#include <media/AudioResamplerPublic.h>
#include <media/AVSyncSettings.h>

#include <media/IMediaHTTPConnection.h>
#include <media/IMediaHTTPService.h>
#include <media/IMediaPlayerService.h>

#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaHTTP.h>
#include <media/stagefright/MetaData.h>

#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>

#include "avc_utils.h"

#include "ESDS.h"
#include <media/stagefright/Utils.h>

#include "HTTPBase.h"
#include "audio_table.h"
#include "video_table.h"
#include <inttypes.h>
#ifdef RTK_PLATFORM
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/resource.h>

#endif

namespace android {

struct NuPlayer::Action : public RefBase {
    Action() {}

    virtual void execute(NuPlayer *player) = 0;

private:
    DISALLOW_EVIL_CONSTRUCTORS(Action);
};

struct NuPlayer::SeekAction : public Action {
    SeekAction(int64_t seekTimeUs)
        : mSeekTimeUs(seekTimeUs) {
    }

    virtual void execute(NuPlayer *player) {
        player->performSeek(mSeekTimeUs);
    }

private:
    int64_t mSeekTimeUs;

    DISALLOW_EVIL_CONSTRUCTORS(SeekAction);
};

struct NuPlayer::ResumeDecoderAction : public Action {
    ResumeDecoderAction(bool needNotify)
        : mNeedNotify(needNotify) {
    }

    virtual void execute(NuPlayer *player) {
        player->performResumeDecoders(mNeedNotify);
    }

private:
    bool mNeedNotify;

    DISALLOW_EVIL_CONSTRUCTORS(ResumeDecoderAction);
};

struct NuPlayer::SetSurfaceAction : public Action {
    SetSurfaceAction(const sp<Surface> &surface)
        : mSurface(surface) {
    }

    virtual void execute(NuPlayer *player) {
        player->performSetSurface(mSurface);
    }

private:
    sp<Surface> mSurface;

    DISALLOW_EVIL_CONSTRUCTORS(SetSurfaceAction);
};

struct NuPlayer::FlushDecoderAction : public Action {
    FlushDecoderAction(FlushCommand audio, FlushCommand video)
        : mAudio(audio),
          mVideo(video) {
    }

    virtual void execute(NuPlayer *player) {
        player->performDecoderFlush(mAudio, mVideo);
    }

private:
    FlushCommand mAudio;
    FlushCommand mVideo;

    DISALLOW_EVIL_CONSTRUCTORS(FlushDecoderAction);
};

struct NuPlayer::PostMessageAction : public Action {
    PostMessageAction(const sp<AMessage> &msg)
        : mMessage(msg) {
    }

    virtual void execute(NuPlayer *) {
        mMessage->post();
    }

private:
    sp<AMessage> mMessage;

    DISALLOW_EVIL_CONSTRUCTORS(PostMessageAction);
};

// Use this if there's no state necessary to save in order to execute
// the action.
struct NuPlayer::SimpleAction : public Action {
    typedef void (NuPlayer::*ActionFunc)();

    SimpleAction(ActionFunc func)
        : mFunc(func) {
    }

    virtual void execute(NuPlayer *player) {
        (player->*mFunc)();
    }

private:
    ActionFunc mFunc;

    DISALLOW_EVIL_CONSTRUCTORS(SimpleAction);
};

////////////////////////////////////////////////////////////////////////////////

NuPlayer::NuPlayer(pid_t pid)
    : mUIDValid(false),
      mPID(pid),
      mSourceFlags(0),
      mOffloadAudio(false),
      mAudioDecoderGeneration(0),
      mVideoDecoderGeneration(0),
      mRendererGeneration(0),
      mPreviousSeekTimeUs(0),
      mAudioEOS(false),
      mVideoEOS(false),
      mScanSourcesPending(false),
      mScanSourcesGeneration(0),
      mPollDurationGeneration(0),
      mTimedTextGeneration(0),
      mFlushingAudio(NONE),
      mFlushingVideo(NONE),
      mResumePending(false),
      mVideoScalingMode(NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW),
      mPlaybackSettings(AUDIO_PLAYBACK_RATE_DEFAULT),
      mVideoFpsHint(-1.f),
      mStarted(false),
      mSourceStarted(false),
      mPaused(false),
      mPausedByClient(false),
      mPausedForBuffering(false),
      mIsProg(-1),
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
      mStartTime(ALooper::GetNowUs()),
#endif
#ifdef RTK_PLATFORM
      mIsM3u8LiveFlg(0),
      mIsMVC(0),
#endif
      mIsHdr(-1) {
    clearFlushComplete();
#if defined (RTK_PLATFORM) && defined (RTK_NET_ASYNC_PLAYBACK_FLOW)
    mAsyncIsResetFlg = false;
    mIsNetAsyncFlw = false;
#endif
#ifdef RTK_PLATFORM
    mIsLiveFlw = false;
    mIsHttpFlw = false;
    mDecErrorCnt = 0;
    mDecErrPrePosUs = 0;
#endif
}

NuPlayer::~NuPlayer() {
}

void NuPlayer::setUID(uid_t uid) {
    mUIDValid = true;
    mUID = uid;
}

void NuPlayer::setDriver(const wp<NuPlayerDriver> &driver) {
    mDriver = driver;
}

void NuPlayer::setDataSourceAsync(const sp<IStreamSource> &source) {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif
    sp<AMessage> msg = new AMessage(kWhatSetDataSource, this);

    sp<AMessage> notify = new AMessage(kWhatSourceNotify, this);

    msg->setObject("source", new StreamingSource(notify, source));
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:setDataSourceAsync_IStreamSource timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif
    msg->post();
}

static bool IsHTTPLiveURL(const char *url) {
    if (!strncasecmp("http://", url, 7)
            || !strncasecmp("https://", url, 8)
            || !strncasecmp("file://", url, 7)) {
        size_t len = strlen(url);
        if (len >= 5 && !strcasecmp(".m3u8", &url[len - 5])) {
            return true;
        }

        //if (strstr(url,"m3u8")) {
        //if (strstr(url,"m3u8") || strstr(url,"type=hls")) {
        if (strstr(url,"type=hls")) {
            return true;
        }
    }

    return false;
}


#ifdef RTK_PLATFORM


void *NuPlayer::ThreadWrapper(void *me)
{
    return (void *)(uintptr_t) static_cast<NuPlayer *>(me)->threadFunc();
}

status_t NuPlayer::threadFunc()
{
    prctl(PR_SET_NAME, (unsigned long)"IsHTTPLiveDataThread", 0, 0, 0);
    int t_pid = 0;
    int pri = getpriority(PRIO_PROCESS, t_pid);
    ALOGD("IsHTTPLiveDataThread::threadFunc ,priority = %d,pid=%d", pri, getpid());
    status_t err = OK;

    mIsGetHTTPLiveData = false;
    mIsHTTPLiveData = false;

    const char *uri = mUri.c_str();
    if (!strcasecmp((uri+strlen(uri)-4), ".flv"))
    {
        //ALOGD("\t\033[0;32;32m%s:%d   strlen(uri)= %d  uri= %s\033[m\n", __FILE__, __LINE__, (int)strlen(uri), uri);
        mIsGetHTTPLiveData = true;
        return err;
    }

    sp<IMediaHTTPConnection> conn = mHttpService->makeHTTPConnection();
    sp<HTTPBase> httpSource = new MediaHTTP(conn);
    off64_t size = 64;
    int64_t timeout = 10;//sec
    int maxRetryCount = 10;
    ssize_t n;
    unsigned char* buf;
    if (httpSource->connect(mUri.c_str(), mHeaders) != OK) {
        ALOGD("%s:%d http connect fail, url=%s\n", __FILE__, __LINE__, mUri.c_str());
        mTreadRun = false;
    }
    sp<ABuffer> buffer = new ABuffer(size);
    buffer->setRange(0, 0);

    static nsecs_t start_time;
    start_time = systemTime();
    ALOGD("Start %s::IsHTTPLiveData with %s ", __FUNCTION__, mUri.c_str());
    for(int i = 0; i < maxRetryCount; i++)//retry for waiting source ready
    {
        if(mTreadRun == false)
            break;
        n = httpSource->readAt(0, buffer->data(), size);
        if(n == size)
            break;
        else
        {
            ALOGE("%s read size %llu but get %zd retry %d times current %lld ",
                    __FUNCTION__, (long long)size, n, i, (long long)(systemTime()-start_time)/1000000);
            if((systemTime() - start_time) / 1000000000 > timeout)
            {
                ALOGE("waiting HTTP readData may cause ANR, skip retrying ...");
                break;
            }
        }
#ifdef RTK_NET_ASYNC_PLAYBACK_FLOW
        if(mAsyncIsResetFlg == true)
        {
            ALOGI("[%s][%d] mAsyncIsResetFlg true return\n",__FUNCTION__,__LINE__);
            break;
        }
#endif
        usleep(10000);
    }

    buf = buffer->data();
    httpSource->disconnect();
//    ALOGD("\t\033[0;32;32m%s:%d   ===========   START  ===========  \033[m\n", __FILE__, __LINE__);
//    ALOGD("\t\033[0;32;32m   buf: %s  \033[m\n", buf);
//    ALOGD("\t\033[0;32;32m%s:%d   ===========   END  ===========  \033[m\n", __FILE__, __LINE__);
    if (strstr((char *)buf, "EXTM3U"))
        mIsHTTPLiveData = true;

    mIsGetHTTPLiveData = true;
    ALOGE("[%s][%d]Thread End, pthread_join.\n",__FUNCTION__,__LINE__);
    void *dummy = 0;
    pthread_join(mThread, &dummy);
    return err;
}

bool NuPlayer::IsHTTPLiveData(const sp<IMediaHTTPService> &httpService,
                           const char *url, const KeyedVector<String8, String8> *headers) {

    if(strncasecmp(url, "http", 4))
    {
        return false;
    }

    mUri.clear();
    mUri = url;
    mHeaders = headers;
    mHttpService = httpService;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&mThread, &attr, ThreadWrapper, this);
    pthread_attr_destroy(&attr);

    mTreadRun = true;
    int maxRetryCount = 100;
    for(int i = 0; i < maxRetryCount; i++)//retry for waiting source ready
    {
        usleep(100000);
        if(mIsGetHTTPLiveData == true)
        {
            mTreadRun = false;
            if(mIsHTTPLiveData == true)
                return true;
            break;
        }
#ifdef RTK_NET_ASYNC_PLAYBACK_FLOW
        if(mAsyncIsResetFlg == true)
        {
            ALOGI("[%s][%d] mAsyncIsResetFlg true return\n",__FUNCTION__,__LINE__);
            break;
        }
#endif
    }
    mTreadRun = false;
    return false;
}
#endif
#if defined (RTK_PLATFORM) && defined (RTK_NET_ASYNC_PLAYBACK_FLOW)
void NuPlayer::setDataSourceAsyncHttpCheck(
        const sp<IMediaHTTPService> &httpService,
        const char *url,
        const KeyedVector<String8, String8> *headers) {
    size_t len = strlen(url);

    sp<AMessage> notify = new AMessage(kWhatSourceNotify, this);

    sp<Source> source;
#ifdef RTK_PLATFORM
    if (!strncasecmp("http://", url, 7)
            || !strncasecmp("https://", url, 8))
    {
        mIsHttpFlw = true;
    }
#endif
    if (IsHTTPLiveURL(url) || IsHTTPLiveData(httpService, url, headers)) {
        mIsLiveFlw = true;
        source = new HTTPLiveSource(notify, httpService, url, headers);
    } else if (!strncasecmp(url, "rtsp://", 7)) {
        source = new RTSPSource(
                notify, httpService, url, headers, mUIDValid, mUID);
    } else if ((!strncasecmp(url, "http://", 7)
                || !strncasecmp(url, "https://", 8))
                    && ((len >= 4 && !strcasecmp(".sdp", &url[len - 4]))
                    || strstr(url, ".sdp?"))) {
        source = new RTSPSource(
                notify, httpService, url, headers, mUIDValid, mUID, true);
    } else {
        sp<GenericSource> genericSource =
                new GenericSource(notify, mUIDValid, mUID);
        // Don't set FLAG_SECURE on mSourceFlags here for widevine.
        // The correct flags will be updated in Source::kWhatFlagsChanged
        // handler when  GenericSource is prepared.

        status_t err = genericSource->setDataSource(httpService, url, headers);

        if (err == OK) {
            source = genericSource;
        } else {
            ALOGE("Failed to set data source!");
        }
    }
    ALOGV("kWhatSetDataSource");
    CHECK(mSource == NULL);
    status_t err = OK;
    if (source != NULL) {
        mSource = source;
    } else {
        err = UNKNOWN_ERROR;
    }

    CHECK(mDriver != NULL);
    sp<NuPlayerDriver> driver = mDriver.promote();
    if (driver != NULL) {
        driver->notifySetDataSourceCompleted(err);
    }

}
#endif
void NuPlayer::setDataSourceAsync(
        const sp<IMediaHTTPService> &httpService,
        const char *url,
        const KeyedVector<String8, String8> *headers) {

#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif


#if defined (RTK_PLATFORM) && defined (RTK_NET_ASYNC_PLAYBACK_FLOW)
    sp<AMessage> msg = new AMessage(kWhatHttpCheck, this);
    msg->setPointer("headers", (void *) &headers);
    msg->setString("url", url);
    mHttpService = httpService;
    msg->post();
    return;
#else

    sp<AMessage> msg = new AMessage(kWhatSetDataSource, this);
    size_t len = strlen(url);

    sp<AMessage> notify = new AMessage(kWhatSourceNotify, this);

    sp<Source> source;
    //if (IsHTTPLiveURL(url)) {
    if (IsHTTPLiveURL(url) || IsHTTPLiveData(httpService, url, headers)) {
#ifdef RTK_PLATFORM
        mIsM3u8LiveFlg = 1;
#endif
        source = new HTTPLiveSource(notify, httpService, url, headers);
    } else if (!strncasecmp(url, "rtsp://", 7)) {
        source = new RTSPSource(
                notify, httpService, url, headers, mUIDValid, mUID);
    } else if ((!strncasecmp(url, "http://", 7)
                || !strncasecmp(url, "https://", 8))
                    && ((len >= 4 && !strcasecmp(".sdp", &url[len - 4]))
                    || strstr(url, ".sdp?"))) {
        source = new RTSPSource(
                notify, httpService, url, headers, mUIDValid, mUID, true);
    } else {
        sp<GenericSource> genericSource =
                new GenericSource(notify, mUIDValid, mUID);
        // Don't set FLAG_SECURE on mSourceFlags here for widevine.
        // The correct flags will be updated in Source::kWhatFlagsChanged
        // handler when  GenericSource is prepared.

        status_t err = genericSource->setDataSource(httpService, url, headers);

        if (err == OK) {
            source = genericSource;
        } else {
            ALOGE("Failed to set data source!");
        }
    }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:setDataSourceAsync_httpService timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif
    msg->setObject("source", source);
    msg->post();
#endif
}

void NuPlayer::setDataSourceAsync(int fd, int64_t offset, int64_t length) {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif
    sp<AMessage> msg = new AMessage(kWhatSetDataSource, this);

    sp<AMessage> notify = new AMessage(kWhatSourceNotify, this);

    sp<GenericSource> source =
            new GenericSource(notify, mUIDValid, mUID);

    status_t err = source->setDataSource(fd, offset, length);

    if (err != OK) {
        ALOGE("Failed to set data source!");
        source = NULL;
    }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:setDataSourceAsync_fd timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif
    msg->setObject("source", source);
    msg->post();
}

void NuPlayer::setDataSourceAsync(const sp<DataSource> &dataSource) {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif
    sp<AMessage> msg = new AMessage(kWhatSetDataSource, this);
    sp<AMessage> notify = new AMessage(kWhatSourceNotify, this);

    sp<GenericSource> source = new GenericSource(notify, mUIDValid, mUID);
    status_t err = source->setDataSource(dataSource);

    if (err != OK) {
        ALOGE("Failed to set data source!");
        source = NULL;
    }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:setDataSourceAsync_dataSource timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif
    msg->setObject("source", source);
    msg->post();
}

void NuPlayer::prepareAsync() {
    (new AMessage(kWhatPrepare, this))->post();
}

void NuPlayer::setVideoSurfaceTextureAsync(
        const sp<IGraphicBufferProducer> &bufferProducer) {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif
    sp<AMessage> msg = new AMessage(kWhatSetVideoSurface, this);

    if (bufferProducer == NULL) {
        msg->setObject("surface", NULL);
    } else {
        msg->setObject("surface", new Surface(bufferProducer, true /* controlledByApp */));
    }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:setVideoSurfaceTextureAsync timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif
    msg->post();
}

void NuPlayer::setAudioSink(const sp<MediaPlayerBase::AudioSink> &sink) {
    sp<AMessage> msg = new AMessage(kWhatSetAudioSink, this);
    msg->setObject("sink", sink);
    msg->post();
}

void NuPlayer::start() {
    (new AMessage(kWhatStart, this))->post();
}

status_t NuPlayer::setPlaybackSettings(const AudioPlaybackRate &rate) {
    // do some cursory validation of the settings here. audio modes are
    // only validated when set on the audiosink.
     if ((rate.mSpeed != 0.f && rate.mSpeed < AUDIO_TIMESTRETCH_SPEED_MIN)
            || rate.mSpeed > AUDIO_TIMESTRETCH_SPEED_MAX
            || rate.mPitch < AUDIO_TIMESTRETCH_SPEED_MIN
            || rate.mPitch > AUDIO_TIMESTRETCH_SPEED_MAX) {
        return BAD_VALUE;
    }
    sp<AMessage> msg = new AMessage(kWhatConfigPlayback, this);
    writeToAMessage(msg, rate);
    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    if (err == OK && response != NULL) {
        CHECK(response->findInt32("err", &err));
    }
    return err;
}

status_t NuPlayer::getPlaybackSettings(AudioPlaybackRate *rate /* nonnull */) {
    sp<AMessage> msg = new AMessage(kWhatGetPlaybackSettings, this);
    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    if (err == OK && response != NULL) {
        CHECK(response->findInt32("err", &err));
        if (err == OK) {
            readFromAMessage(response, rate);
        }
    }
    return err;
}

status_t NuPlayer::setSyncSettings(const AVSyncSettings &sync, float videoFpsHint) {
    sp<AMessage> msg = new AMessage(kWhatConfigSync, this);
    writeToAMessage(msg, sync, videoFpsHint);
    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    if (err == OK && response != NULL) {
        CHECK(response->findInt32("err", &err));
    }
    return err;
}

status_t NuPlayer::getSyncSettings(
        AVSyncSettings *sync /* nonnull */, float *videoFps /* nonnull */) {
    sp<AMessage> msg = new AMessage(kWhatGetSyncSettings, this);
    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    if (err == OK && response != NULL) {
        CHECK(response->findInt32("err", &err));
        if (err == OK) {
            readFromAMessage(response, sync, videoFps);
        }
    }
    return err;
}

void NuPlayer::pause() {
    (new AMessage(kWhatPause, this))->post();
}

void NuPlayer::resetAsync() {
    if (mSource != NULL) {
        // During a reset, the data source might be unresponsive already, we need to
        // disconnect explicitly so that reads exit promptly.
        // We can't queue the disconnect request to the looper, as it might be
        // queued behind a stuck read and never gets processed.
        // Doing a disconnect outside the looper to allows the pending reads to exit
        // (either successfully or with error).
        mSource->disconnect();
    }

    (new AMessage(kWhatReset, this))->post();
}

void NuPlayer::seekToAsync(int64_t seekTimeUs, bool needNotify) {
    sp<AMessage> msg = new AMessage(kWhatSeek, this);
    msg->setInt64("seekTimeUs", seekTimeUs);
    msg->setInt32("needNotify", needNotify);
    msg->post();
}


void NuPlayer::writeTrackInfo(
        Parcel* reply, const sp<AMessage> format) const {
    int32_t trackType;
    CHECK(format->findInt32("type", &trackType));

    AString mime;
    if (!format->findString("mime", &mime)) {
        // Java MediaPlayer only uses mimetype for subtitle and timedtext tracks.
        // If we can't find the mimetype here it means that we wouldn't be needing
        // the mimetype on the Java end. We still write a placeholder mime to keep the
        // (de)serialization logic simple.
        if (trackType == MEDIA_TRACK_TYPE_AUDIO) {
            mime = "audio/";
        } else if (trackType == MEDIA_TRACK_TYPE_VIDEO) {
            mime = "video/";
        } else {
            TRESPASS();
        }
    }

    AString lang;
    CHECK(format->findString("language", &lang));

    if (trackType == MEDIA_TRACK_TYPE_SUBTITLE) {
        int32_t isAuto, isDefault, isForced;
        CHECK(format->findInt32("auto", &isAuto));
        CHECK(format->findInt32("default", &isDefault));
        CHECK(format->findInt32("forced", &isForced));
        reply->writeInt32(3); // write something non-zero
        reply->writeInt32(trackType);
        reply->writeString16(String16(mime.c_str()));
        reply->writeString16(String16(lang.c_str()));
        reply->writeInt32(isAuto);
        reply->writeInt32(isDefault);
        reply->writeInt32(isForced);
    }
    else if(trackType == MEDIA_TRACK_TYPE_AUDIO) {
        ALOGD("Audio mime-type: %s", mime.c_str());
        int32_t sampleRate=0, channelCount=0;
        format->findInt32("sample-rate", &sampleRate);
        format->findInt32("channel-count", &channelCount);
        reply->writeInt32(5); // write something non-zero
        reply->writeInt32(trackType);
        reply->writeString16(String16(getAudioTypeString(mime.c_str())));
        reply->writeString16(String16(lang.c_str()));
        reply->writeInt32(sampleRate);
        reply->writeInt32(channelCount);
    }
    else if(trackType == MEDIA_TRACK_TYPE_VIDEO) {
        ALOGD("Video mime-type: %s", mime.c_str());
        int32_t width=0, height=0, frameRate=0;
        format->findInt32("width", &width);
        format->findInt32("height", &height);
        format->findInt32("frame-rate", &frameRate);
#ifndef RTK_PLATFORM
        reply->writeInt32(6); // write something non-zero
#else
        reply->writeInt32(9); // write something non-zero
#endif
        reply->writeInt32(trackType);
        reply->writeString16(String16(getVideoTypeString(mime.c_str())));
        reply->writeString16(String16(lang.c_str()));
        reply->writeInt32(width);
        reply->writeInt32(height);
#ifndef RTK_PLATFORM
        reply->writeFloat(frameRate);
#else
        float rtkFrameRate;
        format->findFloat("frame-rate-float", &rtkFrameRate);
        if(rtkFrameRate > 0)
            reply->writeFloat(rtkFrameRate);
        else
            reply->writeFloat(frameRate);
        int32_t isProg=-1, isHdr=-1, isMVC=-1;
        format->findInt32("is-progressive", &isProg);
        format->findInt32("is-hdr", &isHdr);
        format->findInt32("is-mvc", &isMVC);
        reply->writeInt32(isProg);
        reply->writeInt32(isHdr);
        reply->writeInt32(isMVC);
#endif
    }
    else {
        reply->writeInt32(3); // write something non-zero
        reply->writeInt32(trackType);
        reply->writeString16(String16(mime.c_str()));
        reply->writeString16(String16(lang.c_str()));
    }
}

void NuPlayer::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatSetDataSource:
        {
            ALOGV("kWhatSetDataSource");
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:kWhatSetDataSource timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            CHECK(mSource == NULL);

            status_t err = OK;
            sp<RefBase> obj;
            CHECK(msg->findObject("source", &obj));
            if (obj != NULL) {
                mSource = static_cast<Source *>(obj.get());
            } else {
                err = UNKNOWN_ERROR;
            }

            CHECK(mDriver != NULL);
            sp<NuPlayerDriver> driver = mDriver.promote();
            if (driver != NULL) {
                driver->notifySetDataSourceCompleted(err);
            }
            break;
        }

        case kWhatPrepare:
        {

#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatPrepare");
            ALOGD("CountFlag:kWhatPrepare timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            int64_t prepareAsync_start = ALooper::GetNowUs();
#endif
            mSource->prepareAsync();
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:prepareAsync timeUs: %" PRId64, ALooper::GetNowUs() - prepareAsync_start);
#endif
            break;
        }

        case kWhatGetTrackInfo:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatGetTrackInfo");
            ALOGD("CountFlag:kWhatGetTrackInfo timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));

            Parcel* reply;
            CHECK(msg->findPointer("reply", (void**)&reply));

            size_t inbandTracks = 0;
            if (mSource != NULL) {
                inbandTracks = mSource->getTrackCount();
            }

            size_t ccTracks = 0;
            if (mCCDecoder != NULL) {
                ccTracks = mCCDecoder->getTrackCount();
            }

            // total track count
            reply->writeInt32(inbandTracks + ccTracks);
            // write inband tracks
            for (size_t i = 0; i < inbandTracks; ++i) {
#ifdef RTK_PLATFORM
                sp<AMessage> format = mSource->getTrackInfo(i);
                int32_t type32;
                CHECK(format->findInt32("type", (int32_t*)&type32));
                if(type32 == MEDIA_TRACK_TYPE_VIDEO) {
                    if(getFrameRate() < 0) {
                        format->setFloat("frame-rate-float", mFrameRate);
                    }
                    format->setInt32("is-progressive", mIsProg);
                    format->setInt32("is-hdr", mIsHdr);
                    format->setInt32("is-mvc", mIsMVC);
                }
                writeTrackInfo(reply, format);
#else
                writeTrackInfo(reply, mSource->getTrackInfo(i));
#endif
            }

            // write CC track
            for (size_t i = 0; i < ccTracks; ++i) {
                writeTrackInfo(reply, mCCDecoder->getTrackInfo(i));
            }

            sp<AMessage> response = new AMessage;
            response->postReply(replyID);
            break;
        }

        case kWhatGetSelectedTrack:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatGetSelectedTrack");
            ALOGD("CountFlag:kWhatGetSelectedTrack timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            status_t err = INVALID_OPERATION;
            if (mSource != NULL) {
                err = OK;

                int32_t type32;
                CHECK(msg->findInt32("type", (int32_t*)&type32));
                media_track_type type = (media_track_type)type32;
                ssize_t selectedTrack = mSource->getSelectedTrack(type);

                Parcel* reply;
                CHECK(msg->findPointer("reply", (void**)&reply));
                reply->writeInt32(selectedTrack);
            }

            sp<AMessage> response = new AMessage;
            response->setInt32("err", err);

            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));
            response->postReply(replyID);
            break;
        }

        case kWhatSelectTrack:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatSelectTrack");
            ALOGD("CountFlag:kWhatSelectTrack timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));

            size_t trackIndex;
            int32_t select;
            int64_t timeUs;
            CHECK(msg->findSize("trackIndex", &trackIndex));
            CHECK(msg->findInt32("select", &select));
            CHECK(msg->findInt64("timeUs", &timeUs));

            status_t err = INVALID_OPERATION;

            size_t inbandTracks = 0;
            if (mSource != NULL) {
                inbandTracks = mSource->getTrackCount();
            }
            size_t ccTracks = 0;
            if (mCCDecoder != NULL) {
                ccTracks = mCCDecoder->getTrackCount();
            }

            if (trackIndex < inbandTracks) {
                err = mSource->selectTrack(trackIndex, select, timeUs);

                if (!select && err == OK) {
                    int32_t type;
                    sp<AMessage> info = mSource->getTrackInfo(trackIndex);
                    if (info != NULL
                            && info->findInt32("type", &type)
                            && type == MEDIA_TRACK_TYPE_TIMEDTEXT) {
                        ++mTimedTextGeneration;
                    }
                }
#ifdef RTK_PLATFORM
                ALOGI("[%s][%d] setSelectTrackFlg.\n",__FUNCTION__,__LINE__);
                mRenderer->setSelectTrackFlg(1);

                mDeferredActions.push_back(
                        new FlushDecoderAction(FLUSH_CMD_FLUSH /* audio */,
                            FLUSH_CMD_FLUSH /* video */));
                mDeferredActions.push_back(
                        new SeekAction(timeUs));
                // After a flush without shutdown, decoder is paused.
                // Don't resume it until source seek is done, otherwise it could
                // start pulling stale data too soon.
                mDeferredActions.push_back(
                        new ResumeDecoderAction(0));

                processDeferredActions();
#endif
            } else {
                trackIndex -= inbandTracks;

                if (trackIndex < ccTracks) {
                    err = mCCDecoder->selectTrack(trackIndex, select);
                }
            }

            sp<AMessage> response = new AMessage;
            response->setInt32("err", err);

            response->postReply(replyID);
            break;
        }

        case kWhatPollDuration:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatPollDuration");
            ALOGD("CountFlag:kWhatPollDuration timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            int32_t generation;
            CHECK(msg->findInt32("generation", &generation));

            if (generation != mPollDurationGeneration) {
                // stale
                break;
            }

            int64_t durationUs;
            if (mDriver != NULL && mSource->getDuration(&durationUs) == OK) {
                sp<NuPlayerDriver> driver = mDriver.promote();
                if (driver != NULL) {
                    driver->notifyDuration(durationUs);
                }
            }
#ifdef RTK_PLATFORM
            if(mIsLiveFlw == true && mDecErrorCnt > 0)
            {
                ALOGD("[%s][%d] mDecErrorCnt::%d check Render status: \n",__FUNCTION__,__LINE__, mDecErrorCnt);
                int64_t currentPositionUs;
                if (getCurrentPosition(&currentPositionUs) == OK)
                {
                    if((mDecErrPrePosUs < currentPositionUs) && ((currentPositionUs - mDecErrPrePosUs) > 10 * 1E6))
                    {
                        mDecErrorCnt--;
                        if(mDecErrorCnt == 0)
                            cancelPollDuration();
                    }
                }
            }
#endif
            msg->post(1000000ll);  // poll again in a second.
            break;
        }

        case kWhatSetVideoSurface:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatSetVideoSurface");
            ALOGD("CountFlag:kWhatSetVideoSurface timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            int64_t kWhatSetVideoSurface_start = ALooper::GetNowUs();
#endif
            sp<RefBase> obj;
            CHECK(msg->findObject("surface", &obj));
            sp<Surface> surface = static_cast<Surface *>(obj.get());

            ALOGD("onSetVideoSurface(%p, %s video decoder)",
                    surface.get(),
                    (mSource != NULL && mStarted && mSource->getFormat(false /* audio */) != NULL
                            && mVideoDecoder != NULL) ? "have" : "no");

            // Need to check mStarted before calling mSource->getFormat because NuPlayer might
            // be in preparing state and it could take long time.
            // When mStarted is true, mSource must have been set.
            if (mSource == NULL || !mStarted || mSource->getFormat(false /* audio */) == NULL
                    // NOTE: mVideoDecoder's mSurface is always non-null
                    || (mVideoDecoder != NULL && mVideoDecoder->setVideoSurface(surface) == OK)) {
                performSetSurface(surface);
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
                ALOGD("CountDuration:kWhatSetVideoSurface timeUs: %" PRId64, ALooper::GetNowUs() - kWhatSetVideoSurface_start);
#endif
                break;
            }

            mDeferredActions.push_back(
                    new FlushDecoderAction(FLUSH_CMD_FLUSH /* audio */,
                                           FLUSH_CMD_SHUTDOWN /* video */));

            mDeferredActions.push_back(new SetSurfaceAction(surface));

            if (obj != NULL || mAudioDecoder != NULL) {
                if (mStarted) {
                    // Issue a seek to refresh the video screen only if started otherwise
                    // the extractor may not yet be started and will assert.
                    // If the video decoder is not set (perhaps audio only in this case)
                    // do not perform a seek as it is not needed.
                    int64_t currentPositionUs = 0;
                    if (getCurrentPosition(&currentPositionUs) == OK) {
                        mDeferredActions.push_back(
                                new SeekAction(currentPositionUs));
                    }
                }

                // If there is a new surface texture, instantiate decoders
                // again if possible.
                mDeferredActions.push_back(
                        new SimpleAction(&NuPlayer::performScanSources));
            }

            // After a flush without shutdown, decoder is paused.
            // Don't resume it until source seek is done, otherwise it could
            // start pulling stale data too soon.
            mDeferredActions.push_back(
                    new ResumeDecoderAction(false /* needNotify */));

            processDeferredActions();
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:kWhatSetVideoSurface timeUs: %" PRId64, ALooper::GetNowUs() - kWhatSetVideoSurface_start);
#endif
            break;
        }

        case kWhatSetAudioSink:
        {
            ALOGV("kWhatSetAudioSink");
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:kWhatSetAudioSink timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<RefBase> obj;
            CHECK(msg->findObject("sink", &obj));

            mAudioSink = static_cast<MediaPlayerBase::AudioSink *>(obj.get());
            break;
        }

        case kWhatStart:
        {
            ALOGV("kWhatStart");
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:kWhatStart timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            int64_t kWhatStart_start = ALooper::GetNowUs();
#endif
            if (mStarted) {
                // do not resume yet if the source is still buffering
                if (!mPausedForBuffering) {
                    onResume();
                }
            } else {
                onStart();
            }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:kWhatStart timeUs: %" PRId64, ALooper::GetNowUs() - kWhatStart_start);
#endif
            mPausedByClient = false;
            break;
        }

        case kWhatConfigPlayback:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatConfigPlayback");
            ALOGD("CountFlag:kWhatConfigPlayback timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));
            AudioPlaybackRate rate /* sanitized */;
            readFromAMessage(msg, &rate);
            status_t err = OK;
            if (mRenderer != NULL) {
                err = mRenderer->setPlaybackSettings(rate);
            }
            if (err == OK) {
                if (rate.mSpeed == 0.f) {
                    onPause();
                    // save all other settings (using non-paused speed)
                    // so we can restore them on start
                    AudioPlaybackRate newRate = rate;
                    newRate.mSpeed = mPlaybackSettings.mSpeed;
                    mPlaybackSettings = newRate;
                } else { /* rate.mSpeed != 0.f */
                    onResume();
                    mPlaybackSettings = rate;
                }
            }

            if (mVideoDecoder != NULL) {
                float rate = getFrameRate();
                if (rate > 0) {
                    sp<AMessage> params = new AMessage();
                    params->setFloat("operating-rate", rate * mPlaybackSettings.mSpeed);
                    mVideoDecoder->setParameters(params);
                }
            }

            sp<AMessage> response = new AMessage;
            response->setInt32("err", err);
            response->postReply(replyID);
            break;
        }

        case kWhatGetPlaybackSettings:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatGetPlaybackSettings");
            ALOGD("CountFlag:kWhatGetPlaybackSettings timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));
            AudioPlaybackRate rate = mPlaybackSettings;
            status_t err = OK;
            if (mRenderer != NULL) {
                err = mRenderer->getPlaybackSettings(&rate);
            }
            if (err == OK) {
                // get playback settings used by renderer, as it may be
                // slightly off due to audiosink not taking small changes.
                mPlaybackSettings = rate;
                if (mPaused) {
                    rate.mSpeed = 0.f;
                }
            }
            sp<AMessage> response = new AMessage;
            if (err == OK) {
                writeToAMessage(response, rate);
            }
            response->setInt32("err", err);
            response->postReply(replyID);
            break;
        }

        case kWhatConfigSync:
        {
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));

            ALOGV("kWhatConfigSync");
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:kWhatConfigSync timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            AVSyncSettings sync;
            float videoFpsHint;
            readFromAMessage(msg, &sync, &videoFpsHint);
            status_t err = OK;
            if (mRenderer != NULL) {
                err = mRenderer->setSyncSettings(sync, videoFpsHint);
            }
            if (err == OK) {
                mSyncSettings = sync;
                mVideoFpsHint = videoFpsHint;
            }
            sp<AMessage> response = new AMessage;
            response->setInt32("err", err);
            response->postReply(replyID);
            break;
        }

        case kWhatGetSyncSettings:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatGetSyncSettings");
            ALOGD("CountFlag:kWhatGetSyncSettings timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));
            AVSyncSettings sync = mSyncSettings;
            float videoFps = mVideoFpsHint;
            status_t err = OK;
            if (mRenderer != NULL) {
                err = mRenderer->getSyncSettings(&sync, &videoFps);
                if (err == OK) {
                    mSyncSettings = sync;
                    mVideoFpsHint = videoFps;
                }
            }
            sp<AMessage> response = new AMessage;
            if (err == OK) {
                writeToAMessage(response, sync, videoFps);
            }
            response->setInt32("err", err);
            response->postReply(replyID);
            break;
        }

        case kWhatScanSources:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatScanSources");
            ALOGD("CountFlag:kWhatScanSources timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            int64_t kWhatScanSources_start = ALooper::GetNowUs();
#endif
            int32_t generation;
            CHECK(msg->findInt32("generation", &generation));
            if (generation != mScanSourcesGeneration) {
                // Drop obsolete msg.
                break;
            }

            mScanSourcesPending = false;

            ALOGV("scanning sources haveAudio=%d, haveVideo=%d",
                 mAudioDecoder != NULL, mVideoDecoder != NULL);

            bool mHadAnySourcesBefore =
                (mAudioDecoder != NULL) || (mVideoDecoder != NULL);

            // initialize video before audio because successful initialization of
            // video may change deep buffer mode of audio.
            if (mSurface != NULL) {
                instantiateDecoder(false, &mVideoDecoder);
            }

            // Don't try to re-open audio sink if there's an existing decoder.
            if (mAudioSink != NULL && mAudioDecoder == NULL) {
                instantiateDecoder(true, &mAudioDecoder);
            }

            if (!mHadAnySourcesBefore
                    && (mAudioDecoder != NULL || mVideoDecoder != NULL)) {
                // This is the first time we've found anything playable.

                if (mSourceFlags & Source::FLAG_DYNAMIC_DURATION) {
                    schedulePollDuration();
                }
            }

            status_t err;
            if ((err = mSource->feedMoreTSData()) != OK) {
                if (mAudioDecoder == NULL && mVideoDecoder == NULL) {
                    // We're not currently decoding anything (no audio or
                    // video tracks found) and we just ran out of input data.

                    if (err == ERROR_END_OF_STREAM) {
                        notifyListener(MEDIA_PLAYBACK_COMPLETE, 0, 0);
                    } else {
                        notifyListener(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, err);
                    }
                }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
                ALOGD("CountDuration:kWhatScanSources timeUs: %" PRId64, ALooper::GetNowUs() - kWhatScanSources_start);
#endif
                break;
            }

            if ((mAudioDecoder == NULL && mAudioSink != NULL)
                    || (mVideoDecoder == NULL && mSurface != NULL)) {
                msg->post(100000ll);
                mScanSourcesPending = true;
            }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:kWhatScanSources timeUs: %" PRId64, ALooper::GetNowUs() - kWhatScanSources_start);
#endif
            break;
        }

        case kWhatVideoNotify:
        case kWhatAudioNotify:
        {
            bool audio = msg->what() == kWhatAudioNotify;
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            if(audio){
                ALOGD("CountFlag:kWhatAudioNotify timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            }else{
                ALOGD("CountFlag:kWhatVideoNotify timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            }
#endif
            int32_t currentDecoderGeneration =
                (audio? mAudioDecoderGeneration : mVideoDecoderGeneration);
            int32_t requesterGeneration = currentDecoderGeneration - 1;
            CHECK(msg->findInt32("generation", &requesterGeneration));

            if (requesterGeneration != currentDecoderGeneration) {
                ALOGV("got message from old %s decoder, generation(%d:%d)",
                        audio ? "audio" : "video", requesterGeneration,
                        currentDecoderGeneration);
                sp<AMessage> reply;
                if (!(msg->findMessage("reply", &reply))) {
                    return;
                }

                reply->setInt32("err", INFO_DISCONTINUITY);
                reply->post();
                return;
            }

            int32_t what;
            CHECK(msg->findInt32("what", &what));

            if (what == DecoderBase::kWhatInputDiscontinuity) {
                int32_t formatChange;
                CHECK(msg->findInt32("formatChange", &formatChange));

                ALOGV("%s discontinuity: formatChange %d",
                        audio ? "audio" : "video", formatChange);

                if (formatChange) {
                    mDeferredActions.push_back(
                            new FlushDecoderAction(
                                audio ? FLUSH_CMD_SHUTDOWN : FLUSH_CMD_NONE,
                                audio ? FLUSH_CMD_NONE : FLUSH_CMD_SHUTDOWN));
                }

                mDeferredActions.push_back(
                        new SimpleAction(
                                &NuPlayer::performScanSources));

                processDeferredActions();
            } else if (what == DecoderBase::kWhatEOS) {
                int32_t err;
                CHECK(msg->findInt32("err", &err));

                if (err == ERROR_END_OF_STREAM) {
                    ALOGV("got %s decoder EOS", audio ? "audio" : "video");
                } else {
                    ALOGV("got %s decoder EOS w/ error %d",
                         audio ? "audio" : "video",
                         err);
                }

                mRenderer->queueEOS(audio, err);
            } else if (what == DecoderBase::kWhatFlushCompleted) {
                ALOGV("decoder %s flush completed", audio ? "audio" : "video");

                handleFlushComplete(audio, true /* isDecoder */);
                finishFlushIfPossible();
            } else if (what == DecoderBase::kWhatVideoSizeChanged) {
                sp<AMessage> format;
                CHECK(msg->findMessage("format", &format));

                sp<AMessage> inputFormat =
                        mSource->getFormat(false /* audio */);
#ifdef RTK_PLATFORM
                if(!audio) {
                    if(getFrameRate() < 0) {
                        double frameRate = 0;
                        format->findDouble("rtk-frame-rate", &frameRate);
                        if(frameRate > 0)
                            mFrameRate = frameRate;
                    }
                    format->findInt32("rtk-is-progressive", &mIsProg);
                    format->findInt32("rtk-is-hdr", &mIsHdr);
                    format->findInt32("rtk-is-mvc", &mIsMVC);
                }
#endif
                updateVideoSize(inputFormat, format);
            } else if (what == DecoderBase::kWhatShutdownCompleted) {
                ALOGV("%s shutdown completed", audio ? "audio" : "video");
                if (audio) {
                    mAudioDecoder.clear();
                    ++mAudioDecoderGeneration;

                    CHECK_EQ((int)mFlushingAudio, (int)SHUTTING_DOWN_DECODER);
                    mFlushingAudio = SHUT_DOWN;
                } else {
                    mVideoDecoder.clear();
                    ++mVideoDecoderGeneration;

                    CHECK_EQ((int)mFlushingVideo, (int)SHUTTING_DOWN_DECODER);
                    mFlushingVideo = SHUT_DOWN;
                }

                finishFlushIfPossible();
            } else if (what == DecoderBase::kWhatResumeCompleted) {
                finishResume();
            } else if (what == DecoderBase::kWhatError) {
                status_t err;
                if (!msg->findInt32("err", &err) || err == OK) {
                    err = UNKNOWN_ERROR;
                }

                // Decoder errors can be due to Source (e.g. from streaming),
                // or from decoding corrupted bitstreams, or from other decoder
                // MediaCodec operations (e.g. from an ongoing reset or seek).
                // They may also be due to openAudioSink failure at
                // decoder start or after a format change.
                //
                // We try to gracefully shut down the affected decoder if possible,
                // rather than trying to force the shutdown with something
                // similar to performReset(). This method can lead to a hang
                // if MediaCodec functions block after an error, but they should
                // typically return INVALID_OPERATION instead of blocking.

                FlushStatus *flushing = audio ? &mFlushingAudio : &mFlushingVideo;
                ALOGE("received error(%#x) from %s decoder, flushing(%d), now shutting down",
                        err, audio ? "audio" : "video", *flushing);
#ifdef RTK_PLATFORM
                if(mIsLiveFlw == true)
                {
                    mDecErrorCnt++;
                    if(mDecErrorCnt < 10)
                    {
                        ALOGI("[%s][%d] mIsLiveFlw true, mDecErrorCnt::%d skip decode error msg.\n",__FUNCTION__,__LINE__, mDecErrorCnt);
                        mDeferredActions.push_back(
                                new FlushDecoderAction(FLUSH_CMD_FLUSH /* audio */,
                                    FLUSH_CMD_FLUSH /* video */));

                        // After a flush without shutdown, decoder is paused.
                        // Don't resume it until source seek is done, otherwise it could
                        // start pulling stale data too soon.
                        mDeferredActions.push_back(
                                new ResumeDecoderAction(0));

                        processDeferredActions();
                        int64_t currentPositionUs;
                        if (getCurrentPosition(&currentPositionUs) == OK)
                        {
                            mDecErrPrePosUs = currentPositionUs;
                            schedulePollDuration();
                            break;
                        }
                        else
                            ALOGW("[%s][%d] getCurrentPosition fail.\n",__FUNCTION__,__LINE__);
                    }
                }
#endif

                switch (*flushing) {
                    case NONE:
                        mDeferredActions.push_back(
                                new FlushDecoderAction(
                                    audio ? FLUSH_CMD_SHUTDOWN : FLUSH_CMD_NONE,
                                    audio ? FLUSH_CMD_NONE : FLUSH_CMD_SHUTDOWN));
                        processDeferredActions();
                        break;
                    case FLUSHING_DECODER:
                        *flushing = FLUSHING_DECODER_SHUTDOWN; // initiate shutdown after flush.
                        break; // Wait for flush to complete.
                    case FLUSHING_DECODER_SHUTDOWN:
                        break; // Wait for flush to complete.
                    case SHUTTING_DOWN_DECODER:
                        break; // Wait for shutdown to complete.
                    case FLUSHED:
                        // Widevine source reads must stop before releasing the video decoder.
                        if (!audio && mSource != NULL && mSourceFlags & Source::FLAG_SECURE) {
                            mSource->stop();
                            mSourceStarted = false;
                        }
                        getDecoder(audio)->initiateShutdown(); // In the middle of a seek.
                        *flushing = SHUTTING_DOWN_DECODER;     // Shut down.
                        break;
                    case SHUT_DOWN:
                        finishFlushIfPossible();  // Should not occur.
                        break;                    // Finish anyways.
                }
                notifyListener(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, err);
            } else {
                ALOGV("Unhandled decoder notification %d '%c%c%c%c'.",
                      what,
                      what >> 24,
                      (what >> 16) & 0xff,
                      (what >> 8) & 0xff,
                      what & 0xff);
            }

            break;
        }

        case kWhatRendererNotify:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatRendererNotify");
            ALOGD("CountFlag:kWhatRendererNotify timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            int32_t requesterGeneration = mRendererGeneration - 1;
            CHECK(msg->findInt32("generation", &requesterGeneration));
            if (requesterGeneration != mRendererGeneration) {
                ALOGV("got message from old renderer, generation(%d:%d)",
                        requesterGeneration, mRendererGeneration);
                return;
            }

            int32_t what;
            CHECK(msg->findInt32("what", &what));

            if (what == Renderer::kWhatEOS) {
                int32_t audio;
                CHECK(msg->findInt32("audio", &audio));

                int32_t finalResult;
                CHECK(msg->findInt32("finalResult", &finalResult));

                if (audio) {
                    mAudioEOS = true;
                } else {
                    mVideoEOS = true;
                }

                if (finalResult == ERROR_END_OF_STREAM) {
                    ALOGV("reached %s EOS", audio ? "audio" : "video");
                } else {
                    ALOGE("%s track encountered an error (%d)",
                         audio ? "audio" : "video", finalResult);

                    notifyListener(
                            MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, finalResult);
                }

                if ((mAudioEOS || mAudioDecoder == NULL)
                        && (mVideoEOS || mVideoDecoder == NULL)) {
                    notifyListener(MEDIA_PLAYBACK_COMPLETE, 0, 0);
                }
            } else if (what == Renderer::kWhatFlushComplete) {
                int32_t audio;
                CHECK(msg->findInt32("audio", &audio));

                if (audio) {
                    mAudioEOS = false;
                } else {
                    mVideoEOS = false;
                }

                ALOGV("renderer %s flush completed.", audio ? "audio" : "video");
                if (audio && (mFlushingAudio == NONE || mFlushingAudio == FLUSHED
                        || mFlushingAudio == SHUT_DOWN)) {
                    // Flush has been handled by tear down.
                    break;
                }
                handleFlushComplete(audio, false /* isDecoder */);
                finishFlushIfPossible();
            } else if (what == Renderer::kWhatVideoRenderingStart) {
                notifyListener(MEDIA_INFO, MEDIA_INFO_RENDERING_START, 0);
            } else if (what == Renderer::kWhatMediaRenderingStart) {
                ALOGV("media rendering started");
                notifyListener(MEDIA_STARTED, 0, 0);
            } else if (what == Renderer::kWhatAudioTearDown) {
                int32_t reason;
                CHECK(msg->findInt32("reason", &reason));
                ALOGV("Tear down audio with reason %d.", reason);
                mAudioDecoder.clear();
                ++mAudioDecoderGeneration;
                bool needsToCreateAudioDecoder = true;
                if (mFlushingAudio == FLUSHING_DECODER) {
                    mFlushComplete[1 /* audio */][1 /* isDecoder */] = true;
                    mFlushingAudio = FLUSHED;
                    finishFlushIfPossible();
                } else if (mFlushingAudio == FLUSHING_DECODER_SHUTDOWN
                        || mFlushingAudio == SHUTTING_DOWN_DECODER) {
                    mFlushComplete[1 /* audio */][1 /* isDecoder */] = true;
                    mFlushingAudio = SHUT_DOWN;
                    finishFlushIfPossible();
                    needsToCreateAudioDecoder = false;
                }
                if (mRenderer == NULL) {
                    break;
                }
                closeAudioSink();
                mRenderer->flush(
                        true /* audio */, false /* notifyComplete */);
                if (mVideoDecoder != NULL) {
                    mRenderer->flush(
                            false /* audio */, false /* notifyComplete */);
                }

                int64_t positionUs;
                if (!msg->findInt64("positionUs", &positionUs)) {
                    positionUs = mPreviousSeekTimeUs;
                }
                performSeek(positionUs);

                if (reason == Renderer::kDueToError && needsToCreateAudioDecoder) {
                    instantiateDecoder(true /* audio */, &mAudioDecoder);
                }
            }
            break;
        }

        case kWhatMoreDataQueued:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatMoreDataQueued");
#endif
            break;
        }

        case kWhatReset:
        {
            ALOGV("kWhatReset");
            mDeferredActions.push_back(
                    new FlushDecoderAction(
                        FLUSH_CMD_SHUTDOWN /* audio */,
                        FLUSH_CMD_SHUTDOWN /* video */));

            mDeferredActions.push_back(
                    new SimpleAction(&NuPlayer::performReset));

            processDeferredActions();
            break;
        }

        case kWhatSeek:
        {
            int64_t seekTimeUs;
            int32_t needNotify;
            CHECK(msg->findInt64("seekTimeUs", &seekTimeUs));
            CHECK(msg->findInt32("needNotify", &needNotify));

            ALOGV("kWhatSeek seekTimeUs=%lld us, needNotify=%d",
                    (long long)seekTimeUs, needNotify);

            if (!mStarted) {
                // Seek before the player is started. In order to preview video,
                // need to start the player and pause it. This branch is called
                // only once if needed. After the player is started, any seek
                // operation will go through normal path.
                // Audio-only cases are handled separately.
                onStart(seekTimeUs);
                if (mStarted) {
                    onPause();
                    mPausedByClient = true;
                }
                if (needNotify) {
                    notifyDriverSeekComplete();
                }
                break;
            }
            mDeferredActions.push_back(
                    new FlushDecoderAction(FLUSH_CMD_FLUSH /* audio */,
                                           FLUSH_CMD_FLUSH /* video */));
            mDeferredActions.push_back(
                    new SeekAction(seekTimeUs));

            // After a flush without shutdown, decoder is paused.
            // Don't resume it until source seek is done, otherwise it could
            // start pulling stale data too soon.
            mDeferredActions.push_back(
                    new ResumeDecoderAction(needNotify));

            processDeferredActions();
            break;
        }

        case kWhatPause:
        {
            onPause();
            mPausedByClient = true;
            break;
        }

        case kWhatSourceNotify:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatSourceNotify");
            ALOGD("CountFlag:kWhatSourceNotify timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            onSourceNotify(msg);
            break;
        }

        case kWhatClosedCaptionNotify:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("kWhatClosedCaptionNotify");
            ALOGD("CountFlag:kWhatClosedCaptionNotify timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            onClosedCaptionNotify(msg);
            break;
        }
#if defined (RTK_PLATFORM) && defined (RTK_NET_ASYNC_PLAYBACK_FLOW)
        case kWhatHttpCheck:
        {
            AString url;
            KeyedVector<String8, String8> headers;

            msg->findPointer("headers", (void **)&headers);
            msg->findString("url", &url);
            setDataSourceAsyncHttpCheck(mHttpService, url.c_str(), &headers);
            break;
        }
#endif

        default:
            TRESPASS();
            break;
    }
}

void NuPlayer::onResume() {
    if (!mPaused) {
        return;
    }
    mPaused = false;
    if (mSource != NULL) {
        mSource->resume();
    } else {
        ALOGW("resume called when source is gone or not set");
    }
    // |mAudioDecoder| may have been released due to the pause timeout, so re-create it if
    // needed.
    if (audioDecoderStillNeeded() && mAudioDecoder == NULL) {
        instantiateDecoder(true /* audio */, &mAudioDecoder);
    }
    if (mRenderer != NULL) {
        mRenderer->resume();
    } else {
        ALOGW("resume called when renderer is gone or not set");
    }
}

status_t NuPlayer::onInstantiateSecureDecoders() {
    status_t err;
    if (!(mSourceFlags & Source::FLAG_SECURE)) {
        return BAD_TYPE;
    }

    if (mRenderer != NULL) {
        ALOGE("renderer should not be set when instantiating secure decoders");
        return UNKNOWN_ERROR;
    }

    // TRICKY: We rely on mRenderer being null, so that decoder does not start requesting
    // data on instantiation.
    if (mSurface != NULL) {
        err = instantiateDecoder(false, &mVideoDecoder);
        if (err != OK) {
            return err;
        }
    }

    if (mAudioSink != NULL) {
        err = instantiateDecoder(true, &mAudioDecoder);
        if (err != OK) {
            return err;
        }
    }
    return OK;
}

void NuPlayer::onStart(int64_t startPositionUs) {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    int64_t startTime = ALooper::GetNowUs();
#endif
    if (!mSourceStarted) {
        mSourceStarted = true;
        mSource->start();
    }
    if (startPositionUs > 0) {
        performSeek(startPositionUs);
        if (mSource->getFormat(false /* audio */) == NULL) {
            return;
        }
    }

    mOffloadAudio = false;
    mAudioEOS = false;
    mVideoEOS = false;
    mStarted = true;

    uint32_t flags = 0;

    if (mSource->isRealTime()) {
        flags |= Renderer::FLAG_REAL_TIME;
    }

    sp<MetaData> audioMeta = mSource->getFormatMeta(true /* audio */);
    audio_stream_type_t streamType = AUDIO_STREAM_MUSIC;
    if (mAudioSink != NULL) {
        streamType = mAudioSink->getAudioStreamType();
    }

    sp<AMessage> videoFormat = mSource->getFormat(false /* audio */);

    mOffloadAudio =
        canOffloadStream(audioMeta, (videoFormat != NULL), mSource->isStreaming(), streamType);
    if (mOffloadAudio) {
        flags |= Renderer::FLAG_OFFLOAD_AUDIO;
    }

    sp<AMessage> notify = new AMessage(kWhatRendererNotify, this);
    ++mRendererGeneration;
    notify->setInt32("generation", mRendererGeneration);
    mRenderer = new Renderer(mAudioSink, notify, flags);
    mRendererLooper = new ALooper;
    mRendererLooper->setName("NuPlayerRenderer");
    mRendererLooper->start(false, false, ANDROID_PRIORITY_AUDIO);
    mRendererLooper->registerHandler(mRenderer);

    status_t err = mRenderer->setPlaybackSettings(mPlaybackSettings);
    if (err != OK) {
        mSource->stop();
        mSourceStarted = false;
        notifyListener(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, err);
        return;
    }

    float rate = getFrameRate();
    if (rate > 0) {
        mRenderer->setVideoFrameRate(rate);
    }

    if (mVideoDecoder != NULL) {
        mVideoDecoder->setRenderer(mRenderer);
    }
    if (mAudioDecoder != NULL) {
        mAudioDecoder->setRenderer(mRenderer);
    }

    postScanSources();
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
    ALOGD("CountDuration:NuPlayer_onStart timeUs: %" PRId64, ALooper::GetNowUs() - startTime);
#endif

#ifdef RTK_PLATFORM
    mRenderer->setStartPBDropFrame(mIsM3u8LiveFlg);
#endif
}

void NuPlayer::onPause() {
    if (mPaused) {
        return;
    }
    mPaused = true;
    if (mSource != NULL) {
        mSource->pause();
    } else {
        ALOGW("pause called when source is gone or not set");
    }
    if (mRenderer != NULL) {
        mRenderer->pause();
    } else {
        ALOGW("pause called when renderer is gone or not set");
    }
}

bool NuPlayer::audioDecoderStillNeeded() {
    // Audio decoder is no longer needed if it's in shut/shutting down status.
    return ((mFlushingAudio != SHUT_DOWN) && (mFlushingAudio != SHUTTING_DOWN_DECODER));
}

void NuPlayer::handleFlushComplete(bool audio, bool isDecoder) {
    // We wait for both the decoder flush and the renderer flush to complete
    // before entering either the FLUSHED or the SHUTTING_DOWN_DECODER state.

    mFlushComplete[audio][isDecoder] = true;
    if (!mFlushComplete[audio][!isDecoder]) {
        return;
    }

    FlushStatus *state = audio ? &mFlushingAudio : &mFlushingVideo;
    switch (*state) {
        case FLUSHING_DECODER:
        {
            *state = FLUSHED;
            break;
        }

        case FLUSHING_DECODER_SHUTDOWN:
        {
            *state = SHUTTING_DOWN_DECODER;

            ALOGV("initiating %s decoder shutdown", audio ? "audio" : "video");
            if (!audio) {
                // Widevine source reads must stop before releasing the video decoder.
                if (mSource != NULL && mSourceFlags & Source::FLAG_SECURE) {
                    mSource->stop();
                    mSourceStarted = false;
                }
            }
            getDecoder(audio)->initiateShutdown();
            break;
        }

        default:
            // decoder flush completes only occur in a flushing state.
            LOG_ALWAYS_FATAL_IF(isDecoder, "decoder flush in invalid state %d", *state);
            break;
    }
}

void NuPlayer::finishFlushIfPossible() {
    if (mFlushingAudio != NONE && mFlushingAudio != FLUSHED
            && mFlushingAudio != SHUT_DOWN) {
        return;
    }

    if (mFlushingVideo != NONE && mFlushingVideo != FLUSHED
            && mFlushingVideo != SHUT_DOWN) {
        return;
    }

    ALOGV("both audio and video are flushed now.");

    mFlushingAudio = NONE;
    mFlushingVideo = NONE;

    clearFlushComplete();

    processDeferredActions();
}

void NuPlayer::postScanSources() {
    if (mScanSourcesPending) {
        return;
    }

    sp<AMessage> msg = new AMessage(kWhatScanSources, this);
    msg->setInt32("generation", mScanSourcesGeneration);
    msg->post();

    mScanSourcesPending = true;
}

void NuPlayer::tryOpenAudioSinkForOffload(const sp<AMessage> &format, bool hasVideo) {
    // Note: This is called early in NuPlayer to determine whether offloading
    // is possible; otherwise the decoders call the renderer openAudioSink directly.

    status_t err = mRenderer->openAudioSink(
            format, true /* offloadOnly */, hasVideo, AUDIO_OUTPUT_FLAG_NONE, &mOffloadAudio);
    if (err != OK) {
        // Any failure we turn off mOffloadAudio.
        mOffloadAudio = false;
    } else if (mOffloadAudio) {
        sp<MetaData> audioMeta =
                mSource->getFormatMeta(true /* audio */);
        sendMetaDataToHal(mAudioSink, audioMeta);
    }
}

void NuPlayer::closeAudioSink() {
    mRenderer->closeAudioSink();
}

void NuPlayer::determineAudioModeChange() {
    if (mSource == NULL || mAudioSink == NULL) {
        return;
    }

    if (mRenderer == NULL) {
        ALOGW("No renderer can be used to determine audio mode. Use non-offload for safety.");
        mOffloadAudio = false;
        return;
    }

    sp<MetaData> audioMeta = mSource->getFormatMeta(true /* audio */);
    sp<AMessage> videoFormat = mSource->getFormat(false /* audio */);
    audio_stream_type_t streamType = mAudioSink->getAudioStreamType();
    const bool hasVideo = (videoFormat != NULL);
    const bool canOffload = canOffloadStream(
            audioMeta, hasVideo, mSource->isStreaming(), streamType);
    if (canOffload) {
        if (!mOffloadAudio) {
            mRenderer->signalEnableOffloadAudio();
        }
        // open audio sink early under offload mode.
        sp<AMessage> format = mSource->getFormat(true /*audio*/);
        tryOpenAudioSinkForOffload(format, hasVideo);
    } else {
        if (mOffloadAudio) {
            mRenderer->signalDisableOffloadAudio();
            mOffloadAudio = false;
        }
    }
}

status_t NuPlayer::instantiateDecoder(bool audio, sp<DecoderBase> *decoder) {
    // The audio decoder could be cleared by tear down. If still in shut down
    // process, no need to create a new audio decoder.
    if (*decoder != NULL || (audio && mFlushingAudio == SHUT_DOWN)) {
        return OK;
    }

    sp<AMessage> format = mSource->getFormat(audio);

    if (format == NULL) {
        return -EWOULDBLOCK;
    }

    format->setInt32("priority", 0 /* realtime */);

    if (!audio) {
        AString mime;
        CHECK(format->findString("mime", &mime));

        sp<AMessage> ccNotify = new AMessage(kWhatClosedCaptionNotify, this);
        if (mCCDecoder == NULL) {
            mCCDecoder = new CCDecoder(ccNotify);
        }

        if (mSourceFlags & Source::FLAG_SECURE) {
            format->setInt32("secure", true);
        }

        if (mSourceFlags & Source::FLAG_PROTECTED) {
            format->setInt32("protected", true);
        }

        float rate = getFrameRate();
        if (rate > 0) {
            format->setFloat("operating-rate", rate * mPlaybackSettings.mSpeed);
        }
    }

    if (audio) {
        sp<AMessage> notify = new AMessage(kWhatAudioNotify, this);
        ++mAudioDecoderGeneration;
        notify->setInt32("generation", mAudioDecoderGeneration);

        determineAudioModeChange();
        if (mOffloadAudio) {
            mSource->setOffloadAudio(true /* offload */);
            const bool hasVideo = (mSource->getFormat(false /*audio */) != NULL);
            format->setInt32("has-video", hasVideo);
            *decoder = new DecoderPassThrough(notify, mSource, mRenderer);
        } else {
            *decoder = new Decoder(notify, mSource, mPID, mRenderer);
        }
    } else {
        sp<AMessage> notify = new AMessage(kWhatVideoNotify, this);
        ++mVideoDecoderGeneration;
        notify->setInt32("generation", mVideoDecoderGeneration);

        *decoder = new Decoder(
                notify, mSource, mPID, mRenderer, mSurface, mCCDecoder);

        // enable FRC if high-quality AV sync is requested, even if not
        // directly queuing to display, as this will even improve textureview
        // playback.
        {
            char value[PROPERTY_VALUE_MAX];
            if (property_get("persist.sys.media.avsync", value, NULL) &&
                    (!strcmp("1", value) || !strcasecmp("true", value))) {
                format->setInt32("auto-frc", 1);
            }
        }
    }
    (*decoder)->init();
    (*decoder)->configure(format);

    // allocate buffers to decrypt widevine source buffers
    if (!audio && (mSourceFlags & Source::FLAG_SECURE)) {
        Vector<sp<ABuffer> > inputBufs;
        CHECK_EQ((*decoder)->getInputBuffers(&inputBufs), (status_t)OK);

        Vector<MediaBuffer *> mediaBufs;
        for (size_t i = 0; i < inputBufs.size(); i++) {
            const sp<ABuffer> &buffer = inputBufs[i];
            MediaBuffer *mbuf = new MediaBuffer(buffer->data(), buffer->size());
            mediaBufs.push(mbuf);
        }

        status_t err = mSource->setBuffers(audio, mediaBufs);
        if (err != OK) {
            for (size_t i = 0; i < mediaBufs.size(); ++i) {
                mediaBufs[i]->release();
            }
            mediaBufs.clear();
            ALOGE("Secure source didn't support secure mediaBufs.");
            return err;
        }
    }
    return OK;
}

void NuPlayer::updateVideoSize(
        const sp<AMessage> &inputFormat,
        const sp<AMessage> &outputFormat) {
    if (inputFormat == NULL) {
        ALOGW("Unknown video size, reporting 0x0!");
        notifyListener(MEDIA_SET_VIDEO_SIZE, 0, 0);
        return;
    }

    int32_t displayWidth, displayHeight;
    if (outputFormat != NULL) {
        int32_t width, height;
        CHECK(outputFormat->findInt32("width", &width));
        CHECK(outputFormat->findInt32("height", &height));

        int32_t cropLeft, cropTop, cropRight, cropBottom;
        CHECK(outputFormat->findRect(
                    "crop",
                    &cropLeft, &cropTop, &cropRight, &cropBottom));

        displayWidth = cropRight - cropLeft + 1;
        displayHeight = cropBottom - cropTop + 1;

        ALOGV("Video output format changed to %d x %d "
             "(crop: %d x %d @ (%d, %d))",
             width, height,
             displayWidth,
             displayHeight,
             cropLeft, cropTop);
    } else {
        CHECK(inputFormat->findInt32("width", &displayWidth));
        CHECK(inputFormat->findInt32("height", &displayHeight));

        ALOGV("Video input format %d x %d", displayWidth, displayHeight);
    }

    // Take into account sample aspect ratio if necessary:
    int32_t sarWidth, sarHeight;
    if (inputFormat->findInt32("sar-width", &sarWidth)
            && inputFormat->findInt32("sar-height", &sarHeight)) {
        ALOGV("Sample aspect ratio %d : %d", sarWidth, sarHeight);

        displayWidth = (displayWidth * sarWidth) / sarHeight;

        ALOGV("display dimensions %d x %d", displayWidth, displayHeight);
    }

    int32_t rotationDegrees;
    if (!inputFormat->findInt32("rotation-degrees", &rotationDegrees)) {
        rotationDegrees = 0;
    }

    if (rotationDegrees == 90 || rotationDegrees == 270) {
        int32_t tmp = displayWidth;
        displayWidth = displayHeight;
        displayHeight = tmp;
    }

    notifyListener(
            MEDIA_SET_VIDEO_SIZE,
            displayWidth,
            displayHeight);
}

void NuPlayer::notifyListener(int msg, int ext1, int ext2, const Parcel *in) {
    if (mDriver == NULL) {
        return;
    }

    sp<NuPlayerDriver> driver = mDriver.promote();

    if (driver == NULL) {
        return;
    }

    driver->notifyListener(msg, ext1, ext2, in);
}

void NuPlayer::flushDecoder(bool audio, bool needShutdown) {
    ALOGV("[%s] flushDecoder needShutdown=%d",
          audio ? "audio" : "video", needShutdown);

    const sp<DecoderBase> &decoder = getDecoder(audio);
    if (decoder == NULL) {
        ALOGI("flushDecoder %s without decoder present",
             audio ? "audio" : "video");
        return;
    }

    // Make sure we don't continue to scan sources until we finish flushing.
    ++mScanSourcesGeneration;
    if (mScanSourcesPending) {
        mDeferredActions.push_back(
                new SimpleAction(&NuPlayer::performScanSources));
        mScanSourcesPending = false;
    }

    decoder->signalFlush();

    FlushStatus newStatus =
        needShutdown ? FLUSHING_DECODER_SHUTDOWN : FLUSHING_DECODER;

    mFlushComplete[audio][false /* isDecoder */] = (mRenderer == NULL);
    mFlushComplete[audio][true /* isDecoder */] = false;
    if (audio) {
        ALOGE_IF(mFlushingAudio != NONE,
                "audio flushDecoder() is called in state %d", mFlushingAudio);
        mFlushingAudio = newStatus;
    } else {
        ALOGE_IF(mFlushingVideo != NONE,
                "video flushDecoder() is called in state %d", mFlushingVideo);
        mFlushingVideo = newStatus;
    }
}

void NuPlayer::queueDecoderShutdown(
        bool audio, bool video, const sp<AMessage> &reply) {
    ALOGI("queueDecoderShutdown audio=%d, video=%d", audio, video);

    mDeferredActions.push_back(
            new FlushDecoderAction(
                audio ? FLUSH_CMD_SHUTDOWN : FLUSH_CMD_NONE,
                video ? FLUSH_CMD_SHUTDOWN : FLUSH_CMD_NONE));

    mDeferredActions.push_back(
            new SimpleAction(&NuPlayer::performScanSources));

    mDeferredActions.push_back(new PostMessageAction(reply));

    processDeferredActions();
}

status_t NuPlayer::setVideoScalingMode(int32_t mode) {
    mVideoScalingMode = mode;
    if (mSurface != NULL) {
        status_t ret = native_window_set_scaling_mode(mSurface.get(), mVideoScalingMode);
        if (ret != OK) {
            ALOGE("Failed to set scaling mode (%d): %s",
                -ret, strerror(-ret));
            return ret;
        }
    }
    return OK;
}

status_t NuPlayer::getTrackInfo(Parcel* reply) const {
    sp<AMessage> msg = new AMessage(kWhatGetTrackInfo, this);
    msg->setPointer("reply", reply);

    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    return err;
}

status_t NuPlayer::getSelectedTrack(int32_t type, Parcel* reply) const {
    sp<AMessage> msg = new AMessage(kWhatGetSelectedTrack, this);
    msg->setPointer("reply", reply);
    msg->setInt32("type", type);

    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);
    if (err == OK && response != NULL) {
        CHECK(response->findInt32("err", &err));
    }
    return err;
}

status_t NuPlayer::selectTrack(size_t trackIndex, bool select, int64_t timeUs) {
    sp<AMessage> msg = new AMessage(kWhatSelectTrack, this);
    msg->setSize("trackIndex", trackIndex);
    msg->setInt32("select", select);
    msg->setInt64("timeUs", timeUs);

    sp<AMessage> response;
    status_t err = msg->postAndAwaitResponse(&response);

    if (err != OK) {
        return err;
    }

    if (!response->findInt32("err", &err)) {
        err = OK;
    }

    return err;
}

status_t NuPlayer::getCurrentPosition(int64_t *mediaUs) {
    sp<Renderer> renderer = mRenderer;
    if (renderer == NULL) {
        return NO_INIT;
    }

    return renderer->getCurrentPosition(mediaUs);
}

void NuPlayer::getStats(Vector<sp<AMessage> > *mTrackStats) {
    CHECK(mTrackStats != NULL);

    mTrackStats->clear();
    if (mVideoDecoder != NULL) {
        mTrackStats->push_back(mVideoDecoder->getStats());
    }
    if (mAudioDecoder != NULL) {
        mTrackStats->push_back(mAudioDecoder->getStats());
    }
}

sp<MetaData> NuPlayer::getFileMeta() {
    return mSource->getFileFormatMeta();
}

float NuPlayer::getFrameRate() {
    sp<MetaData> meta = mSource->getFormatMeta(false /* audio */);
    if (meta == NULL) {
        return 0;
    }
#ifdef RTK_PLATFORM
    float rateFloat;
    if (meta->findFloat(kKeyFrameRateFloat, &rateFloat)) {
        // fall back to try file meta
        sp<MetaData> fileMeta = getFileMeta();
        float fileMetaRateFloat;
        if (fileMeta != NULL && fileMeta->findFloat(kKeyFrameRateFloat, &fileMetaRateFloat)) {
            return fileMetaRateFloat;
        }
        return rateFloat;
    }
#endif
    int32_t rate;
    if (!meta->findInt32(kKeyFrameRate, &rate)) {
        // fall back to try file meta
        sp<MetaData> fileMeta = getFileMeta();
        if (fileMeta == NULL) {
            ALOGW("source has video meta but not file meta");
            return -1;
        }
        int32_t fileMetaRate;
        if (!fileMeta->findInt32(kKeyFrameRate, &fileMetaRate)) {
            return -1;
        }
        return fileMetaRate;
    }
    return rate;
}

void NuPlayer::schedulePollDuration() {
    sp<AMessage> msg = new AMessage(kWhatPollDuration, this);
    msg->setInt32("generation", mPollDurationGeneration);
    msg->post();
}

void NuPlayer::cancelPollDuration() {
    ++mPollDurationGeneration;
}

void NuPlayer::processDeferredActions() {
    while (!mDeferredActions.empty()) {
        // We won't execute any deferred actions until we're no longer in
        // an intermediate state, i.e. one more more decoders are currently
        // flushing or shutting down.

        if (mFlushingAudio != NONE || mFlushingVideo != NONE) {
            // We're currently flushing, postpone the reset until that's
            // completed.

            ALOGV("postponing action mFlushingAudio=%d, mFlushingVideo=%d",
                  mFlushingAudio, mFlushingVideo);

            break;
        }

        sp<Action> action = *mDeferredActions.begin();
        mDeferredActions.erase(mDeferredActions.begin());

        action->execute(this);
    }
}

void NuPlayer::performSeek(int64_t seekTimeUs) {
    ALOGV("performSeek seekTimeUs=%lld us (%.2f secs)",
          (long long)seekTimeUs,
          seekTimeUs / 1E6);

    if (mSource == NULL) {
        // This happens when reset occurs right before the loop mode
        // asynchronously seeks to the start of the stream.
        LOG_ALWAYS_FATAL_IF(mAudioDecoder != NULL || mVideoDecoder != NULL,
                "mSource is NULL and decoders not NULL audio(%p) video(%p)",
                mAudioDecoder.get(), mVideoDecoder.get());
        return;
    }
#ifdef RTK_PLATFORM
    if(mIsHttpFlw == true)
    {
        //Source::kWhatPauseOnBufferingStart:
        ALOGI("[%s][%d]buffer low, pausing...",__FUNCTION__,__LINE__);
        notifyListener(MEDIA_INFO, MEDIA_INFO_BUFFERING_START, 0);
    }
#endif
    mPreviousSeekTimeUs = seekTimeUs;
    mSource->seekTo(seekTimeUs);
    ++mTimedTextGeneration;
#ifdef RTK_PLATFORM
    if(mIsHttpFlw == true)
    {
        //Source::kWhatResumeOnBufferingEnd:
        ALOGI("[%s][%d]buffer ready, resuming...",__FUNCTION__,__LINE__);
        notifyListener(MEDIA_INFO, MEDIA_INFO_BUFFERING_END, 0);
    }
#endif

    // everything's flushed, continue playback.
}

void NuPlayer::performDecoderFlush(FlushCommand audio, FlushCommand video) {
    ALOGV("performDecoderFlush audio=%d, video=%d", audio, video);

    if ((audio == FLUSH_CMD_NONE || mAudioDecoder == NULL)
            && (video == FLUSH_CMD_NONE || mVideoDecoder == NULL)) {
        return;
    }

    if (audio != FLUSH_CMD_NONE && mAudioDecoder != NULL) {
        flushDecoder(true /* audio */, (audio == FLUSH_CMD_SHUTDOWN));
    }

    if (video != FLUSH_CMD_NONE && mVideoDecoder != NULL) {
        flushDecoder(false /* audio */, (video == FLUSH_CMD_SHUTDOWN));
    }
}

void NuPlayer::performReset() {
    ALOGV("performReset");

    CHECK(mAudioDecoder == NULL);
    CHECK(mVideoDecoder == NULL);

    cancelPollDuration();

    ++mScanSourcesGeneration;
    mScanSourcesPending = false;

    if (mRendererLooper != NULL) {
        if (mRenderer != NULL) {
            mRendererLooper->unregisterHandler(mRenderer->id());
        }
        mRendererLooper->stop();
        mRendererLooper.clear();
    }
    mRenderer.clear();
    ++mRendererGeneration;

    if (mSource != NULL) {
        mSource->stop();

        mSource.clear();
    }

    if (mDriver != NULL) {
        sp<NuPlayerDriver> driver = mDriver.promote();
        if (driver != NULL) {
            driver->notifyResetComplete();
        }
    }

    mStarted = false;
    mSourceStarted = false;
#ifdef RTK_NET_ASYNC_PLAYBACK_FLOW
    mAsyncIsResetFlg = false;
#endif
}

void NuPlayer::performScanSources() {
    ALOGV("performScanSources");

    if (!mStarted) {
        return;
    }

    if (mAudioDecoder == NULL || mVideoDecoder == NULL) {
        postScanSources();
    }
}

void NuPlayer::performSetSurface(const sp<Surface> &surface) {
    ALOGV("performSetSurface");

    mSurface = surface;

    // XXX - ignore error from setVideoScalingMode for now
    setVideoScalingMode(mVideoScalingMode);

    if (mDriver != NULL) {
        sp<NuPlayerDriver> driver = mDriver.promote();
        if (driver != NULL) {
            driver->notifySetSurfaceComplete();
        }
    }
}

void NuPlayer::performResumeDecoders(bool needNotify) {
    if (needNotify) {
        mResumePending = true;
        if (mVideoDecoder == NULL) {
            // if audio-only, we can notify seek complete now,
            // as the resume operation will be relatively fast.
            finishResume();
        }
    }

    if (mVideoDecoder != NULL) {
        // When there is continuous seek, MediaPlayer will cache the seek
        // position, and send down new seek request when previous seek is
        // complete. Let's wait for at least one video output frame before
        // notifying seek complete, so that the video thumbnail gets updated
        // when seekbar is dragged.
        mVideoDecoder->signalResume(needNotify);
    }

    if (mAudioDecoder != NULL) {
        mAudioDecoder->signalResume(false /* needNotify */);
    }
}

void NuPlayer::finishResume() {
    if (mResumePending) {
        mResumePending = false;
        notifyDriverSeekComplete();
    }
}

void NuPlayer::notifyDriverSeekComplete() {
    if (mDriver != NULL) {
        sp<NuPlayerDriver> driver = mDriver.promote();
        if (driver != NULL) {
            driver->notifySeekComplete();
        }
    }
}

void NuPlayer::onSourceNotify(const sp<AMessage> &msg) {
    int32_t what;
    CHECK(msg->findInt32("what", &what));

    switch (what) {
        case Source::kWhatInstantiateSecureDecoders:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:kWhatInstantiateSecureDecoders timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            if (mSource == NULL) {
                // This is a stale notification from a source that was
                // asynchronously preparing when the client called reset().
                // We handled the reset, the source is gone.
                break;
            }
            sp<AMessage> reply;
            CHECK(msg->findMessage("reply", &reply));
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            int64_t onInstantiateSecureDecoders_start = ALooper::GetNowUs();
#endif
            status_t err = onInstantiateSecureDecoders();
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:onInstantiateSecureDecoders timeUs: %" PRId64, ALooper::GetNowUs() - onInstantiateSecureDecoders_start);
#endif
            reply->setInt32("err", err);
            reply->post();
            break;
        }

        case Source::kWhatPrepared:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:Source_kWhatPrepared timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
            int64_t kWhatPrepared_start = ALooper::GetNowUs();
#endif
            if (mSource == NULL) {
                // This is a stale notification from a source that was
                // asynchronously preparing when the client called reset().
                // We handled the reset, the source is gone.
                break;
            }

            int32_t err;
            CHECK(msg->findInt32("err", &err));

            if (err != OK) {
                // shut down potential secure codecs in case client never calls reset
                mDeferredActions.push_back(
                        new FlushDecoderAction(FLUSH_CMD_SHUTDOWN /* audio */,
                                               FLUSH_CMD_SHUTDOWN /* video */));
                processDeferredActions();
            }

            sp<NuPlayerDriver> driver = mDriver.promote();
            if (driver != NULL) {
                // notify duration first, so that it's definitely set when
                // the app received the "prepare complete" callback.
                int64_t durationUs;
                if (mSource->getDuration(&durationUs) == OK) {
                    driver->notifyDuration(durationUs);
                }
                driver->notifyPrepareCompleted(err);
            }
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountDuration:Source_kWhatPrepared timeUs: %" PRId64, ALooper::GetNowUs() - kWhatPrepared_start);
#endif
            break;
        }

        case Source::kWhatFlagsChanged:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:Source_kWhatFlagsChanged timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            uint32_t flags;
            CHECK(msg->findInt32("flags", (int32_t *)&flags));

            sp<NuPlayerDriver> driver = mDriver.promote();
            if (driver != NULL) {
                if ((flags & NuPlayer::Source::FLAG_CAN_SEEK) == 0) {
                    driver->notifyListener(
                            MEDIA_INFO, MEDIA_INFO_NOT_SEEKABLE, 0);
                }
                driver->notifyFlagsChanged(flags);
            }

            if ((mSourceFlags & Source::FLAG_DYNAMIC_DURATION)
                    && (!(flags & Source::FLAG_DYNAMIC_DURATION))) {
                cancelPollDuration();
            } else if (!(mSourceFlags & Source::FLAG_DYNAMIC_DURATION)
                    && (flags & Source::FLAG_DYNAMIC_DURATION)
                    && (mAudioDecoder != NULL || mVideoDecoder != NULL)) {
                schedulePollDuration();
            }

            mSourceFlags = flags;
            break;
        }

        case Source::kWhatVideoSizeChanged:
        {
#if defined (RTK_PLATFORM) && defined (RTK_NUPLAYER_TIME_LOG)
            ALOGD("CountFlag:Source_kWhatVideoSizeChanged timeUs: %" PRId64, ALooper::GetNowUs() - mStartTime);
#endif
            sp<AMessage> format;
            CHECK(msg->findMessage("format", &format));

            updateVideoSize(format);
            break;
        }

        case Source::kWhatBufferingUpdate:
        {
            int32_t percentage;
            CHECK(msg->findInt32("percentage", &percentage));

            notifyListener(MEDIA_BUFFERING_UPDATE, percentage, 0);
            break;
        }

        case Source::kWhatPauseOnBufferingStart:
        {
            // ignore if not playing
            if (mStarted) {
                ALOGI("buffer low, pausing...");

                mPausedForBuffering = true;
                onPause();
            }
            // fall-thru
        }

        case Source::kWhatBufferingStart:
        {
            notifyListener(MEDIA_INFO, MEDIA_INFO_BUFFERING_START, 0);
            break;
        }

        case Source::kWhatResumeOnBufferingEnd:
        {
            // ignore if not playing
            if (mStarted) {
                ALOGI("buffer ready, resuming...");

                mPausedForBuffering = false;

                // do not resume yet if client didn't unpause
                if (!mPausedByClient) {
                    onResume();
                }
            }
            // fall-thru
        }

        case Source::kWhatBufferingEnd:
        {
            notifyListener(MEDIA_INFO, MEDIA_INFO_BUFFERING_END, 0);
            break;
        }

        case Source::kWhatCacheStats:
        {
            int32_t kbps;
            CHECK(msg->findInt32("bandwidth", &kbps));

            notifyListener(MEDIA_INFO, MEDIA_INFO_NETWORK_BANDWIDTH, kbps);
            break;
        }

        case Source::kWhatSubtitleData:
        {
            sp<ABuffer> buffer;
            CHECK(msg->findBuffer("buffer", &buffer));

            sendSubtitleData(buffer, 0 /* baseIndex */);
            break;
        }

        case Source::kWhatTimedMetaData:
        {
            sp<ABuffer> buffer;
            if (!msg->findBuffer("buffer", &buffer)) {
                notifyListener(MEDIA_INFO, MEDIA_INFO_METADATA_UPDATE, 0);
            } else {
                sendTimedMetaData(buffer);
            }
            break;
        }

        case Source::kWhatTimedTextData:
        {
            int32_t generation;
            if (msg->findInt32("generation", &generation)
                    && generation != mTimedTextGeneration) {
                break;
            }

            sp<ABuffer> buffer;
            CHECK(msg->findBuffer("buffer", &buffer));

            sp<NuPlayerDriver> driver = mDriver.promote();
            if (driver == NULL) {
                break;
            }

            int posMs;
            int64_t timeUs, posUs;
            driver->getCurrentPosition(&posMs);
            posUs = posMs * 1000;
            CHECK(buffer->meta()->findInt64("timeUs", &timeUs));

            if (posUs < timeUs) {
                if (!msg->findInt32("generation", &generation)) {
                    msg->setInt32("generation", mTimedTextGeneration);
                }
                msg->post(timeUs - posUs);
            } else {
                sendTimedTextData(buffer);
            }
            break;
        }

        case Source::kWhatQueueDecoderShutdown:
        {
            int32_t audio, video;
            CHECK(msg->findInt32("audio", &audio));
            CHECK(msg->findInt32("video", &video));

            sp<AMessage> reply;
            CHECK(msg->findMessage("reply", &reply));

            queueDecoderShutdown(audio, video, reply);
            break;
        }

        case Source::kWhatDrmNoLicense:
        {
            notifyListener(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, ERROR_DRM_NO_LICENSE);
            break;
        }

        default:
            TRESPASS();
    }
}

void NuPlayer::onClosedCaptionNotify(const sp<AMessage> &msg) {
    int32_t what;
    CHECK(msg->findInt32("what", &what));

    switch (what) {
        case NuPlayer::CCDecoder::kWhatClosedCaptionData:
        {
            sp<ABuffer> buffer;
            CHECK(msg->findBuffer("buffer", &buffer));

            size_t inbandTracks = 0;
            if (mSource != NULL) {
                inbandTracks = mSource->getTrackCount();
            }

            sendSubtitleData(buffer, inbandTracks);
            break;
        }

        case NuPlayer::CCDecoder::kWhatTrackAdded:
        {
            notifyListener(MEDIA_INFO, MEDIA_INFO_METADATA_UPDATE, 0);

            break;
        }

        default:
            TRESPASS();
    }


}

void NuPlayer::sendSubtitleData(const sp<ABuffer> &buffer, int32_t baseIndex) {
    int32_t trackIndex;
    int64_t timeUs, durationUs;
    CHECK(buffer->meta()->findInt32("trackIndex", &trackIndex));
    CHECK(buffer->meta()->findInt64("timeUs", &timeUs));
    CHECK(buffer->meta()->findInt64("durationUs", &durationUs));

    Parcel in;
    in.writeInt32(trackIndex + baseIndex);
    in.writeInt64(timeUs);
    in.writeInt64(durationUs);
    in.writeInt32(buffer->size());
    in.writeInt32(buffer->size());
    in.write(buffer->data(), buffer->size());

    notifyListener(MEDIA_SUBTITLE_DATA, 0, 0, &in);
}

void NuPlayer::sendTimedMetaData(const sp<ABuffer> &buffer) {
    int64_t timeUs;
    CHECK(buffer->meta()->findInt64("timeUs", &timeUs));

    Parcel in;
    in.writeInt64(timeUs);
    in.writeInt32(buffer->size());
    in.writeInt32(buffer->size());
    in.write(buffer->data(), buffer->size());

    notifyListener(MEDIA_META_DATA, 0, 0, &in);
}

void NuPlayer::sendTimedTextData(const sp<ABuffer> &buffer) {
    const void *data;
    size_t size = 0;
    int64_t timeUs;
    int32_t flag = TextDescriptions::LOCAL_DESCRIPTIONS;

    AString mime;
    CHECK(buffer->meta()->findString("mime", &mime));
    CHECK(strcasecmp(mime.c_str(), MEDIA_MIMETYPE_TEXT_3GPP) == 0);

    data = buffer->data();
    size = buffer->size();

    Parcel parcel;
    if (size > 0) {
        CHECK(buffer->meta()->findInt64("timeUs", &timeUs));
        flag |= TextDescriptions::IN_BAND_TEXT_3GPP;
        TextDescriptions::getParcelOfDescriptions(
                (const uint8_t *)data, size, flag, timeUs / 1000, &parcel);
    }

    if ((parcel.dataSize() > 0)) {
        notifyListener(MEDIA_TIMED_TEXT, 0, 0, &parcel);
    } else {  // send an empty timed text
        notifyListener(MEDIA_TIMED_TEXT, 0, 0);
    }
}

#if defined (RTK_PLATFORM) && defined (RTK_NET_ASYNC_PLAYBACK_FLOW)
void NuPlayer::setResetFlgForAsyncFlw(bool flags)
{
    mAsyncIsResetFlg = flags;
    ALOGV("[%s][%d] mAsyncIsResetFlg set %d \n",__FUNCTION__,__LINE__,flags);
}
void NuPlayer::setmIsNetAsyncFlw(bool flags)
{
    mIsNetAsyncFlw = flags;
}
bool NuPlayer::chkmIsNetAsyncFlw()
{
    return mIsNetAsyncFlw;
}
#endif
#ifdef RTK_PLATFORM
bool NuPlayer::chkIsLiveFlw()
{
    return mIsLiveFlw;
}
#endif

////////////////////////////////////////////////////////////////////////////////

sp<AMessage> NuPlayer::Source::getFormat(bool audio) {
    sp<MetaData> meta = getFormatMeta(audio);

    if (meta == NULL) {
        return NULL;
    }

    sp<AMessage> msg = new AMessage;

    if(convertMetaDataToMessage(meta, &msg) == OK) {
        return msg;
    }
    return NULL;
}

void NuPlayer::Source::notifyFlagsChanged(uint32_t flags) {
    sp<AMessage> notify = dupNotify();
    notify->setInt32("what", kWhatFlagsChanged);
    notify->setInt32("flags", flags);
    notify->post();
}

void NuPlayer::Source::notifyVideoSizeChanged(const sp<AMessage> &format) {
    sp<AMessage> notify = dupNotify();
    notify->setInt32("what", kWhatVideoSizeChanged);
    notify->setMessage("format", format);
    notify->post();
}

void NuPlayer::Source::notifyPrepared(status_t err) {
    sp<AMessage> notify = dupNotify();
    notify->setInt32("what", kWhatPrepared);
    notify->setInt32("err", err);
    notify->post();
}

void NuPlayer::Source::notifyInstantiateSecureDecoders(const sp<AMessage> &reply) {
    sp<AMessage> notify = dupNotify();
    notify->setInt32("what", kWhatInstantiateSecureDecoders);
    notify->setMessage("reply", reply);
    notify->post();
}

void NuPlayer::Source::onMessageReceived(const sp<AMessage> & /* msg */) {
    TRESPASS();
}

}  // namespace android
