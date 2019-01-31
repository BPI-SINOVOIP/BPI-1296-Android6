/*
 * Copyright (C) 2009 The Android Open Source Project
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
#define LOG_TAG "StagefrightMetadataRetriever"

#include <inttypes.h>

#include <utils/Log.h>
#include <gui/Surface.h>

#include "include/StagefrightMetadataRetriever.h"

#include <media/ICrypto.h>
#include <media/IMediaHTTPService.h>

#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/ColorConverter.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaExtractor.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/Utils.h>
#include <media/stagefright/MediaDefs.h>
#include <cutils/properties.h>

#ifdef USE_RTK_EXTRACTOR
#include <media/stagefright/RTKFileSource.h>
#include "include/RTKExtractor.h"
#endif
#define CHECK_ALIGN
#define ALIGN(x, y)     ((x + y - 1) & ~(y - 1))
#define ALIGN16(x)      ALIGN(x, 16)
#define ALIGN32(x)      ALIGN(x, 32)
#define VP9_MAX_SUPPROT_RESOLUTION	4096*2160 //640*360//(width*height)

#include <CharacterEncodingDetector.h>

namespace android {

static const int64_t kBufferTimeOutUs = 30000ll; // 30 msec
static const size_t kRetryCount = 100; // must be >0

StagefrightMetadataRetriever::StagefrightMetadataRetriever()
    : mParsedMetaData(false),
      mAlbumArt(NULL) {
    ALOGV("StagefrightMetadataRetriever()");

    DataSource::RegisterDefaultSniffers();
    CHECK_EQ(mClient.connect(), (status_t)OK);
}

StagefrightMetadataRetriever::~StagefrightMetadataRetriever() {
    ALOGV("~StagefrightMetadataRetriever()");
    clearMetadata();
    mClient.disconnect();
}

status_t StagefrightMetadataRetriever::setDataSource(
        const sp<IMediaHTTPService> &httpService,
        const char *uri,
        const KeyedVector<String8, String8> *headers) {
    ALOGV("setDataSource(%s)", uri);

    clearMetadata();
    mSource = DataSource::CreateFromURI(httpService, uri, headers);

    if (mSource == NULL) {
        ALOGE("Unable to create data source for '%s'.", uri);
        return UNKNOWN_ERROR;
    }
#ifdef USE_RTK_EXTRACTOR
    char value[PROPERTY_VALUE_MAX];
    if (property_get("persist.rtk.ctstest", value, NULL) && (!strcmp(value, "1") || !strcasecmp(value, "true")))
        mSource->RTK_EXT = 0;
    else
        mSource->RTK_EXT = 0; // 0x2379;
#endif /* end of USE_RTK_EXTRACTOR */

    mExtractor = MediaExtractor::Create(mSource);

    if (mExtractor == NULL) {
        ALOGE("Unable to instantiate an extractor for '%s'.", uri);

        mSource.clear();

        return UNKNOWN_ERROR;
    }

    return OK;
}

// Warning caller retains ownership of the filedescriptor! Dup it if necessary.
status_t StagefrightMetadataRetriever::setDataSource(
        int fd, int64_t offset, int64_t length) {
    fd = dup(fd);

    ALOGV("setDataSource(%d, %" PRId64 ", %" PRId64 ")", fd, offset, length);

    clearMetadata();
    mSource = new FileSource(fd, offset, length);
#ifdef USE_RTK_EXTRACTOR
    char value[PROPERTY_VALUE_MAX];
    if (property_get("persist.rtk.ctstest", value, NULL) && (!strcmp(value, "1") || !strcasecmp(value, "true")))
        mSource->RTK_EXT = 0;
    else
        mSource->RTK_EXT = 0; // 0x2379;
#endif /* end of USE_RTK_EXTRACTOR */

    status_t err;
    if ((err = mSource->initCheck()) != OK) {
        mSource.clear();

        return err;
    }

    mExtractor = MediaExtractor::Create(mSource);

    if (mExtractor == NULL) {
        mSource.clear();

        return UNKNOWN_ERROR;
    }

    return OK;
}

status_t StagefrightMetadataRetriever::setDataSource(
        const sp<DataSource>& source) {
    ALOGV("setDataSource(DataSource)");

    clearMetadata();
    mSource = source;
    mExtractor = MediaExtractor::Create(mSource);

    if (mExtractor == NULL) {
        ALOGE("Failed to instantiate a MediaExtractor.");
        mSource.clear();
        return UNKNOWN_ERROR;
    }

    return OK;
}

