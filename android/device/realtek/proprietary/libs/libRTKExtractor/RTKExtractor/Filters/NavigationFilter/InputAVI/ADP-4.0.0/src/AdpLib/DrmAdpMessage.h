/*
// $Id: DrmAdpMessage.h 783 2006-03-29 19:13:54Z egrab $
// Copyright (c) 2005 DivX, Inc. http://www.divx.com/corporate
// All rights reserved.
//
// This software is the confidential and proprietary information of DivX
// Inc. ("Confidential Information").  You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the license
// agreement you entered into with DivX, Inc.
*/

/*
These functions and structures allow representation and manipulation
of the challenge response messages used by the ADP DRM.
*/

#ifndef DRMADPMESSAGE_H_INCLUDE
#define DRMADPMESSAGE_H_INCLUDE

#include "DrmAdpTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void packRegistrationRequest(RegistrationRequest request, DrmMessagePacked *messagePacked);
void packRegistrationLogic(RegistrationRequest *request, DrmMessagePacked *messagePacked, uint8_t direction);
void registrationSecuritySwap(DrmMessagePacked *messagePacked);
void wrapSecureMessage(DrmMessagePacked *transformMessage);
void unwrapSecureMessage(DrmMessagePacked *transformMessage);

#ifdef __cplusplus
};
#endif

#endif
// DRMADPMESSAGE_H_INCLUDE
