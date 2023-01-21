#pragma once

/*
reference:
https://github.com/SimpleTalkCpp/SimpleGameEngine
*/

/*
RDS_DetectPlatform:
	- headers only
	- detect compiler / OS / CPU
	- may be used by external lib, so please keep it simple
*/

#ifdef __OBJC__ // Objective C
	#define RDS_OBJC		1
#else
	#define RDS_OBJC		0
#endif

//=========== Detect COMPILER ===============
#if defined(__clang__) 
	#define RDS_COMPILER_CLANG	1
	#include "rds_detect_compiler_gcc.h"

#elif defined(__GNUC__)
	#define RDS_COMPILER_GCC		1
	#include "rds_detect_compiler_gcc.h"

#elif defined(_MSC_VER)
	#define RDS_COMPILER_VC		1
	#include "rds_detect_compiler_vc.h"

#endif

#if RDS_COMPILER_VC + RDS_COMPILER_GCC + RDS_COMPILER_CLANG != 1
    #error "Compiler should be specified"
#endif

//======== Detect CPU =============

// check CPU define
#if RDS_CPU_X86_64 + RDS_CPU_X86 + RDS_CPU_POWERPC + RDS_CPU_ARM + RDS_CPU_ARM64 != 1
	#error "CPU should be specified"
#endif

#if RDS_CPU_X86_64
	#define RDS_CPU_LP64				1
	#define RDS_CPU_ENDIAN_LITTLE	1
	#define RDS_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if RDS_CPU_X86
	#define RDS_CPU_LP32				1
	#define RDS_CPU_ENDIAN_LITTLE	1
	#define RDS_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if RDS_CPU_POWERPC
	#define RDS_CPU_LP32				1
	#define RDS_CPU_ENDIAN_BIG		1
	#define RDS_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if RDS_CPU_ARM64
	#define RDS_CPU_LP64				1
	#define RDS_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if RDS_CPU_ARM
	#define RDS_CPU_LP32				1
	#define RDS_CPU_MISALIGNED_MEMORY_ACCESSES	1
#endif

#if RDS_CPU_LP32 + RDS_CPU_LP64 != 1
	#error "CPU bits should be specified"
#endif

#if RDS_CPU_LP32
	#define RDS_SIZEOF_POINTER		4
#endif

#if RDS_CPU_LP64
	#define RDS_SIZEOF_POINTER		8
#endif

#ifndef RDS_SIZEOF_POINTER
	#error "Unknown RDS_SIZEOF_POINTER"
#endif

#if RDS_CPU_ENDIAN_BIG + RDS_CPU_ENDIAN_LITTLE != 1
	#error "CPU endian should be specified"
#endif

//======== Detect OS ===============

#if RDS_OS_WIN32 + RDS_OS_WIN64 + RDS_OS_WINCE \
	+ RDS_OS_FREEBSD + RDS_OS_LINUX \
	+ RDS_OS_ANDROID \
	+ RDS_OS_MACOSX  + RDS_OS_IOS \
	+ RDS_OS_CYGWIN  + RDS_OS_MINGW != 1
	#error "OS should be specified"
#endif



//====================================

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif
