//------------------------------------------------------------------------------
// File: vputypes.h
//
// Copyright (c) 2006, Realtek.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _JPU_TYPES_H_
#define _JPU_TYPES_H_

typedef unsigned int    UINT;
typedef unsigned char	Uint8;
typedef unsigned int	Uint32;
typedef unsigned short	Uint16;
typedef char	        Int8;
typedef int	            Int32;
typedef short	        Int16;
#if defined(_MSC_VER)
typedef unsigned _int64 Uint64;
typedef _int64 Int64;
#else
typedef unsigned long long Uint64;
typedef long long Int64;
#endif
typedef Uint32 PhysicalAddress;
#ifdef ANDROID
typedef unsigned char   BYTE;
typedef int BOOL;
#endif

#ifndef HAVE_STDINT_H

typedef unsigned char	uint8_t;
typedef unsigned int	uint32_t;
typedef unsigned short	uint16_t;
#ifdef ANDROID
typedef int	int32_t;
typedef short	int16_t;
#endif
#if defined(_MSC_VER)
typedef unsigned _int64 uint64_t;
typedef _int64 int64_t;
#else
typedef unsigned long long uint64_t;
typedef long long int64_t;
#endif

#ifndef __int8_t_defined
typedef signed char			int8_t;
#endif

typedef int	            int32_t;
typedef short	        int16_t;



#ifndef BYTE
typedef unsigned char   BYTE;
#endif

#ifndef BOOL
typedef int BOOL;
#endif
#endif

#ifndef NULL
#define NULL	0
#endif

#if defined(linux) || defined(__linux) || defined(ANDROID)
#define TRUE						1
#define FALSE						0
#endif



#endif	/* _JPU_TYPES_H_ */
