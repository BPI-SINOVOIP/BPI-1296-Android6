/*
 * Copyright (c) 2014, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TEE_API_H
#define TEE_API_H

/* This UUID is generated with uuidgen
   the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html */
#define UUID { 0x7aaaf200, 0x2450, 0x11e4, \
		{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define TA_SUCCESS 1
#define TA_FAILED  0


#define SET_PROTECT 1
#define UNSET_PROTECT 0
#define TA_TEE_TEST 5

#define REG_MMAP_OFFSET 0x1800B000
		
/* The TAFs ID implemented in this TA */

enum CMD_FOR_TA
{
	TA_TEE_MEMCPY 			= 0,
	TA_TEE_SET_PROTECT_MEM	= 1,
};

#define AES_BLOCK_SIZE 16


#define MEM_PROTECT_CONTROL_RIGISTER 0x18008400	
#define TOTAL_SETS_OF_PROTECT_MEM 8

int TEE_API_Initialize(TEEC_Context *ctx, TEEC_Session *sess);
void TEE_API_Finalize(TEEC_Context *ctx, TEEC_Session *sess);
int TEE_API_memcpy(TEEC_Session *sess, unsigned int dstPAddr, unsigned int srcPAddr, int size);
int TEE_API_SetProtectedMem(TEEC_Session *sess, unsigned int startPAddr, unsigned int size, unsigned int setProtect);



// for test used
void test_TEE_API_memcpy(void);
void test_TEE_API_SetProtectedMem(void);

//Realtek OTP
#define RTK_UUID_READ			0xf0
#define RTK_UUID_WRITE			0xf1

int TEE_API_ReadOTP(TEEC_Session *sess, unsigned int startbit, unsigned int bytes, unsigned char *buf);
int TEE_API_WriteOTP(TEEC_Session *sess, int startbit, int bytes, unsigned char *buf);
#endif /*TEE_API_H*/
