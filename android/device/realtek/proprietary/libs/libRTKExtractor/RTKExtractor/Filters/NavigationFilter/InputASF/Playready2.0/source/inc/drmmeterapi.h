/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_METER_API_H
#define __DRM_METER_API_H

#include <drmmetering.h>
#include <drmmanager.h>

ENTER_PK_NAMESPACE;

/**********************************************************************
**
** Function:    Drm_MeterCert_InitEnum
**
** Synopsis:    Initializes an enumeration context to enumerate through all metering certificates stored
**
** Arguments:   [pAppContext]           -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [pMeterCertEnumContext] -- Meter cert enumeration context to be initialized
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_InitEnum(
    __in    DRM_APP_CONTEXT     *pAppContext,
    __out   DRM_METERCERT_ENUM  *pMeterCertEnumContext );

/**********************************************************************
**
** Function:    Drm_MeterCert_EnumNext
**
** Synopsis:    Enumerates to the next metering cert in the metering cert store
**
** Arguments:   [f_pMeterCertEnumContext] -- Meter cert enumeration context
**              [f_pmid]                  -- Buffer to hold the meter ID of the enumerated meter cert
**              [f_pwszLAINFO]            -- Buffer to hold the enumerated LAINFO
**              [f_pcchLAINFO]            -- On input specifies the number of wide characters available in the buffer,
**                                           on output specifies the number of characters in the returned LAINFO
**              [f_pfIsV2]                -- Returns whether the server pointed to by the LAINFO URL is a V2 or V4 server (ie WMDRM-PD or PlayReady )
**              [f_pbMeterCert]           -- Buffer to hold the enumerated meter cert
**              [f_pcbMeterCert]          -- On input specifies the number of bytes available in the buffer,
**                                           on output specifies the number of bytes in the returned meter cert
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_EnumNext(
    __in                                DRM_METERCERT_ENUM  *pMeterCertEnumContext,
    __inout                             DRM_MID             *pmid,
    __out_ecount_opt( *f_cchLAINFO )    DRM_WCHAR           *pwszLAINFO,
    __out_opt                           DRM_DWORD           *f_cchLAINFO,
    __out_opt                           DRM_BOOL            *f_pfIsV2,
    __out_bcount_opt( *f_pcbMeterCert ) DRM_BYTE            *f_pbMeterCert,
    __out_opt                           DRM_DWORD           *f_pcbMeterCert );

/**********************************************************************
**
** Function:    Drm_MeterCert_Update
**
** Synopsis:    Updates a metering certificate in the metering certificate store
**
** Arguments:   [pAppContext]     -- initialized DRM Application context
**              [pbMeterCert]     -- meter cert to store
**              [cbMeterCert]     -- size of meter cert to store.
**              [pmid]            -- optional pointer to MID that was added/updated
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Update(
    __in                        DRM_APP_CONTEXT    *pAppContext,
    __in_bcount( cbMeterCert )  DRM_BYTE           *pbMeterCert,
    __in                        DRM_DWORD           cbMeterCert,
    __out_opt                   DRM_MID            *pmid);

/**********************************************************************
**
** Function:    Drm_MeterCert_Invalidate
**
** Synopsis:    Invalidates a metering certificate
**
** Arguments:   [pAppContext]  -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [pmid]         -- Meter ID of the meter cert to invalidate
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Invalidate(
    __in      DRM_APP_CONTEXT  *pAppContext,
    __in      DRM_MID          *pmid );

/**********************************************************************
**
** Function:    Drm_MeterCert_Delete
**
** Synopsis:    Deletes a metering certificate
**
** Arguments:   [pAppContext]  -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [pmid]         -- Meter ID of the meter cert to delete
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Delete(
    __in      DRM_APP_CONTEXT  *pAppContext,
    __in      DRM_MID          *pmid );

DRM_API DRM_RESULT DRM_CALL Drm_Metering_GenerateChallenge(
    __in                                DRM_APP_CONTEXT  *f_poAppContext,
    __in_bcount( f_cbMeterCert ) const  DRM_BYTE         *f_pbMeterCert,
    __in                                DRM_DWORD         f_cbMeterCert,
    __in_ecount_opt( f_cchCustomData )  DRM_CHAR         *f_pchCustomData,
    __in_opt                            DRM_DWORD         f_cchCustomData,
    __out_ecount_opt( *f_pcchURL )      DRM_CHAR         *f_pchURL,
    __inout_opt                         DRM_DWORD        *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE         *f_pbChallenge,
    __inout                             DRM_DWORD        *f_pcbChallenge );

DRM_API DRM_RESULT DRM_CALL Drm_Metering_ProcessResponse(
    __in                        DRM_APP_CONTEXT  *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE         *f_pbResponse,
    __in                        DRM_DWORD         f_cbResponse,
    __out                       DRM_DWORD        *f_pfFlags );

DRM_API DRM_RESULT DRM_CALL DRM_METERING_UpdateData(
    __in                        DRM_APP_CONTEXT   *f_poAppContext,
    __in                  const DRM_CONST_STRING **f_ppdstrActions,
    __in                        DRM_DWORD          f_cActions );

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_GenerateChallenge(
    __in                                    DRM_APP_CONTEXT *f_poAppContext,
    __in                                    DRM_MID         *f_poMID,
    __in_ecount_opt( f_cchCustomData) const DRM_CHAR        *f_pchCustomData,
    __in_opt                                DRM_DWORD        f_cchCustomData,
    __out_bcount_opt( *f_pcbChallenge )     DRM_BYTE        *f_pbChallenge,
    __inout                                 DRM_DWORD       *f_pcbChallenge );

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_ProcessResponse(
    __in                        DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    __in                        DRM_DWORD        f_cbResponse,
    __out                       DRM_RESULT      *f_pResult );

EXIT_PK_NAMESPACE;

#endif /* __DRM_METER_API_H */
