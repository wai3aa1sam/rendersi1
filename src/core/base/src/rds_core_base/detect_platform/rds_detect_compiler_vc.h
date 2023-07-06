#pragma once

/*
references:
- sge_detect_compiler_vc.h in https://github.com/SimpleTalkCpp/SimpleGameEngine
*/


#pragma warning(disable: 4668) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'

#if !RDS_COMPILER_VC	
	#error
#endif

#if _MSVC_LANG >= 199711L
	#define RDS_CPLUSPLUS_03	1
#endif

#if _MSVC_LANG >= 201103L
	#define RDS_CPLUSPLUS_11	1
#endif

#if _MSVC_LANG >= 201402L
	#define RDS_CPLUSPLUS_14	1
#endif

#if _MSVC_LANG >= 201703L
	#define RDS_CPLUSPLUS_17	1
#endif

#if _MSVC_LANG >= 202002L
	#define RDS_CPLUSPLUS_20	1
#endif

#define RDS_TYPE_HAS_SIZE_T	0
#define RDS_TYPE_HAS_SSIZE_T	0
#define RDS_TYPE_HAS_LONGLONG	0

#define RDS_DLL_EXPORT			_declspec(dllexport)
#define RDS_DLL_IMPORT			_declspec(dllimport)

#define	RDS_FUNC_NAME_SZ			__FUNCTION__
#define RDS_FUNC_FULLNAME_SZ		__FUNCSIG__

#define RDS_DEPRECATED			__declspec(deprecated)

#define RDS_COMPILER_VER _MSVC_LANG

#if RDS_CPLUSPLUS_17
	#define RDS_FALLTHROUGH	//	[[fallthrough]]
	#define RDS_NODISCARD		[[nodiscard]]
#else
	#define RDS_FALLTHROUGH
	#define RDS_NODISCARD
#endif

//#define RDS_ALIGN(N)				__declspec(align(N)) 
//#define RDS_ALIGN(N)				alignas(N) //c++11

#define RDS_OPTIMIZE_OFF			__pragma(optimize("", off))

#if 0 // RDS_GEN_CONFIG_Debug
	#define	RDS_INLINE		inline
#else
	#define	RDS_INLINE		__forceinline
#endif

#define RDS_THREAD_LOCAL			__declspec( thread )
//#define RDS_THREAD_LOCAL thread_local //C++11

#if _MSC_VER < 1600
	#define	nullptr	NULL
#endif

// sanitizer support
#if __SANITIZE_ADDRESS__ && _MSC_VER >= 1928
	#define RDS_IS_ASAN_ENABLE 1
#else
	#define RDS_IS_ASAN_ENABLE 0
#endif // __SANITIZE_ADDRESS__

//cpu
#if _M_X64
	#define RDS_CPU_X86_64	1
	#define RDS_CPU_FEATURE_SSE1		1
	#define RDS_CPU_FEATURE_SSE2		1
	#define RDS_CPU_FEATURE_SSE3		1

#elif _M_PPC
	#define RDS_CPU_PowerPC     1
#else
	#define RDS_CPU_x86         1
#endif

//os

#if _WIN64
	#define RDS_OS_WIN64     1
	#define RDS_OS_WINDOWS	1
#elif _WIN32
	#define RDS_OS_WIN32     1
	#define RDS_OS_WINDOWS	1
#elif _WINCE
	#define RDS_OS_WinCE     1
	#define RDS_OS_WINDOWS	1
#endif

// function
#define RDS_DEBUG_BREAK(...)		__debugbreak()

#define RDS_DISABLE_ALL_WARNINGS() \
	__pragma(warning(push, 0)) \
	__pragma(warning(disable: 4172 4275)) \
// ---

#define RDS_RESTORE_ALL_WARNINGS() \
	__pragma(warning(pop)) \
// ---
