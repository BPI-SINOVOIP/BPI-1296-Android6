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
#define LOG_TAG "ICrypto"
#include <utils/Log.h>

#include <binder/Parcel.h>
#include <binder/IMemory.h>
#include <media/ICrypto.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AString.h>

#define CHECK_SRC_VIR_ADDR(x)   (x & 0x1)
#define CHECK_DST_VIR_ADDR(x)   (x & 0x2)

namespace android {

enum {
    INIT_CHECK = IBinder::FIRST_CALL_TRANSACTION,
    IS_CRYPTO_SUPPORTED,
    CREATE_PLUGIN,
    DESTROY_PLUGIN,
    REQUIRES_SECURE_COMPONENT,
    DECRYPT,
    NOTIFY_RESOLUTION,
    SET_MEDIADRM_SESSION,
    DECRYPT_RTK,
    CHECK_PLUGIN_RTK,
    ENCRYPT_RTK,
};

struct BpCrypto : public BpInterface<ICrypto> {
    BpCrypto(const sp<IBinder> &impl)
        : BpInterface<ICrypto>(impl) {
    }

    virtual status_t initCheck() const {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        remote()->transact(INIT_CHECK, data, &reply);

        return reply.readInt32();
    }

    virtual bool isCryptoSchemeSupported(const uint8_t uuid[16]) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.write(uuid, 16);
        remote()->transact(IS_CRYPTO_SUPPORTED, data, &reply);

        return reply.readInt32() != 0;
    }

    virtual status_t createPlugin(
            const uint8_t uuid[16], const void *opaqueData, size_t opaqueSize) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.write(uuid, 16);
        data.writeInt32(opaqueSize);

        if (opaqueSize > 0) {
            data.write(opaqueData, opaqueSize);
        }

        remote()->transact(CREATE_PLUGIN, data, &reply);

