#pragma once

/*
references:
- sge_detect_compiler_gcc.h in https://github.com/SimpleTalkCpp/SimpleGameEngine
*/

#if ! (RDS_COMPILER_CLANG | RDS_COMPILER_GCC)
	#error
#endif

#if __cplusplus > 0L
	#define RDS_LANG_CPP		1
#endif

#if __cplusplus >= 199711L
	#define RDS_CPLUSPLUS_03	1
#endif

#if __cplusplus >= 201103L 
	#define RDS_CPLUSPLUS_11	1
#endif

#if __cplusplus >= 201402L
	#define RDS_CPLUSPLUS_14	1
#endif

#if __cplusplus >= 201703L
	#define RDS_CPLUSPLUS_17	1
#endif

#if __cplusplus >= 202002L
	#define RDS_CPLUSPLUS_20	1
#endif


#if RDS_COMPILER_CLANG
	#define RDS_TYPE_HAS_SIZE_T		0  //linux 
	#define RDS_TYPE_HAS_SSIZE_T		0  //linux 

	#define RDS_OPTIMIZE_OFF			_Pragma("clang optimize off")
#else
	#define RDS_TYPE_HAS_SIZE_T		0  //linux 
	#define RDS_TYPE_HAS_SSIZE_T		0  //linux 

	#define RDS_OPTIMIZE_OFF			_Pragma("GCC optimize(\"O0\")")
#endif

#if __GNUC__ >= 4
	#define	RDS_DLL_EXPORT			__attribute__ ((visibility ("default")))
	#define RDS_DLL_IMPORT			// __attribute__ ((visibility ("hidden")))
#else
	#define	RDS_DLL_EXPORT
	#define RDS_DLL_IMPORT
#endif


// #define nullptr	NULL

#define	RDS_FUNC_NAME_SZ				__FUNCTION__
#define RDS_FUNC_FULLNAME_SZ			__PRETTY_FUNCTION__

#define RDS_DEPRECATED( f )		f __attribute__( (deprecated) )

#define RDS_COMPILER_VER __cplusplus

#if RDS_CPLUSPLUS_17
	#define RDS_FALLTHROUGH		// [[fallthrough]]
	#define RDS_NODISCARD		[[nodiscard]]
#else
	#define RDS_FALLTHROUGH
	#define RDS_NODISCARD
#endif

//#define RDS_ALIGN(N)				__attribute__((aligned(N)))
//#define RDS_ALIGN(N)				alignas(N) //c++11

#if RDS_GEN_CONFIG_Debug
	#define	RDS_INLINE      inline
#else
	#define	RDS_INLINE		inline //__attribute__( (always_inline) )
#endif

//#define RDS_THREAD_LOCAL	__thread
#define RDS_THREAD_LOCAL thread_local //C++11

// sanitizer support
#if defined(__has_feature)
	#if __has_feature(address_sanitizer)
		#define RDS_IS_ASAN_ENABLE 1
	#else
		#define RDS_IS_ASAN_ENABLE 0
	#endif
#else
	#define RDS_IS_ASAN_ENABLE 0
#endif

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__)
	#define RDS_CPU_X86_64      1

#elif defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
	#define RDS_CPU_X86         1

#elif defined(__POWERPC__)
	#define RDS_CPU_POWERPC     1

#elif defined(__aarch64__)
	#define RDS_CPU_ARM64 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define RDS_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define RDS_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif

#elif defined(__arm__)
	#define RDS_CPU_ARM 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define RDS_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define RDS_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif
#endif

#if	defined(__ARM_NEON__) || defined(__ARM_NEON)
	#define RDS_CPU_FEATURE_ARM_NEON
#endif

#if __SSE__
	#define RDS_CPU_FEATURE_SSE1			1
#endif

#if __SSE2__
	#define RDS_CPU_FEATURE_SSE2			1
	#include <xmmintrin.h>
#endif

#if __SSE3__
	#define RDS_CPU_FEATURE_SSE3			1
#endif

#if __SSSE3__
	#define RDS_CPU_FEATURE_SSSE3		1
#endif

#if __SSE4_1__
	#define RDS_CPU_FEATURE_SSE4_1		1
#endif

#if __SSE4_2__
	#define RDS_CPU_FEATURE_SSE4_2		1
#endif

//os

#if _WIN64
	#define RDS_OS_WIN64     1
	#define RDS_OS_WINDOWS	1

#elif _WIN32
	#define RDS_OS_WIN32     1
	#define RDS_OS_WINDOWS	1

#elif __ANDROID__
	#define RDS_OS_ANDROID	1

#elif __linux
	#define RDS_OS_LINUX        1

#elif __FreeBSD__
	#define RDS_OS_FREEBSD		1

#elif __APPLE__ && __MACH__
	#include <TargetConditionals.h>
	#if (TARGET_OF_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
		#define RDS_OS_IOS		1
	#else
		#define RDS_OS_MACOSX	1
	#endif

#elif __sun
	#define RDS_OS_SOLARIS		1

#elif __CYGWIN__
    #define RDS_OS_CYGWIN        1

#elif __MINGW32__
	#define RDS_OS_MINGW			1
#endif

// function
#include <signal.h>
#define RDS_DEBUG_BREAK(...)		raise(SIGTRAP)