static VideoFrame *extractVideoFrame(
        const char *componentName,
        const sp<MetaData> &trackMeta,
        const sp<MediaSource> &source,
        int64_t frameTimeUs,
        int seekMode) {

    sp<MetaData> format = source->getFormat();

    sp<AMessage> videoFormat;
    if (convertMetaDataToMessage(trackMeta, &videoFormat) != OK) {
        ALOGW("Failed to convert meta data to message");
        return NULL;
    }

    // TODO: Use Flexible color instead
    videoFormat->setInt32("color-format", OMX_COLOR_FormatYUV420Planar);
    // Rtk for thumbnail
    videoFormat->setInt32("isThumbnail", 1);

    status_t err;
    sp<ALooper> looper = new ALooper;
    looper->start();
    sp<MediaCodec> decoder = MediaCodec::CreateByComponentName(
            looper, componentName, &err);

    if (decoder.get() == NULL || err != OK) {
        ALOGW("Failed to instantiate decoder [%s]", componentName);
        return NULL;
    }

    err = decoder->configure(videoFormat, NULL /* surface */, NULL /* crypto */, 0 /* flags */);
    if (err != OK) {
        ALOGW("configure returned error %d (%s)", err, asString(err));
        decoder->release();
        return NULL;
    }

    err = decoder->start();
    if (err != OK) {
        ALOGW("start returned error %d (%s)", err, asString(err));
        decoder->release();
        return NULL;
    }

    MediaSource::ReadOptions options;
    if (seekMode < MediaSource::ReadOptions::SEEK_PREVIOUS_SYNC ||
        seekMode > MediaSource::ReadOptions::SEEK_CLOSEST) {

        ALOGE("Unknown seek mode: %d", seekMode);
        decoder->release();
        return NULL;
    }

    MediaSource::ReadOptions::SeekMode mode =
            static_cast<MediaSource::ReadOptions::SeekMode>(seekMode);

    int64_t thumbNailTime;
    if (frameTimeUs < 0) {
        if (!trackMeta->findInt64(kKeyThumbnailTime, &thumbNailTime)
                || thumbNailTime < 0) {
            thumbNailTime = 0;
        }
        options.setSeekTo(thumbNailTime, mode);
    } else {
        thumbNailTime = -1;
        options.setSeekTo(frameTimeUs, mode);
    }

    err = source->start();
    if (err != OK) {
        ALOGW("source failed to start: %d (%s)", err, asString(err));
        decoder->release();
        return NULL;
    }

    Vector<sp<ABuffer> > inputBuffers;
    err = decoder->getInputBuffers(&inputBuffers);
    if (err != OK) {
        ALOGW("failed to get input buffers: %d (%s)", err, asString(err));
        decoder->release();
        return NULL;
    }

    Vector<sp<ABuffer> > outputBuffers;
    err = decoder->getOutputBuffers(&outputBuffers);
    if (err != OK) {
        ALOGW("failed to get output buffers: %d (%s)", err, asString(err));
        decoder->release();
        return NULL;
    }

    sp<AMessage> outputFormat = NULL;
    bool haveMoreInputs = true;
    size_t index, offset, size;
    int64_t timeUs;
    size_t retriesLeft = kRetryCount;
    bool done = false;

    do {
        size_t inputIndex = -1;
        int64_t ptsUs = 0ll;
        uint32_t flags = 0;
        sp<ABuffer> codecBuffer = NULL;

        while (haveMoreInputs) {
            err = decoder->dequeueInputBuffer(&inputIndex, kBufferTimeOutUs);
            if (err != OK) {
                ALOGW("Timed out waiting for input");
                if (retriesLeft) {
                    err = OK;
                }
                break;
            }
            codecBuffer = inputBuffers[inputIndex];

            MediaBuffer *mediaBuffer = NULL;

            err = source->read(&mediaBuffer, &options);
            options.clearSeekTo();
            if (err != OK) {
                err = source->read(&mediaBuffer, NULL);
                if(err != OK)
                {
                    ALOGW("Input Error or EOS");
#ifdef RTK_PLATFORM
                    if (err == ERROR_END_OF_STREAM)
                    {
                        err = OK;
                        break;
                    }
#endif
                    haveMoreInputs = false;
                    break;
                }
            }

            if (mediaBuffer->range_length() > codecBuffer->capacity()) {
                ALOGE("buffer size (%zu) too large for codec input size (%zu)",
                        mediaBuffer->range_length(), codecBuffer->capacity());
                err = BAD_VALUE;
            } else {
                codecBuffer->setRange(0, mediaBuffer->range_length());

                CHECK(mediaBuffer->meta_data()->findInt64(kKeyTime, &ptsUs));
                memcpy(codecBuffer->data(),
                        (const uint8_t*)mediaBuffer->data() + mediaBuffer->range_offset(),
                        mediaBuffer->range_length());
            }

            mediaBuffer->release();
            break;
        }

        if (err == OK && inputIndex < inputBuffers.size()) {
            ALOGV("QueueInput: size=%zu ts=%" PRId64 " us flags=%x",
                    codecBuffer->size(), ptsUs, flags);
            err = decoder->queueInputBuffer(
                    inputIndex,
                    codecBuffer->offset(),
                    codecBuffer->size(),
                    ptsUs,
                    flags);

            // we don't expect an output from codec config buffer
            if (flags & MediaCodec::BUFFER_FLAG_CODECCONFIG) {
                continue;
            }
        }

        while (err == OK) {
            // wait for a decoded buffer
            err = decoder->dequeueOutputBuffer(
                    &index,
                    &offset,
                    &size,
                    &timeUs,
                    &flags,
                    kBufferTimeOutUs);

            if (err == INFO_FORMAT_CHANGED) {
                ALOGV("Received format change");
                err = decoder->getOutputFormat(&outputFormat);
            } else if (err == INFO_OUTPUT_BUFFERS_CHANGED) {
                ALOGV("Output buffers changed");
                err = decoder->getOutputBuffers(&outputBuffers);
            } else {

                if(kRetryCount-retriesLeft==20)
                    ALOGI("dequeueOutputBuffer: retry failed more than 20 times");

                if (err == -EAGAIN /* INFO_TRY_AGAIN_LATER */ && --retriesLeft > 0) {
                    ALOGV("Timed-out waiting for output.. retries left = %zu", retriesLeft);
                    err = OK;
                } else if (err == OK) {
                    ALOGV("Received an output buffer");
                    done = true;
                } else {
                    ALOGW("Received error %d (%s) instead of output", err, asString(err));
                    done = true;
                }
                break;
            }
        }
    } while (err == OK && !done);

    if (err != OK || size <= 0 || outputFormat == NULL) {
        ALOGE("Failed to decode thumbnail frame");
        source->stop();
        decoder->stop();
        decoder->release();
        return NULL;
    }

    ALOGV("successfully decoded video frame.");
    sp<ABuffer> videoFrameBuffer = outputBuffers.itemAt(index);

    if (thumbNailTime >= 0) {
        if (timeUs != thumbNailTime) {
            AString mime;
            CHECK(outputFormat->findString("mime", &mime));

            ALOGV("thumbNailTime = %lld us, timeUs = %lld us, mime = %s",
                    (long long)thumbNailTime, (long long)timeUs, mime.c_str());
        }
    }

    int32_t width, height;
    CHECK(outputFormat->findInt32("width", &width));
    CHECK(outputFormat->findInt32("height", &height));

#ifdef CHECK_ALIGN
    AString decoderName;
    bool reSize = false;
    sp<ABuffer> alignBuffer=NULL;
    if(decoder->getName(&decoderName) == OK)
    {
        if (strncmp("OMX.realtek.video.dec", decoderName.c_str(), strlen("OMX.realtek.video.dec")))
        {
            if ((width & 0xf || height & 0xf))
            {
                /* It mean need align */
                reSize = true;
            }
        }
    }
    if (reSize)
    {
        int orig_width = width;
        int orig_height = height;
        int orig_size = orig_width * orig_height;
        width = ALIGN16(width);
        height = ALIGN16(height);
        int size = width*height;
        alignBuffer = new ABuffer(size + (size >> 1));
        if (width == orig_width)
        {
            ALOGD("\033[0;35m ---- in %s:%d wxh:%dx%d alloc size:%d , Ysize:%d Yorig_size:%d--\033[m", __FUNCTION__, __LINE__, width, height, (size + (size >> 1)), size, orig_size);
            // copy Y
            memcpy((uint8_t *)alignBuffer->data(), (const uint8_t *)videoFrameBuffer->data(), orig_size);
            // copy U
            memcpy((uint8_t *)alignBuffer->data()+ size, (const uint8_t *)videoFrameBuffer->data() + orig_size, orig_size >> 2);
            // copy V
            memcpy((uint8_t *)alignBuffer->data()+ size + (size >> 2), (const uint8_t *)videoFrameBuffer->data() + orig_size + (orig_size >> 2) , orig_size >> 2);
        }
        else
        {
            ALOGD("\033[0;35m ---- in %s:%d wxh:%dx%d alloc size:%d , Ysize:%d Yorig_size:%d--\033[m", __FUNCTION__, __LINE__, width, height, (size + (size >> 1)), size, orig_size);
            int dst_y_offset = 0;
            int dst_u_offset = size;
            int dst_v_offset = size + (size >> 2);
            int src_y_offset = 0;
            int src_u_offset = orig_size;
            int src_v_offset = orig_size + (orig_size >> 2);
            int copy_uv_size = (orig_width >> 1);
            int dst_half_width = (width >> 1);
            int src_half_width = (orig_width >> 1);
            int half_height = (orig_height >> 1);
            int ii = 0;
            for (ii = 0 ; ii < orig_height ; ii++)
            {
                // copy Y
                memcpy((uint8_t *)alignBuffer->data()+ dst_y_offset, (const uint8_t *)videoFrameBuffer->data() + src_y_offset, orig_width);
                if (ii < half_height)
                {
                    // copy U
                    memcpy((uint8_t *)alignBuffer->data()+ dst_u_offset, (const uint8_t *)videoFrameBuffer->data() + src_u_offset, copy_uv_size);
                    // copy V
                    memcpy((uint8_t *)alignBuffer->data()+ dst_v_offset, (const uint8_t *)videoFrameBuffer->data() + src_v_offset, copy_uv_size);
                    dst_u_offset += dst_half_width;
                    dst_v_offset += dst_half_width;
                    src_u_offset += src_half_width;
                    src_v_offset += src_half_width;
                }
                dst_y_offset += width;
                src_y_offset += orig_width;
            }
        }
    }
#endif /* end of CHECK_ALIGN */

    int32_t crop_left, crop_top, crop_right, crop_bottom;
    if (!outputFormat->findRect("crop", &crop_left, &crop_top, &crop_right, &crop_bottom)) {
        crop_left = crop_top = 0;
        crop_right = width - 1;
        crop_bottom = height - 1;
    }

    int32_t rotationAngle;
    if (!trackMeta->findInt32(kKeyRotation, &rotationAngle)) {
        rotationAngle = 0;  // By default, no rotation
    }

    VideoFrame *frame = new VideoFrame;
    frame->mWidth = crop_right - crop_left + 1;
    frame->mHeight = crop_bottom - crop_top + 1;
    frame->mDisplayWidth = frame->mWidth;
    frame->mDisplayHeight = frame->mHeight;
    frame->mSize = frame->mWidth * frame->mHeight * 2;
    frame->mData = new uint8_t[frame->mSize];
    frame->mRotationAngle = rotationAngle;

    int32_t sarWidth, sarHeight;
    if (trackMeta->findInt32(kKeySARWidth, &sarWidth)
            && trackMeta->findInt32(kKeySARHeight, &sarHeight)
            && sarHeight != 0) {
        frame->mDisplayWidth = (frame->mDisplayWidth * sarWidth) / sarHeight;
    }

    int32_t srcFormat;
    CHECK(outputFormat->findInt32("color-format", &srcFormat));

    ColorConverter converter((OMX_COLOR_FORMATTYPE)srcFormat, OMX_COLOR_Format16bitRGB565);

    if (converter.isValid()) {
#ifdef CHECK_ALIGN
        if (alignBuffer != NULL)
        {
            err = converter.convert(
                    (const uint8_t *)alignBuffer->data(),
                    width, height,
                    crop_left, crop_top, crop_right, crop_bottom,
                    frame->mData,
                    frame->mWidth,
                    frame->mHeight,
                    0, 0, frame->mWidth - 1, frame->mHeight - 1);
        }
        else
#endif /* end of CHECK_ALIGN */
        err = converter.convert(
                (const uint8_t *)videoFrameBuffer->data(),
                width, height,
                crop_left, crop_top, crop_right, crop_bottom,
                frame->mData,
                frame->mWidth,
                frame->mHeight,
                0, 0, frame->mWidth - 1, frame->mHeight - 1);
    } else {
        ALOGE("Unable to convert from format 0x%08x to RGB565", srcFormat);

        err = ERROR_UNSUPPORTED;
    }

    videoFrameBuffer.clear();
    source->stop();
    decoder->releaseOutputBuffer(index);

    decoder->stop();
    decoder->release();

    if (err != OK) {
        ALOGE("Colorconverter failed to convert frame.");

        delete frame;
        frame = NULL;
    }

    ALOGV("\033[0;32m In %s:%d done .\033[m", __FUNCTION__, __LINE__);
    return frame;
}

