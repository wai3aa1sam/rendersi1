#pragma once

#if 0
#pragma mark --- rds_build_marco-Impl ---
#endif // 0
#if 1

#if _DEBUG
	#define RDS_DEBUG 1
#else
	#define RDS_DEBUG 0
#endif // _DEBUG


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

