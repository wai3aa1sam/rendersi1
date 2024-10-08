#pragma once

#if 0
#pragma mark --- rds_build_marco-Impl ---
#endif // 0
#if 1

#if _DEBUG
	#define RDS_DEBUG 1
	#define RDS_BUILD_CONFIG_STR "Debug"
#else
	#define RDS_DEBUG 0
	#define RDS_BUILD_CONFIG_STR "Release"
#endif // _DEBUG

#define RDS_DEVELOPMENT		1
#define RDS_ENABLE_ASSERT	1

#if RDS_OS_WINDOWS
	#ifdef RDS_DYNAMIC_LINK
		#ifdef RDS_BUILD_DLL
			#define RDS_API RDS_DLL_EXPORT
		#else
			#define RDS_API RDS_DLL_IMPORT
		#endif // RDS_BUILD_DLL
	#else
		#define RDS_API
	#endif
#else
	#error rds only support Windows!
#endif // RDS_OS_WINDOWS

#endif // 1