VideoFrame *StagefrightMetadataRetriever::getFrameAtTime(
        int64_t timeUs, int option) {

    ALOGV("getFrameAtTime: %" PRId64 " us option: %d", timeUs, option);

    if (mExtractor.get() == NULL) {
        ALOGV("no extractor.");
        return NULL;
    }

    sp<MetaData> fileMeta = mExtractor->getMetaData();

    if (fileMeta == NULL) {
        ALOGV("extractor doesn't publish metadata, failed to initialize?");
        return NULL;
    }

    int32_t drm = 0;
    if (fileMeta->findInt32(kKeyIsDRM, &drm) && drm != 0) {
        ALOGE("frame grab not allowed.");
        return NULL;
    }

    size_t n = mExtractor->countTracks();
    size_t i;
    for (i = 0; i < n; ++i) {
        sp<MetaData> meta = mExtractor->getTrackMetaData(i);

        const char *mime;
        CHECK(meta->findCString(kKeyMIMEType, &mime));

#if 0   // Using on2 decode vp9, don't worry about that.
        if (!strncasecmp(mime, "video/x-vnd.on2.vp9", 19)) {
            int32_t width, height;
            CHECK(meta->findInt32(kKeyWidth, &width));
            CHECK(meta->findInt32(kKeyHeight, &height));
            if ((width*height) > VP9_MAX_SUPPROT_RESOLUTION)
            {
                ALOGE("###  mime: %s   resolution: %d x %d not support!! ###\033[m", mime, width, height);
                return NULL;
            }
        }
#endif

        if (!strncasecmp(mime, "video/", 6)) {
#if 0       // Fixed H265 mp4 thumbnail issue. 20140502, 
            /* TODO: fix me later, need wait decode H265 thumbnail stable */
            if (!strncasecmp(mime, MEDIA_MIMETYPE_VIDEO_HEVC, strlen(MEDIA_MIMETYPE_VIDEO_HEVC)))
            {
                ALOGD("Skip h265 thumbnail");
                return NULL;
            }
#endif
            break;
        }
    }

    if (i == n) {
        ALOGV("no video track found.");
        return NULL;
    }

    sp<MetaData> trackMeta = mExtractor->getTrackMetaData(
            i, MediaExtractor::kIncludeExtensiveMetaData);

    sp<MediaSource> source = mExtractor->getTrack(i);

    if (source.get() == NULL) {
        ALOGV("unable to instantiate video track.");
        return NULL;
    }

    const void *data;
    uint32_t type;
    size_t dataSize;
    if (fileMeta->findData(kKeyAlbumArt, &type, &data, &dataSize)
            && mAlbumArt == NULL) {
        mAlbumArt = MediaAlbumArt::fromData(dataSize, data);
    }

    const char *mime;
    CHECK(trackMeta->findCString(kKeyMIMEType, &mime));

    Vector<OMXCodec::CodecNameAndQuirks> matchingCodecs;
    OMXCodec::findMatchingCodecs(
            mime,
            false, /* encoder */
            NULL, /* matchComponentName */
            OMXCodec::kPreferSoftwareCodecs,
            &matchingCodecs);

    for (size_t i = 0; i < matchingCodecs.size(); ++i) {
        const char *componentName = matchingCodecs[i].mName.string();
        VideoFrame *frame =
            extractVideoFrame(componentName, trackMeta, source, timeUs, option);

        if (frame != NULL) {
            return frame;
        }
        ALOGV("%s failed to extract thumbnail, trying next decoder.", componentName);
    }

    return NULL;
}

