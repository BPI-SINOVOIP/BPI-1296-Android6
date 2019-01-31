/*
// $Id: rsadecrypt.h 1250 2006-06-06 22:35:28Z egrab $
// Copyright (c) 2005 DivX, Inc. http://www.divx.com/corporate
// All rights reserved.
//
// This software is the confidential and proprietary information of DivX
// Inc. ("Confidential Information").  You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the license
// agreement you entered into with DivX, Inc.
*/

#ifndef RSA_H_INCLUDE
#define RSA_H_INCLUDE

#include "DivXInt.h"

#define RSA_SIZE_NHEX_STRING     513  // This is also the max hex message that can be handled.
#define RSA_SIZE_EHEX_STRING     17
#define RSA_SIZE_CIPHERED_BYTES  256
#define RSA_SIZE_PLAIN_BYTES     255 

#ifdef __cplusplus
extern "C" {
#endif

// Decrypts 256 bytes of data.  Returns 0 if successful.
uint8_t rsaPublicDecrypt255(char nHex[RSA_SIZE_NHEX_STRING],
							char eHex[RSA_SIZE_EHEX_STRING],
							uint8_t ciphered[RSA_SIZE_CIPHERED_BYTES],
							uint8_t plain[RSA_SIZE_PLAIN_BYTES]);

#ifdef __cplusplus
};
#endif

#endif
// RSA_H_INCLUDE
