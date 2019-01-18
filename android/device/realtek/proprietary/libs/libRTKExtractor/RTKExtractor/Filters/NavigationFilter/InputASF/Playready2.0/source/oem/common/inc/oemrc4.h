/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMRC4_H__
#define __OEMRC4_H__

#include <drmcommon.h>

ENTER_PK_NAMESPACE;

#define RC4_TABLESIZE 256

/* Key structure */
typedef struct __tagRC4_KEYSTRUCT
{
    DRM_BYTE S[__CB_DECL(RC4_TABLESIZE)];     /* State table */
    DRM_BYTE i, j;        /* Indices */
} RC4_KEYSTRUCT;

DRM_API DRM_VOID DRM_CALL DRM_RC4_ZeroKey(
    __inout           RC4_KEYSTRUCT    *pKS );

/*********************************************************************
**
**  Function:  DRM_RC4_KeySetup
**
**  Synopsis:  Generate the key control structure.  Key can be any size.
**
**  Arguments:  
**     [pKS] -- A KEYSTRUCT structure that will be initialized.
**     [cbKey] -- Size of the key, in bytes.
**     [pbKey] -- Pointer to the key.
**
**  Returns:  None
**
*********************************************************************/

DRM_API DRM_VOID DRM_CALL DRM_RC4_KeySetup(
    OUT       RC4_KEYSTRUCT  *pKS,
    IN        DRM_DWORD       cbKey,
    IN  const DRM_BYTE       *pbKey );

/*********************************************************************
**
**  Function:  DRM_RC4_Cipher
**
**  Synopsis:  
**
**  Arguments:  
**     [pKS] -- Pointer to the KEYSTRUCT created using DRM_RC4_KeySetup.
**     [cbBuffer] -- Size of buffer, in bytes.
**     [pbBuffer] -- Buffer to be encrypted in place.
**
**  Returns:  None
*********************************************************************/

DRM_API DRM_VOID DRM_CALL DRM_RC4_Cipher(
    IN OUT                   RC4_KEYSTRUCT *pKS,
    IN                       DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer );

EXIT_PK_NAMESPACE;

#endif /* __OEMRC4_H__ */