MediaAlbumArt *StagefrightMetadataRetriever::extractAlbumArt() {
    ALOGV("extractAlbumArt (extractor: %s)", mExtractor.get() != NULL ? "YES" : "NO");

    if (mExtractor == NULL) {
        return NULL;
    }

    if (!mParsedMetaData) {
        parseMetaData();

        mParsedMetaData = true;
    }

    if (mAlbumArt) {
        return mAlbumArt->clone();
    }

    return NULL;
}

const char *StagefrightMetadataRetriever::extractMetadata(int keyCode) {
    if (mExtractor == NULL) {
        return NULL;
    }

    if (!mParsedMetaData) {
        parseMetaData();

        mParsedMetaData = true;
    }

    ssize_t index = mMetaData.indexOfKey(keyCode);

    if (index < 0) {
        return NULL;
    }

    return mMetaData.valueAt(index).string();
}

void StagefrightMetadataRetriever::parseMetaData() {
    sp<MetaData> meta = mExtractor->getMetaData();

    if (meta == NULL) {
        ALOGV("extractor doesn't publish metadata, failed to initialize?");
        return;
    }

    struct Map {
        int from;
        int to;
        const char *name;
    };
    static const Map kMap[] = {
        { kKeyMIMEType, METADATA_KEY_MIMETYPE, NULL },
        { kKeyCDTrackNumber, METADATA_KEY_CD_TRACK_NUMBER, "tracknumber" },
        { kKeyDiscNumber, METADATA_KEY_DISC_NUMBER, "discnumber" },
        { kKeyAlbum, METADATA_KEY_ALBUM, "album" },
        { kKeyArtist, METADATA_KEY_ARTIST, "artist" },
        { kKeyAlbumArtist, METADATA_KEY_ALBUMARTIST, "albumartist" },
        { kKeyAuthor, METADATA_KEY_AUTHOR, NULL },
        { kKeyComposer, METADATA_KEY_COMPOSER, "composer" },
        { kKeyDate, METADATA_KEY_DATE, NULL },
        { kKeyGenre, METADATA_KEY_GENRE, "genre" },
        { kKeyTitle, METADATA_KEY_TITLE, "title" },
        { kKeyYear, METADATA_KEY_YEAR, "year" },
        { kKeyWriter, METADATA_KEY_WRITER, "writer" },
        { kKeyCompilation, METADATA_KEY_COMPILATION, "compilation" },
        { kKeyLocation, METADATA_KEY_LOCATION, NULL },
    };

    static const size_t kNumMapEntries = sizeof(kMap) / sizeof(kMap[0]);

    CharacterEncodingDetector *detector = new CharacterEncodingDetector();

    for (size_t i = 0; i < kNumMapEntries; ++i) {
        const char *value;
        if (meta->findCString(kMap[i].from, &value)) {
            if (kMap[i].name) {
                // add to charset detector
                detector->addTag(kMap[i].name, value);
            } else {
                // directly add to output list
                mMetaData.add(kMap[i].to, String8(value));
            }
        }
    }

    detector->detectAndConvert();
    int size = detector->size();
    if (size) {
        for (int i = 0; i < size; i++) {
            const char *name;
            const char *value;
            detector->getTag(i, &name, &value);
            for (size_t j = 0; j < kNumMapEntries; ++j) {
                if (kMap[j].name && !strcmp(kMap[j].name, name)) {
                    mMetaData.add(kMap[j].to, String8(value));
                }
            }
        }
    }
    delete detector;

    const void *data;
    uint32_t type;
    size_t dataSize;
    if (meta->findData(kKeyAlbumArt, &type, &data, &dataSize)
            && mAlbumArt == NULL) {
        mAlbumArt = MediaAlbumArt::fromData(dataSize, data);
    }

    size_t numTracks = mExtractor->countTracks();

    char tmp[32];
    sprintf(tmp, "%zu", numTracks);

    mMetaData.add(METADATA_KEY_NUM_TRACKS, String8(tmp));

    float captureFps;
    if (meta->findFloat(kKeyCaptureFramerate, &captureFps)) {
        if(captureFps > 0) {
            sprintf(tmp, "%f", captureFps);
            mMetaData.add(METADATA_KEY_CAPTURE_FRAMERATE, String8(tmp));
        }
    }

    bool hasAudio = false;
    bool hasVideo = false;
    int32_t videoWidth = -1;
    int32_t videoHeight = -1;
    int32_t audioBitrate = -1;
    int32_t rotationAngle = -1;

    // The overall duration is the duration of the longest track.
    int64_t maxDurationUs = 0;
    String8 timedTextLang;
    for (size_t i = 0; i < numTracks; ++i) {
        sp<MetaData> trackMeta = mExtractor->getTrackMetaData(i);

        int64_t durationUs;
        if (trackMeta->findInt64(kKeyDuration, &durationUs)) {
            if (durationUs > maxDurationUs) {
                maxDurationUs = durationUs;
            }
        }

        const char *mime;
        if (trackMeta->findCString(kKeyMIMEType, &mime)) {
            if (!hasAudio && !strncasecmp("audio/", mime, 6)) {
                hasAudio = true;

                if (!trackMeta->findInt32(kKeyBitRate, &audioBitrate)) {
                    audioBitrate = -1;
                }
            } else if (!hasVideo && !strncasecmp("video/", mime, 6)) {
                hasVideo = true;

                CHECK(trackMeta->findInt32(kKeyWidth, &videoWidth));
                CHECK(trackMeta->findInt32(kKeyHeight, &videoHeight));
                if (!trackMeta->findInt32(kKeyRotation, &rotationAngle)) {
                    rotationAngle = 0;
                }
            } else if (!strcasecmp(mime, MEDIA_MIMETYPE_TEXT_3GPP)) {
                const char *lang;
                trackMeta->findCString(kKeyMediaLanguage, &lang);
                timedTextLang.append(String8(lang));
                timedTextLang.append(String8(":"));
            }
        }
    }

    // To save the language codes for all timed text tracks
    // If multiple text tracks present, the format will look
    // like "eng:chi"
    if (!timedTextLang.isEmpty()) {
        mMetaData.add(METADATA_KEY_TIMED_TEXT_LANGUAGES, timedTextLang);
    }

    // The duration value is a string representing the duration in ms.
    sprintf(tmp, "%" PRId64, (maxDurationUs + 500) / 1000);
    mMetaData.add(METADATA_KEY_DURATION, String8(tmp));

    if (hasAudio) {
        mMetaData.add(METADATA_KEY_HAS_AUDIO, String8("yes"));
    }

    if (hasVideo) {
        mMetaData.add(METADATA_KEY_HAS_VIDEO, String8("yes"));

        sprintf(tmp, "%d", videoWidth);
        mMetaData.add(METADATA_KEY_VIDEO_WIDTH, String8(tmp));

        sprintf(tmp, "%d", videoHeight);
        mMetaData.add(METADATA_KEY_VIDEO_HEIGHT, String8(tmp));

        sprintf(tmp, "%d", rotationAngle);
        mMetaData.add(METADATA_KEY_VIDEO_ROTATION, String8(tmp));
    }

    if (numTracks == 1 && hasAudio && audioBitrate >= 0) {
        sprintf(tmp, "%d", audioBitrate);
        mMetaData.add(METADATA_KEY_BITRATE, String8(tmp));
    } else {
        off64_t sourceSize;
        if (mSource->getSize(&sourceSize) == OK) {
            int64_t avgBitRate = (int64_t)(sourceSize * 8E6 / maxDurationUs);

            sprintf(tmp, "%" PRId64, avgBitRate);
            mMetaData.add(METADATA_KEY_BITRATE, String8(tmp));
        }
    }

    if (numTracks == 1) {
        const char *fileMIME;
        CHECK(meta->findCString(kKeyMIMEType, &fileMIME));

        if (!strcasecmp(fileMIME, "video/x-matroska")) {
            sp<MetaData> trackMeta = mExtractor->getTrackMetaData(0);
            const char *trackMIME;
            CHECK(trackMeta->findCString(kKeyMIMEType, &trackMIME));

            if (!strncasecmp("audio/", trackMIME, 6)) {
                // The matroska file only contains a single audio track,
                // rewrite its mime type.
                mMetaData.add(
                        METADATA_KEY_MIMETYPE, String8("audio/x-matroska"));
            }
        }
    }

    // To check whether the media file is drm-protected
    if (mExtractor->getDrmFlag()) {
        mMetaData.add(METADATA_KEY_IS_DRM, String8("1"));
    }
}

void StagefrightMetadataRetriever::clearMetadata() {
    mParsedMetaData = false;
    mMetaData.clear();
    delete mAlbumArt;
    mAlbumArt = NULL;
}

}  // namespace android
