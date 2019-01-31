#ifndef HARDWARE_API_REALTEK_EXT_H_
#define HARDWARE_API_REALTEK_EXT_H_

#include "OMX_Types.h"

/**
 * Structure used to pass the parent message when the extension
 * index for the 'OMX.realtek.android.index.notifyParentInfo'
 * extension is given.
 *
 *  nSize               : Size of the structure in bytes
 *  nVersion            : OMX specification version information
 *  sComm               : comm name
 *  nPid                : pid
 *  nTid                : tid
 */

typedef struct OMX_VIDEO_PARENT_INFO {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U8 sComm[1024];
    OMX_U32 nPid;
    OMX_U32 nTid;
} OMX_VIDEO_PARENT_INFO;
#define OMX_VIDEO_PARENT_INFO OMX_VIDEO_PARENT_INFO

#endif /* end of HARDWARE_API_REALTEK_EXT_H_ */
