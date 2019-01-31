/*
// $Id: base64.h 450 2006-01-13 02:31:16Z egrab $
// Copyright (c) 2005 DivX, Inc. http://www.divx.com/corporate
// All rights reserved.
//
// This software is the confidential and proprietary information of DivX
// Inc. ("Confidential Information").  You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the license
// agreement you entered into with DivX, Inc.
*/

#ifndef BASE_64_H_INCLUDE
#define BASE_64_H_INCLUDE

//
// Base-64 encode & decode
// 
#ifdef __cplusplus
extern "C" {
#endif

char *base64Encode(char *buffer, 
				   unsigned int length, 
				   unsigned int *outlength);

char *base64Decode(char *str,
				   unsigned int length,
				   unsigned int *outlength);

int validBase64Data(char *str, unsigned int length);
int validBase32Data(const char *str, unsigned int length);
//
// Base-32 encode & decode
// 
void base32Encode40Bits(const unsigned char *input, char *output);
void base32Decode40Bits(const char *input, unsigned char *output);

#ifdef __cplusplus
};
#endif

#endif
//BASE_64_H_INCLUDE
