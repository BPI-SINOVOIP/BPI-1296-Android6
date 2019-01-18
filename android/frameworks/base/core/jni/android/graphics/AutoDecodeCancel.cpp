#include "AutoDecodeCancel.h"
#include "SkMutex.h"

SK_DECLARE_STATIC_MUTEX(gAutoDecoderCancelMutex);
static AutoDecoderCancel* gAutoDecoderCancel;
#ifdef SK_DEBUG
static int gAutoDecoderCancelCount;
#endif

AutoDecoderCancel::AutoDecoderCancel(jobject joptions,
                                       SkImageDecoder* decoder) {
    fJOptions = joptions;
    fDecoder = decoder;
    fPixelRef = NULL;

    if (NULL != joptions) {
        SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

        // Add us as the head of the list
        fPrev = NULL;
        fNext = gAutoDecoderCancel;
        if (gAutoDecoderCancel) {
            gAutoDecoderCancel->fPrev = this;
        }
        gAutoDecoderCancel = this;

        SkDEBUGCODE(gAutoDecoderCancelCount += 1;)
        Validate();
    }
}

AutoDecoderCancel::AutoDecoderCancel(jobject joptions,
                                       SkImageDecoder* decoder, SkPixelRef* pixelRef) {
    fJOptions = joptions;
    fDecoder = decoder;
    fPixelRef = pixelRef;

    if (NULL != joptions) {
        SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

        // Add us as the head of the list
        fPrev = NULL;
        fNext = gAutoDecoderCancel;
        if (gAutoDecoderCancel) {
            gAutoDecoderCancel->fPrev = this;
        }
        gAutoDecoderCancel = this;

        SkDEBUGCODE(gAutoDecoderCancelCount += 1;)
        Validate();
    }
}


AutoDecoderCancel::~AutoDecoderCancel() {
    if (NULL != fJOptions) {
        SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

        // take us out of the dllist
        AutoDecoderCancel* prev = fPrev;
        AutoDecoderCancel* next = fNext;

        if (prev) {
            SkASSERT(prev->fNext == this);
            prev->fNext = next;
        } else {
            SkASSERT(gAutoDecoderCancel == this);
            gAutoDecoderCancel = next;
        }
        if (next) {
            SkASSERT(next->fPrev == this);
            next->fPrev = prev;
        }

        SkDEBUGCODE(gAutoDecoderCancelCount -= 1;)
        Validate();
    }
}

bool AutoDecoderCancel::RequestCancel(jobject joptions) {
    SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

    Validate();

    AutoDecoderCancel* pair = gAutoDecoderCancel;
    while (pair != NULL) {
        if (pair->fJOptions == joptions) {
            if (pair->fDecoder)
                pair->fDecoder->cancelDecode();
            if (pair->fPixelRef)
                pair->fPixelRef->cancelDecode();
            return true;
        }
        pair = pair->fNext;
    }
    return false;
}

bool AutoDecoderCancel::SetPixelRef(jobject joptions, SkPixelRef* pixelRef) {
    SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

    Validate();

    AutoDecoderCancel* pair = gAutoDecoderCancel;
    while (pair != NULL) {
        if (pair->fJOptions == joptions) {
            pair->fPixelRef = pixelRef;
            return true;
        }
        pair = pair->fNext;
    }

    return false;
}

bool AutoDecoderCancel::SetPixelRef(JNIEnv* env, jobject joptions, SkPixelRef* pixelRef) {
    SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

    Validate();

    AutoDecoderCancel* pair = gAutoDecoderCancel;
    while (pair != NULL) {
        if(!env->IsSameObject(NULL, pair->fJOptions) && env->IsSameObject(joptions, pair->fJOptions)) {
            pair->fPixelRef = pixelRef;
            return true;
        }
        pair = pair->fNext;
    }

    return false;
}

bool AutoDecoderCancel::RequestCancel(JNIEnv* env, jobject joptions) {
    SkAutoMutexAcquire ac(gAutoDecoderCancelMutex);

    Validate();

    AutoDecoderCancel* pair = gAutoDecoderCancel;
    while (pair != NULL) {
//        if (pair->fJOptions == joptions) {
        if(!env->IsSameObject(NULL, pair->fJOptions) && env->IsSameObject(joptions, pair->fJOptions)) {
            SkDebugf("Request cancel decode");
            if (pair->fDecoder)
                pair->fDecoder->cancelDecode();
            if (pair->fPixelRef)
                pair->fPixelRef->cancelDecode();
            return true;
        }
        pair = pair->fNext;
    }
    return false;
}

#ifdef SK_DEBUG
// can only call this inside a lock on gAutoDecoderCancelMutex
void AutoDecoderCancel::Validate() {
    const int gCount = gAutoDecoderCancelCount;

    if (gCount == 0) {
        SkASSERT(gAutoDecoderCancel == NULL);
    } else {
        SkASSERT(gCount > 0);

        AutoDecoderCancel* curr = gAutoDecoderCancel;
        SkASSERT(curr);
        SkASSERT(curr->fPrev == NULL);

        int count = 0;
        while (curr) {
            count += 1;
            SkASSERT(count <= gCount);
            if (curr->fPrev) {
                SkASSERT(curr->fPrev->fNext == curr);
            }
            if (curr->fNext) {
                SkASSERT(curr->fNext->fPrev == curr);
            }
            curr = curr->fNext;
        }
        SkASSERT(count == gCount);
    }
}
#endif
