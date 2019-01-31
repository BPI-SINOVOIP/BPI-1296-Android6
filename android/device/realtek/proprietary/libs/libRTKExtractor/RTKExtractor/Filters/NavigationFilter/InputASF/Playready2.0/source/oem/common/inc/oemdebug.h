/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMDEBUG_H__
#define __OEMDEBUG_H__

#include <drmnamespace.h>
#include <drmtypes.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_VOID DRM_CALL Oem_Debug_Trace( 
    __in const DRM_CHAR *szFmt,
    ... );

DRM_API DRM_VOID DRM_CALL Oem_Debug_HookTrace(
    __in DRM_VOID *pNewTraceFcn );

DRM_API DRM_VOID DRM_CALL Oem_Debug_Assert
(
    __in       DRM_BOOL fAssert, 
    __in const DRM_CHAR *assertcmd, 
    __in const DRM_CHAR *file, 
    __in       DRM_LONG line
);

DRM_API DRM_VOID DRM_CALL Oem_Debug_HookAssert(
    __in DRM_VOID *pNewAssertFcn );


EXIT_PK_NAMESPACE;

#endif /* __OEMDEBUG_H__ */