        return reply.readInt32();
    }

    virtual status_t destroyPlugin() {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        remote()->transact(DESTROY_PLUGIN, data, &reply);

        return reply.readInt32();
    }

    virtual bool requiresSecureDecoderComponent(
            const char *mime) const {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeCString(mime);
        remote()->transact(REQUIRES_SECURE_COMPONENT, data, &reply);

        return reply.readInt32() != 0;
    }

    virtual ssize_t decrypt(
            bool secure,
            const uint8_t key[16],
            const uint8_t iv[16],
            CryptoPlugin::Mode mode,
            const sp<IMemory> &sharedBuffer, size_t offset,
            const CryptoPlugin::SubSample *subSamples, size_t numSubSamples,
            void *dstPtr,
            AString *errorDetailMsg) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeInt32(secure);
        data.writeInt32(mode);

        static const uint8_t kDummy[16] = { 0 };

        if (key == NULL) {
            key = kDummy;
        }

        if (iv == NULL) {
            iv = kDummy;
        }

        data.write(key, 16);
        data.write(iv, 16);

        size_t totalSize = 0;
        for (size_t i = 0; i < numSubSamples; ++i) {
            totalSize += subSamples[i].mNumBytesOfEncryptedData;
            totalSize += subSamples[i].mNumBytesOfClearData;
        }

        data.writeInt32(totalSize);
        data.writeStrongBinder(IInterface::asBinder(sharedBuffer));
        data.writeInt32(offset);

        data.writeInt32(numSubSamples);
        data.write(subSamples, sizeof(CryptoPlugin::SubSample) * numSubSamples);

        if (secure) {
            data.writeInt64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstPtr)));
        }

        remote()->transact(DECRYPT, data, &reply);

        ssize_t result = reply.readInt32();

        if (isCryptoError(result)) {
            errorDetailMsg->setTo(reply.readCString());
        }

        if (!secure && result >= 0) {
            reply.read(dstPtr, result);
        }

        return result;
    }

    virtual void notifyResolution(
        uint32_t width, uint32_t height) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeInt32(width);
        data.writeInt32(height);
        remote()->transact(NOTIFY_RESOLUTION, data, &reply);
    }

    virtual status_t setMediaDrmSession(const Vector<uint8_t> &sessionId) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());

        writeVector(data, sessionId);
        remote()->transact(SET_MEDIADRM_SESSION, data, &reply);

        return reply.readInt32();
    }

    /* For Realtek DRM Plugin */
    virtual ssize_t decrypt(
            bool secure,
            const int32_t *drmMessageData,
            int32_t drmMessageLen,
            const uint8_t key[16],
            const uint8_t iv[16],
            CryptoPlugin::Mode mode,
            const void *srcPtr, size_t offset,
            const CryptoPlugin::SubSample *subSamples, size_t numSubSamples,
            void *dstPtr,
            AString *errorDetailMsg) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeInt32(secure);
        data.writeInt32(mode);
        if (drmMessageLen > 0)
            data.writeInt32(drmMessageData[TRANSFER_ADDR_MODE]);
        else
        {
            ALOGE("\033[0;35m ---- In %s:%d not match drmMessageLen:%d ------\033[m", __FUNCTION__, __LINE__, drmMessageLen);
            return ERROR_DRM_CANNOT_HANDLE;
        }

        static const uint8_t kDummy[16] = { 0 };
        ALOGV("\033[0;32m ===== IN %s:%d received addrMode:%x shareFd - S:%d D:%d \033[m", __FUNCTION__, __LINE__, drmMessageData[TRANSFER_ADDR_MODE], drmMessageData[TRANSFER_SRC_SHAREDFD], drmMessageData[TRANSFER_DST_SHAREDFD]);

        if (key == NULL) {
            key = kDummy;
        }

        if (iv == NULL) {
            iv = kDummy;
        }

        data.write(key, 16);
        data.write(iv, 16);

        size_t totalSize = 0;
        for (size_t i = 0; i < numSubSamples; ++i) {
            totalSize += subSamples[i].mNumBytesOfEncryptedData;
            totalSize += subSamples[i].mNumBytesOfClearData;
        }

        data.writeInt32(totalSize);
        if (CHECK_SRC_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE]))
            data.write(srcPtr, totalSize);
        else
            data.writeInt64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(srcPtr)));

        data.writeInt32(offset);
        data.writeInt32(numSubSamples);
        data.write(subSamples, sizeof(CryptoPlugin::SubSample) * numSubSamples);

        if (secure || !CHECK_DST_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE])) {
            data.writeInt64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstPtr)));
        }

        remote()->transact(DECRYPT_RTK, data, &reply);

        ssize_t result = reply.readInt32();

        if (result >= ERROR_DRM_VENDOR_MIN && result <= ERROR_DRM_VENDOR_MAX) {
            errorDetailMsg->setTo(reply.readCString());
        }

        if (!secure && result >= 0 && CHECK_DST_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE])) {
            reply.read(dstPtr, result);
        }

        return result;
    }

    virtual uint32_t checkPlugin(uint32_t magicNumber) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeInt32(magicNumber);
        remote()->transact(CHECK_PLUGIN_RTK, data, &reply);
        return reply.readInt32();
    }

    virtual ssize_t encrypt(bool secure, const void *srcPtr, void *dstPtr, size_t size) {
        Parcel data, reply;
        data.writeInterfaceToken(ICrypto::getInterfaceDescriptor());
        data.writeInt32(secure);
        data.writeInt64((unsigned long)srcPtr);
        data.writeInt64((unsigned long)dstPtr);
        data.writeInt32(size);
        remote()->transact(ENCRYPT_RTK, data, &reply);
        ssize_t result = reply.readInt32();
        return result;
    }

private:
    void readVector(Parcel &reply, Vector<uint8_t> &vector) const {
        uint32_t size = reply.readInt32();
        vector.insertAt((size_t)0, size);
        reply.read(vector.editArray(), size);
    }

    void writeVector(Parcel &data, Vector<uint8_t> const &vector) const {
        data.writeInt32(vector.size());
        data.write(vector.array(), vector.size());
    }

    DISALLOW_EVIL_CONSTRUCTORS(BpCrypto);
};

IMPLEMENT_META_INTERFACE(Crypto, "android.hardware.ICrypto");

////////////////////////////////////////////////////////////////////////////////

void BnCrypto::readVector(const Parcel &data, Vector<uint8_t> &vector) const {
    uint32_t size = data.readInt32();
    vector.insertAt((size_t)0, size);
    data.read(vector.editArray(), size);
}

void BnCrypto::writeVector(Parcel *reply, Vector<uint8_t> const &vector) const {
    reply->writeInt32(vector.size());
    reply->write(vector.array(), vector.size());
}

