/*
// $Id: $
// Copyright (c) 2005 DivX, Inc. http://www.divx.com/corporate
// All rights reserved.
//
// This software is the confidential and proprietary information of DivX
// Inc. ("Confidential Information").  You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the license
// agreement you entered into with DivX, Inc.
*/
#ifndef _DIVXPORTABLECRYPTOAPI_H
#define _DIVXPORTABLECRYPTOAPI_H

/*------------------------------------------------------------------------------------------
 *
 *  LOCAL INCLUDES
 *
 *-----------------------------------------------------------------------------------------*/
#include "DivXInt.h"
#include "DivXNULL.h"


/*------------------------------------------------------------------------------------------
 *
 *  TYPE DECLARATIONS
 *
 *-----------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------
 *
 *  MACROS
 *
 *-----------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------
 *
 *  FUNCTION DECLARATIONS
 *
 *-----------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t DivXPortableCryptoInitialize ( uint32_t* handle );
int32_t DivXPortableCryptoFinalize (  uint32_t* handle );

#ifdef __cplusplus
};
#endif

#endif

