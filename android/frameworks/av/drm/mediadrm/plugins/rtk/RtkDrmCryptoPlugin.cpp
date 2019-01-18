/*
 * Copyright (C) 2013 The Android Open Source Project
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
#define LOG_TAG "RtkDrmCryptoPlugin"
#include <utils/Log.h>


#include "drm/DrmAPI.h"
#include "RtkDrmCryptoPlugin.h"
#include "media/stagefright/MediaErrors.h"
#include "media/stagefright/foundation/AString.h"
#include "media/stagefright/foundation/hexdump.h"
#include "utils/Timers.h"

#include "tee_client_api.h"



using namespace android;

// Shared library entry point
DrmFactory *createDrmFactory()
{
    return new RtkDrmFactory();
}

// Shared library entry point
CryptoFactory *createCryptoFactory()
{
    return new RtkCryptoFactory();
}

const uint8_t rtk_uuid[16] = {0x69, 0x3e, 0x18, 0xba, 0xba, 0xb0, 0x11, 0xe6,
                               0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01};
namespace android {


    // RtkDrmFactory
    bool RtkDrmFactory::isCryptoSchemeSupported(const uint8_t uuid[16])
    {
        bool supported = (memcmp(uuid, rtk_uuid, sizeof(rtk_uuid)) == 0) ? true : false;
        ALOGV("[ RTK ]RtkDrmFactory::isCryptoSchemeSupported: %d", supported);
        ALOGV("uuid:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
        return supported;
    }

    bool RtkDrmFactory::isContentTypeSupported(const String8 &mimeType)
    {
        /* Realtek
        if (mimeType != "video/mp4") {
            return false;
        }
        */
        return true;
    }

    status_t RtkDrmFactory::createDrmPlugin(const uint8_t /* uuid */[16], DrmPlugin **plugin)
    {
        *plugin = new RtkDrmPlugin();
        Rtk_DrmFactory_Initialize();

        ALOGV("\033[0;33m  Rtk_Plugin : RtkDrmFactory::createDrmPlugin\033[m");

        return OK;
    }

    // RtkCryptoFactory
    bool RtkCryptoFactory::isCryptoSchemeSupported(const uint8_t uuid[16]) const
    {
        return (!memcmp(uuid, rtk_uuid, sizeof(rtk_uuid)));
    }

    status_t RtkCryptoFactory::createPlugin(const uint8_t /* uuid */[16],
                                             const void * /* data */,
                                             size_t /* size */, CryptoPlugin **plugin)
    {
        *plugin = new RtkCryptoPlugin();

        ALOGV("\033[0;33m  Rtk_Plugin : RtkCryptoFactory::createPlugin \033[m");
        int res =0;

        res = Rtk_Plugin_Initialize();
        if(res == TA_FAILED)
        {
            ALOGE("RtkCryptoFactory:Rtk_Plugin_Initialize failed");
        }

        return OK;
    }


    // RtkDrmPlugin methods

    status_t RtkDrmPlugin::openSession(Vector<uint8_t> &sessionId)
    {
        const size_t kSessionIdSize = 8;

        ALOGV("\033[0;33m  Rtk_Plugin : RtkDrmPlugin::openSession \033[m");
        Mutex::Autolock lock(mLock);
        for (size_t i = 0; i < kSessionIdSize / sizeof(long); i++) {
            long r = random();
            sessionId.appendArray((uint8_t *)&r, sizeof(long));
        }
        mSessions.add(sessionId);

        ALOGV("RtkDrmPlugin::openSession() -> %s", vectorToString(sessionId).string());
        return OK;
    }

    status_t RtkDrmPlugin::closeSession(Vector<uint8_t> const &sessionId)
    {
        Mutex::Autolock lock(mLock);
        ALOGV("\033[0;33m  Rtk_Plugin : RtkDrmPlugin::closeSession \033[m");

        ALOGV("RtkDrmPlugin::closeSession(%s)", vectorToString(sessionId).string());
        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGE("Invalid sessionId");
            return BAD_VALUE;
        }
        mSessions.removeAt(index);  
        return OK;
    }


    status_t RtkDrmPlugin::getKeyRequest(Vector<uint8_t> const &sessionId,
                                          Vector<uint8_t> const &initData,
                                          String8 const &mimeType, KeyType keyType,
                                          KeyedVector<String8, String8> const &optionalParameters,
                                          Vector<uint8_t> &request, String8 &defaultUrl,
                                          KeyRequestType *keyRequestType)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::getKeyRequest(sessionId=%s, initData=%s, mimeType=%s"
              ", keyType=%d, optionalParameters=%s))",
              vectorToString(sessionId).string(), vectorToString(initData).string(), mimeType.string(),
              keyType, stringMapToString(optionalParameters).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] initData           -> mock-initdata
        //   string mimeType           -> mock-mimetype
        //   string keyType            -> mock-keytype
        //   string optionalParameters -> mock-optparams formatted as {key1,value1},{key2,value2}

        mByteArrayProperties.add(String8("mock-initdata"), initData);
        mStringProperties.add(String8("mock-mimetype"), mimeType);

        String8 keyTypeStr;
        keyTypeStr.appendFormat("%d", (int)keyType);
        mStringProperties.add(String8("mock-keytype"), keyTypeStr);

        String8 params;
        for (size_t i = 0; i < optionalParameters.size(); i++) {
            params.appendFormat("%s{%s,%s}", i ? "," : "",
                                optionalParameters.keyAt(i).string(),
                                optionalParameters.valueAt(i).string());
        }
        mStringProperties.add(String8("mock-optparams"), params);

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-request       -> request
        //   string mock-default-url   -> defaultUrl
        //   string mock-keyRequestType -> keyRequestType

        index = mByteArrayProperties.indexOfKey(String8("mock-request"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            request = mByteArrayProperties.valueAt(index);
        }

        index = mStringProperties.indexOfKey(String8("mock-defaultUrl"));
        if (index < 0) {
            ALOGD("Missing 'mock-defaultUrl' parameter for mock");
            return BAD_VALUE;
        } else {
            defaultUrl = mStringProperties.valueAt(index);
        }

        index = mStringProperties.indexOfKey(String8("mock-keyRequestType"));
        if (index < 0) {
            ALOGD("Missing 'mock-keyRequestType' parameter for mock");
            return BAD_VALUE;
        } else {
            *keyRequestType = static_cast<KeyRequestType>(
                atoi(mStringProperties.valueAt(index).string()));
        }

        return OK;
    }

    status_t RtkDrmPlugin::provideKeyResponse(Vector<uint8_t> const &sessionId,
                                               Vector<uint8_t> const &response,
                                               Vector<uint8_t> &keySetId)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::provideKeyResponse(sessionId=%s, response=%s)",
              vectorToString(sessionId).string(), vectorToString(response).string());
        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }
        if (response.size() == 0) {
            return BAD_VALUE;
        }

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] response            -> mock-response
        mByteArrayProperties.add(String8("mock-response"), response);

        const size_t kKeySetIdSize = 8;

        for (size_t i = 0; i < kKeySetIdSize / sizeof(long); i++) {
            long r = random();
            keySetId.appendArray((uint8_t *)&r, sizeof(long));
        }
        mKeySets.add(keySetId);

        return OK;
    }

    status_t RtkDrmPlugin::removeKeys(Vector<uint8_t> const &keySetId)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::removeKeys(keySetId=%s)",
              vectorToString(keySetId).string());

        ssize_t index = findKeySet(keySetId);
        if (index == kNotFound) {
            ALOGD("Invalid keySetId");
            return BAD_VALUE;
        }
        mKeySets.removeAt(index);

        return OK;
    }

    status_t RtkDrmPlugin::restoreKeys(Vector<uint8_t> const &sessionId,
                                        Vector<uint8_t> const &keySetId)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::restoreKeys(sessionId=%s, keySetId=%s)",
              vectorToString(sessionId).string(),
              vectorToString(keySetId).string());
        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        index = findKeySet(keySetId);
        if (index == kNotFound) {
            ALOGD("Invalid keySetId");
            return BAD_VALUE;
        }

        return OK;
    }

    status_t RtkDrmPlugin::queryKeyStatus(Vector<uint8_t> const &sessionId,
                                               KeyedVector<String8, String8> &infoMap) const
    {
        ALOGD("RtkDrmPlugin::queryKeyStatus(sessionId=%s)",
              vectorToString(sessionId).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        infoMap.add(String8("purchaseDuration"), String8("1000"));
        infoMap.add(String8("licenseDuration"), String8("100"));
        return OK;
    }

    status_t RtkDrmPlugin::getProvisionRequest(String8 const & /* certType */,
                                                String8 const & /* certAuthority */,
                                                Vector<uint8_t> &request,
                                                String8 &defaultUrl)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::getProvisionRequest()");

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-request       -> request
        //   string mock-default-url   -> defaultUrl

        ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-request"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            request = mByteArrayProperties.valueAt(index);
        }

        index = mStringProperties.indexOfKey(String8("mock-defaultUrl"));
        if (index < 0) {
            ALOGD("Missing 'mock-defaultUrl' parameter for mock");
            return BAD_VALUE;
        } else {
            defaultUrl = mStringProperties.valueAt(index);
        }
        return OK;
    }

    status_t RtkDrmPlugin::provideProvisionResponse(Vector<uint8_t> const &response,
                                                     Vector<uint8_t> & /* certificate */,
                                                     Vector<uint8_t> & /* wrappedKey */)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::provideProvisionResponse(%s)",
              vectorToString(response).string());

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] response            -> mock-response

        mByteArrayProperties.add(String8("mock-response"), response);
        return OK;
    }

    status_t RtkDrmPlugin::unprovisionDevice()
    {
        ALOGD("RtkDrmPlugin::unprovisionDevice()");
        return OK;
    }

    status_t RtkDrmPlugin::getSecureStop(Vector<uint8_t> const & /* ssid */,
                                          Vector<uint8_t> & secureStop)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::getSecureStop()");

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-secure-stop  -> first secure stop in list

        ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-secure-stop"));
        if (index < 0) {
            ALOGD("Missing 'mock-secure-stop' parameter for mock");
            return BAD_VALUE;
        } else {
            secureStop = mByteArrayProperties.valueAt(index);
        }
        return OK;
    }

    status_t RtkDrmPlugin::getSecureStops(List<Vector<uint8_t> > &secureStops)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::getSecureStops()");

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-secure-stop1  -> first secure stop in list
        //   byte[] mock-secure-stop2  -> second secure stop in list

        Vector<uint8_t> ss1, ss2;
        ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-secure-stop1"));
        if (index < 0) {
            ALOGD("Missing 'mock-secure-stop1' parameter for mock");
            return BAD_VALUE;
        } else {
            ss1 = mByteArrayProperties.valueAt(index);
        }

        index = mByteArrayProperties.indexOfKey(String8("mock-secure-stop2"));
        if (index < 0) {
            ALOGD("Missing 'mock-secure-stop2' parameter for mock");
            return BAD_VALUE;
        } else {
            ss2 = mByteArrayProperties.valueAt(index);
        }

        secureStops.push_back(ss1);
        secureStops.push_back(ss2);
        return OK;
    }

    status_t RtkDrmPlugin::releaseSecureStops(Vector<uint8_t> const &ssRelease)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::releaseSecureStops(%s)",
              vectorToString(ssRelease).string());

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] secure-stop-release  -> mock-ssrelease
        mByteArrayProperties.add(String8("mock-ssrelease"), ssRelease);

        return OK;
    }

    status_t RtkDrmPlugin::releaseAllSecureStops()
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::releaseAllSecureStops()");
        return OK;
    }

    status_t RtkDrmPlugin::getPropertyString(String8 const &name, String8 &value) const
    {
        ALOGD("RtkDrmPlugin::getPropertyString(name=%s)", name.string());
        ssize_t index = mStringProperties.indexOfKey(name);
        if (index < 0) {
            ALOGD("no property for '%s'", name.string());
            return BAD_VALUE;
        }
        value = mStringProperties.valueAt(index);
        return OK;
    }

    status_t RtkDrmPlugin::getPropertyByteArray(String8 const &name,
                                                 Vector<uint8_t> &value) const
    {
        ALOGD("RtkDrmPlugin::getPropertyByteArray(name=%s)", name.string());
        ssize_t index = mByteArrayProperties.indexOfKey(name);
        if (index < 0) {
            ALOGD("no property for '%s'", name.string());
            return BAD_VALUE;
        }
        value = mByteArrayProperties.valueAt(index);
        return OK;
    }

    status_t RtkDrmPlugin::setPropertyString(String8 const &name,
                                              String8 const &value)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::setPropertyString(name=%s, value=%s)",
              name.string(), value.string());

        if (name == "mock-send-event") {
            unsigned code, extra;
            sscanf(value.string(), "%d %d", &code, &extra);
            DrmPlugin::EventType eventType = (DrmPlugin::EventType)code;

            Vector<uint8_t> const *pSessionId = NULL;
            ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-event-session-id"));
            if (index >= 0) {
                pSessionId = &mByteArrayProperties[index];
            }

            Vector<uint8_t> const *pData = NULL;
            index = mByteArrayProperties.indexOfKey(String8("mock-event-data"));
            if (index >= 0) {
                pData = &mByteArrayProperties[index];
            }
            ALOGD("sending event from mock drm plugin: %d %d %s %s",
                  (int)code, extra, pSessionId ? vectorToString(*pSessionId) : "{}",
                  pData ? vectorToString(*pData) : "{}");

            sendEvent(eventType, extra, pSessionId, pData);
        } else if (name == "mock-send-expiration-update") {
            int64_t expiryTimeMS;
            sscanf(value.string(), "%jd", &expiryTimeMS);

            Vector<uint8_t> const *pSessionId = NULL;
            ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-event-session-id"));
            if (index >= 0) {
                pSessionId = &mByteArrayProperties[index];
            }

            ALOGD("sending expiration-update from mock drm plugin: %jd %s",
                  expiryTimeMS, pSessionId ? vectorToString(*pSessionId) : "{}");

            sendExpirationUpdate(pSessionId, expiryTimeMS);
        } else if (name == "mock-send-keys-change") {
            Vector<uint8_t> const *pSessionId = NULL;
            ssize_t index = mByteArrayProperties.indexOfKey(String8("mock-event-session-id"));
            if (index >= 0) {
                pSessionId = &mByteArrayProperties[index];
            }

            ALOGD("sending keys-change from mock drm plugin: %s",
                  pSessionId ? vectorToString(*pSessionId) : "{}");

            Vector<DrmPlugin::KeyStatus> keyStatusList;
            DrmPlugin::KeyStatus keyStatus;
            uint8_t keyId1[] = {'k', 'e', 'y', '1'};
            keyStatus.mKeyId.clear();
            keyStatus.mKeyId.appendArray(keyId1, sizeof(keyId1));
            keyStatus.mType = DrmPlugin::kKeyStatusType_Usable;
            keyStatusList.add(keyStatus);

            uint8_t keyId2[] = {'k', 'e', 'y', '2'};
            keyStatus.mKeyId.clear();
            keyStatus.mKeyId.appendArray(keyId2, sizeof(keyId2));
            keyStatus.mType = DrmPlugin::kKeyStatusType_Expired;
            keyStatusList.add(keyStatus);

            uint8_t keyId3[] = {'k', 'e', 'y', '3'};
            keyStatus.mKeyId.clear();
            keyStatus.mKeyId.appendArray(keyId3, sizeof(keyId3));
            keyStatus.mType = DrmPlugin::kKeyStatusType_OutputNotAllowed;
            keyStatusList.add(keyStatus);

            uint8_t keyId4[] = {'k', 'e', 'y', '4'};
            keyStatus.mKeyId.clear();
            keyStatus.mKeyId.appendArray(keyId4, sizeof(keyId4));
            keyStatus.mType = DrmPlugin::kKeyStatusType_StatusPending;
            keyStatusList.add(keyStatus);

            uint8_t keyId5[] = {'k', 'e', 'y', '5'};
            keyStatus.mKeyId.clear();
            keyStatus.mKeyId.appendArray(keyId5, sizeof(keyId5));
            keyStatus.mType = DrmPlugin::kKeyStatusType_InternalError;
            keyStatusList.add(keyStatus);

            sendKeysChange(pSessionId, &keyStatusList, true);
        } else {
            mStringProperties.add(name, value);
        }
        return OK;
    }

    status_t RtkDrmPlugin::setPropertyByteArray(String8 const &name,
                                                 Vector<uint8_t> const &value)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::setPropertyByteArray(name=%s, value=%s)",
              name.string(), vectorToString(value).string());
        mByteArrayProperties.add(name, value);
        return OK;
    }

    status_t RtkDrmPlugin::setCipherAlgorithm(Vector<uint8_t> const &sessionId,
                                               String8 const &algorithm)
    {
        Mutex::Autolock lock(mLock);

        ALOGD("RtkDrmPlugin::setCipherAlgorithm(sessionId=%s, algorithm=%s)",
              vectorToString(sessionId).string(), algorithm.string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        if (algorithm == "AES/CBC/NoPadding") {
            return OK;
        }
        return BAD_VALUE;
    }

    status_t RtkDrmPlugin::setMacAlgorithm(Vector<uint8_t> const &sessionId,
                                            String8 const &algorithm)
    {
        Mutex::Autolock lock(mLock);

        ALOGD("RtkDrmPlugin::setMacAlgorithm(sessionId=%s, algorithm=%s)",
              vectorToString(sessionId).string(), algorithm.string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        if (algorithm == "HmacSHA256") {
            return OK;
        }
        return BAD_VALUE;
    }

    status_t RtkDrmPlugin::encrypt(Vector<uint8_t> const &sessionId,
                                    Vector<uint8_t> const &keyId,
                                    Vector<uint8_t> const &input,
                                    Vector<uint8_t> const &iv,
                                    Vector<uint8_t> &output)
    {
        Mutex::Autolock lock(mLock);
        ALOGV("encrypt(sId:%s kId:%s iv:%s size:%zu)",
              vectorToString(sessionId).string(),
              vectorToString(keyId).string(),
              vectorToString(iv).string(),
              input.size());
        ALOGV("in :%s)", vectorToString(input).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        #if 0
        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] keyId              -> mock-keyid
        //   byte[] input              -> mock-input
        //   byte[] iv                 -> mock-iv
        mByteArrayProperties.add(String8("Rtk-keyid"), keyId);
        mByteArrayProperties.add(String8("Rtk-input"), input);
        mByteArrayProperties.add(String8("Rtk-iv"), iv);

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-output        -> output
        index = mByteArrayProperties.indexOfKey(String8("Rtk-output"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            output = mByteArrayProperties.valueAt(index);
        }
        #endif

        Rtk_plugin_Encrypt((unsigned char *)&input[0], (unsigned char *)&output[0], input.size());

        ALOGV("out:%s", vectorToString(output).string());
        return OK;
    }

    status_t RtkDrmPlugin::decrypt(Vector<uint8_t> const &sessionId,
                                    Vector<uint8_t> const &keyId,
                                    Vector<uint8_t> const &input,
                                    Vector<uint8_t> const &iv,
                                    Vector<uint8_t> &output)
    {
        Mutex::Autolock lock(mLock);
        ALOGV("decrypt(sId:%s kId:%s iv:%s size:%zu)",
              vectorToString(sessionId).string(),
              vectorToString(keyId).string(),
              vectorToString(iv).string());
        ALOGV("in :%s)", vectorToString(input).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }
         #if 0
        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] keyId              -> mock-keyid
        //   byte[] input              -> mock-input
        //   byte[] iv                 -> mock-iv
        mByteArrayProperties.add(String8("mock-keyid"), keyId);
        mByteArrayProperties.add(String8("mock-input"), input);
        mByteArrayProperties.add(String8("mock-iv"), iv);

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-output        -> output
        index = mByteArrayProperties.indexOfKey(String8("mock-output"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            output = mByteArrayProperties.valueAt(index);
        }
        #endif

        Rtk_plugin_Decrypt((unsigned char *)&input[0], (unsigned char *)&output[0], input.size());
        ALOGV("out:%s", vectorToString(output).string());
        return OK;
    }

    status_t RtkDrmPlugin::sign(Vector<uint8_t> const &sessionId,
                                 Vector<uint8_t> const &keyId,
                                 Vector<uint8_t> const &message,
                                 Vector<uint8_t> &signature)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::sign(sessionId=%s, keyId=%s, message=%s)",
              vectorToString(sessionId).string(),
              vectorToString(keyId).string(),
              vectorToString(message).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] keyId              -> mock-keyid
        //   byte[] message            -> mock-message
        mByteArrayProperties.add(String8("mock-keyid"), keyId);
        mByteArrayProperties.add(String8("mock-message"), message);

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   byte[] mock-signature        -> signature
        index = mByteArrayProperties.indexOfKey(String8("mock-signature"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            signature = mByteArrayProperties.valueAt(index);
        }
        return OK;
    }

    status_t RtkDrmPlugin::verify(Vector<uint8_t> const &sessionId,
                                   Vector<uint8_t> const &keyId,
                                   Vector<uint8_t> const &message,
                                   Vector<uint8_t> const &signature,
                                   bool &match)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::verify(sessionId=%s, keyId=%s, message=%s, signature=%s)",
              vectorToString(sessionId).string(),
              vectorToString(keyId).string(),
              vectorToString(message).string(),
              vectorToString(signature).string());

        ssize_t index = findSession(sessionId);
        if (index == kNotFound) {
            ALOGD("Invalid sessionId");
            return BAD_VALUE;
        }

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] keyId              -> mock-keyid
        //   byte[] message            -> mock-message
        //   byte[] signature          -> mock-signature
        mByteArrayProperties.add(String8("mock-keyid"), keyId);
        mByteArrayProperties.add(String8("mock-message"), message);
        mByteArrayProperties.add(String8("mock-signature"), signature);

        // Properties used in mock test, set by cts test app returned from mock plugin
        //   String mock-match "1" or "0"         -> match
        index = mStringProperties.indexOfKey(String8("mock-match"));
        if (index < 0) {
            ALOGD("Missing 'mock-request' parameter for mock");
            return BAD_VALUE;
        } else {
            match = atol(mStringProperties.valueAt(index).string());
        }
        return OK;
    }

    status_t RtkDrmPlugin::signRSA(Vector<uint8_t> const &sessionId,
                                    String8 const &algorithm,
                                    Vector<uint8_t> const &message,
                                    Vector<uint8_t> const &wrappedKey,
                                    Vector<uint8_t> &signature)
    {
        Mutex::Autolock lock(mLock);
        ALOGD("RtkDrmPlugin::signRSA(sessionId=%s, algorithm=%s, keyId=%s, "
              "message=%s, signature=%s)",
              vectorToString(sessionId).string(),
              algorithm.string(),
              vectorToString(message).string(),
              vectorToString(wrappedKey).string(),
              vectorToString(signature).string());

        // Properties used in mock test, set by mock plugin and verifed cts test app
        //   byte[] wrappedKey         -> mock-wrappedkey
        //   byte[] message            -> mock-message
        //   byte[] signature          -> mock-signature
        mByteArrayProperties.add(String8("mock-sessionid"), sessionId);
        mStringProperties.add(String8("mock-algorithm"), algorithm);
        mByteArrayProperties.add(String8("mock-message"), message);
        mByteArrayProperties.add(String8("mock-wrappedkey"), wrappedKey);
        mByteArrayProperties.add(String8("mock-signature"), signature);
        return OK;
    }

    ssize_t RtkDrmPlugin::findSession(Vector<uint8_t> const &sessionId) const
    {
        ALOGD("findSession: nsessions=%d, size=%d", mSessions.size(), sessionId.size());
        for (size_t i = 0; i < mSessions.size(); ++i) {
            if (memcmp(mSessions[i].array(), sessionId.array(), sessionId.size()) == 0) {
                return i;
            }
        }
        return kNotFound;
    }

    ssize_t RtkDrmPlugin::findKeySet(Vector<uint8_t> const &keySetId) const
    {
        ALOGD("findKeySet: nkeySets=%d, size=%d", mKeySets.size(), keySetId.size());
        for (size_t i = 0; i < mKeySets.size(); ++i) {
            if (memcmp(mKeySets[i].array(), keySetId.array(), keySetId.size()) == 0) {
                return i;
            }
        }
        return kNotFound;
    }


    // Conversion utilities
    String8 RtkDrmPlugin::vectorToString(Vector<uint8_t> const &vector) const
    {
        return arrayToString(vector.array(), vector.size());
    }

    String8 RtkDrmPlugin::arrayToString(uint8_t const *array, size_t len) const
    {
        String8 result("{ ");
        for (size_t i = 0; i < len; i++) {
            result.appendFormat("0x%02x ", array[i]);
        }
        result += "}";
        return result;
    }

    String8 RtkDrmPlugin::stringMapToString(KeyedVector<String8, String8> map) const
    {
        String8 result("{ ");
        for (size_t i = 0; i < map.size(); i++) {
            result.appendFormat("%s{name=%s, value=%s}", i > 0 ? ", " : "",
                                map.keyAt(i).string(), map.valueAt(i).string());
        }
        return result + " }";
    }

    bool operator<(Vector<uint8_t> const &lhs, Vector<uint8_t> const &rhs) {
        return lhs.size() < rhs.size() || (memcmp(lhs.array(), rhs.array(), lhs.size()) < 0);
    }

    //
    // Crypto Plugin
    //

    bool RtkCryptoPlugin::requiresSecureDecoderComponent(const char *mime) const
    {
        ALOGD("RtkCryptoPlugin::requiresSecureDecoderComponent(mime=%s)", mime);
        return false;
    }


    ssize_t
    RtkCryptoPlugin::decrypt(bool secure, const uint8_t key[16], const uint8_t iv[16],
                              Mode mode, const void *srcPtr, const SubSample *subSamples,
                              size_t numSubSamples, void *dstPtr, AString * /* errorDetailMsg */)
    {
        ALOGV("RtkCryptoPlugin::decrypt(secure=%d, key=%s, iv=%s, mode=%d, src=%p, "
              "subSamples=%s, dst=%p)",
              (int)secure,
              arrayToString(key, sizeof(key)).string(),
              arrayToString(iv, sizeof(iv)).string(),
              (int)mode, srcPtr,
              subSamplesToString(subSamples, numSubSamples).string(),
              dstPtr);

        size_t offset = 0;
        nsecs_t t1 = systemTime();
        for(size_t i = 0; i < numSubSamples; i++){
            const SubSample& subSample = subSamples[i];

			if (subSample.mNumBytesOfClearData > 0) {
                //ALOGD("clear: %zu", subSample.mNumBytesOfClearData);
                //hexdump(srcPtr + offset, subSample.mNumBytesOfClearData, 0, NULL);
				memcpy(dstPtr + offset, srcPtr + offset,
						subSample.mNumBytesOfClearData);
				offset += subSample.mNumBytesOfClearData;
			}

			if (subSample.mNumBytesOfEncryptedData > 0) {
                //ALOGD("enc: %zu", subSample.mNumBytesOfEncryptedData);
                //hexdump(srcPtr + offset, subSample.mNumBytesOfEncryptedData, 0, NULL);
                Rtk_plugin_Decrypt((unsigned char *)srcPtr + offset, (unsigned char *)dstPtr + offset, subSample.mNumBytesOfEncryptedData);
                //ALOGD("dec: %zu", subSample.mNumBytesOfEncryptedData);
                //hexdump(dstPtr + offset, subSample.mNumBytesOfEncryptedData, 0, NULL);
				offset += subSample.mNumBytesOfEncryptedData;
			}
        }
        nsecs_t t2 = systemTime();
        ALOGV("decrypt takes %lldms for %zu bytes", ns2ms(t2-t1), offset);


        ALOGV("dec: %zu", offset);
        //hexdump(dstPtr, offset, 0, NULL);

        ALOGV("enc: %zu", offset);
        //hexdump(srcPtr, offset, 0, NULL);
        return offset;
    }

    ssize_t
    RtkCryptoPlugin::decrypt(bool secure, const void *srcPtr, void *dstPtr, size_t size)
    {
        ALOGV("[ RTK ]RtkCryptoPlugin::decrypt");
        Rtk_plugin_Decrypt((unsigned char *)srcPtr, (unsigned char *)dstPtr, size);
        return OK;
    }

    ssize_t
    RtkCryptoPlugin::encrypt(bool secure, const void *srcPtr, void *dstPtr, size_t size)
    {
        ALOGV("[ RTK ]RtkCryptoPlugin::encrypt");
        //hexdump(srcPtr, size, 0, NULL);
        nsecs_t t1 = systemTime();
        Rtk_plugin_Encrypt((unsigned char *)srcPtr, (unsigned char *)dstPtr, size);
        nsecs_t t2 = systemTime();
        ALOGV("encrypt takes %lldms for %zu bytes", ns2ms(t2-t1), size);
        //hexdump(dstPtr, size, 0, NULL);
        return OK;
    }

    // Conversion utilities
    String8 RtkCryptoPlugin::arrayToString(uint8_t const *array, size_t len) const
    {
        String8 result("{ ");
        for (size_t i = 0; i < len; i++) {
            result.appendFormat("0x%02x ", array[i]);
        }
        result += "}";
        return result;
    }

    String8 RtkCryptoPlugin::subSamplesToString(SubSample const *subSamples,
                                                 size_t numSubSamples) const
    {
        String8 result;
        for (size_t i = 0; i < numSubSamples; i++) {
            result.appendFormat("[%zu] {clear:%u, encrypted:%u} ", i,
                                subSamples[i].mNumBytesOfClearData,
                                subSamples[i].mNumBytesOfEncryptedData);
        }
        return result;
    }

};