status_t BnCrypto::onTransact(
    uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags) {
    switch (code) {
        case INIT_CHECK:
        {
            CHECK_INTERFACE(ICrypto, data, reply);
            reply->writeInt32(initCheck());

            return OK;
        }

        case IS_CRYPTO_SUPPORTED:
        {
            CHECK_INTERFACE(ICrypto, data, reply);
            uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));
            reply->writeInt32(isCryptoSchemeSupported(uuid));

            return OK;
        }

        case CREATE_PLUGIN:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));

            size_t opaqueSize = data.readInt32();
            void *opaqueData = NULL;

            if (opaqueSize > 0) {
                opaqueData = malloc(opaqueSize);
                data.read(opaqueData, opaqueSize);
            }

            reply->writeInt32(createPlugin(uuid, opaqueData, opaqueSize));

            if (opaqueData != NULL) {
                free(opaqueData);
                opaqueData = NULL;
            }

            return OK;
        }

        case DESTROY_PLUGIN:
        {
            CHECK_INTERFACE(ICrypto, data, reply);
            reply->writeInt32(destroyPlugin());

            return OK;
        }

        case REQUIRES_SECURE_COMPONENT:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            const char *mime = data.readCString();
            reply->writeInt32(requiresSecureDecoderComponent(mime));

            return OK;
        }

        case DECRYPT:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            bool secure = data.readInt32() != 0;
            CryptoPlugin::Mode mode = (CryptoPlugin::Mode)data.readInt32();

            uint8_t key[16];
            data.read(key, sizeof(key));

            uint8_t iv[16];
            data.read(iv, sizeof(iv));

            size_t totalSize = data.readInt32();
            sp<IMemory> sharedBuffer =
                interface_cast<IMemory>(data.readStrongBinder());
            int32_t offset = data.readInt32();

            int32_t numSubSamples = data.readInt32();

            CryptoPlugin::SubSample *subSamples =
                new CryptoPlugin::SubSample[numSubSamples];

            data.read(
                    subSamples,
                    sizeof(CryptoPlugin::SubSample) * numSubSamples);

            void *secureBufferId, *dstPtr;
            if (secure) {
                secureBufferId = reinterpret_cast<void *>(static_cast<uintptr_t>(data.readInt64()));
            } else {
                dstPtr = calloc(1, totalSize);
            }

            AString errorDetailMsg;
            ssize_t result;

            size_t sumSubsampleSizes = 0;
            bool overflow = false;
            for (int32_t i = 0; i < numSubSamples; ++i) {
                CryptoPlugin::SubSample &ss = subSamples[i];
                if (sumSubsampleSizes <= SIZE_MAX - ss.mNumBytesOfEncryptedData) {
                    sumSubsampleSizes += ss.mNumBytesOfEncryptedData;
                } else {
                    overflow = true;
                }
                if (sumSubsampleSizes <= SIZE_MAX - ss.mNumBytesOfClearData) {
                    sumSubsampleSizes += ss.mNumBytesOfClearData;
                } else {
                    overflow = true;
                }
            }

            if (overflow || sumSubsampleSizes != totalSize) {
                result = -EINVAL;
            } else if (totalSize > sharedBuffer->size()) {
                result = -EINVAL;
            } else if ((size_t)offset > sharedBuffer->size() - totalSize) {
                result = -EINVAL;
            } else {
                result = decrypt(
                    secure,
                    key,
                    iv,
                    mode,
                    sharedBuffer, offset,
                    subSamples, numSubSamples,
                    secure ? secureBufferId : dstPtr,
                    &errorDetailMsg);
            }

            reply->writeInt32(result);

            if (isCryptoError(result)) {
                reply->writeCString(errorDetailMsg.c_str());
            }

            if (!secure) {
                if (result >= 0) {
                    CHECK_LE(result, static_cast<ssize_t>(totalSize));
                    reply->write(dstPtr, result);
                }
                free(dstPtr);
                dstPtr = NULL;
            }

            delete[] subSamples;
            subSamples = NULL;

            return OK;
        }

        case NOTIFY_RESOLUTION:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            int32_t width = data.readInt32();
            int32_t height = data.readInt32();
            notifyResolution(width, height);

            return OK;
        }

        case SET_MEDIADRM_SESSION:
        {
            CHECK_INTERFACE(IDrm, data, reply);
            Vector<uint8_t> sessionId;
            readVector(data, sessionId);
            reply->writeInt32(setMediaDrmSession(sessionId));
            return OK;
        }

        /* For Realtek DRM plugin */
        case DECRYPT_RTK:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            bool secure = data.readInt32() != 0;
            CryptoPlugin::Mode mode = (CryptoPlugin::Mode)data.readInt32();
            int32_t drmMessageData[MAX_TARNSFER_SIZE];
            int32_t drmMessageLen = MAX_TARNSFER_SIZE;
            memset(drmMessageData, 0, sizeof(int32_t)*drmMessageLen);
            drmMessageData[TRANSFER_ADDR_MODE] = data.readInt32();

            if (CHECK_SRC_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE]))
                drmMessageData[TRANSFER_SRC_SHAREDFD] = 0;
            else
                drmMessageData[TRANSFER_SRC_SHAREDFD] = 1;
            if (CHECK_DST_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE]))
                drmMessageData[TRANSFER_DST_SHAREDFD] = 0;
            else
                drmMessageData[TRANSFER_DST_SHAREDFD] = 1;

            uint8_t key[16];
            data.read(key, sizeof(key));

            uint8_t iv[16];
            data.read(iv, sizeof(iv));
            ALOGV("\033[0;32m $$$$$$$$ In %s:%d secure:%d addrMode:%x shared Fd-S:%d,D:%d \033[m", __FUNCTION__, __LINE__, secure, drmMessageData[TRANSFER_ADDR_MODE], drmMessageData[TRANSFER_SRC_SHAREDFD], drmMessageData[TRANSFER_DST_SHAREDFD]);

            size_t totalSize = data.readInt32();
            // Always used physical address
            void *srcData = NULL;
            if (CHECK_SRC_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE]))
            {
                // Src virtual address
                srcData = malloc(totalSize);
                data.read(srcData, totalSize);
            }
            else
            {   // Src physical address
                srcData = reinterpret_cast<void *>(static_cast<uintptr_t>(data.readInt64()));
            }

            int32_t offset = data.readInt32();
            int32_t numSubSamples = data.readInt32();

            CryptoPlugin::SubSample *subSamples =
                new CryptoPlugin::SubSample[numSubSamples];

            data.read(
                    subSamples,
                    sizeof(CryptoPlugin::SubSample) * numSubSamples);

            void *dstPtr;
            // Dst virtual address
            if (secure || !CHECK_DST_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE])) {
                dstPtr = reinterpret_cast<void *>(static_cast<uintptr_t>(data.readInt64()));
            } else {
                dstPtr = malloc(totalSize);
            }

            AString errorDetailMsg;
            ssize_t result = decrypt(
                    secure,
                    drmMessageData,
                    drmMessageLen,
                    key,
                    iv,
                    mode,
                    srcData, offset,
                    subSamples, numSubSamples,
                    dstPtr,
                    &errorDetailMsg);
            ALOGV("\033[0;32m $$$$$$$$ In %s:%d srcData:%p dstPtr:%p secure:%d result:%zd \033[m", __FUNCTION__, __LINE__, srcData, dstPtr, secure, result);

            reply->writeInt32(result);

            if (result >= ERROR_DRM_VENDOR_MIN
                && result <= ERROR_DRM_VENDOR_MAX) {
                reply->writeCString(errorDetailMsg.c_str());
            }

            if (!secure && CHECK_DST_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE])) {
                if (result >= 0) {
                    CHECK_LE(result, static_cast<ssize_t>(totalSize));
                    reply->write(dstPtr, result);
                }
                free(dstPtr);
                dstPtr = NULL;
            }

            delete[] subSamples;
            subSamples = NULL;

            if (CHECK_SRC_VIR_ADDR(drmMessageData[TRANSFER_ADDR_MODE]))
            {
                // Src virtual address
                free(srcData);
                srcData = NULL;
            }

            return OK;
        }

        case CHECK_PLUGIN_RTK:
        {
            CHECK_INTERFACE(ICrypto, data, reply);

            uint32_t magicNumber = data.readInt32();
            uint32_t retValue = checkPlugin(magicNumber);
            reply->writeInt32(retValue);
            return OK;
        }

        case ENCRYPT_RTK:
        {
            CHECK_INTERFACE(ICrypto, data, reply);
            bool secure = data.readInt32() != 0;
            const void *srcPtr = (void *)(unsigned long)data.readInt64();
            void *dstPtr = (void *)(unsigned long)data.readInt64();
            size_t size = data.readInt32();
            ssize_t result = encrypt(secure, srcPtr, dstPtr, size);
            reply->writeInt32(result);
            return OK;
        }

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}  // namespace android
