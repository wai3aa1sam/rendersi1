#pragma once

#include "rds_core/common/rds_core_common.h"

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{



}
#endif

#if 0
#pragma mark --- rds_profiler_marco-Impl ---
#endif // 0
#if 1

#if RDS_ENABLE_PROFILER

	#define RDS_PROFILE_SCOPED()					NMSP_PROFILE_SCOPED()
	#define RDS_PROFILE_FRAME()						NMSP_PROFILE_FRAME()
	#define RDS_PROFILE_SECTION(NAME)				NMSP_PROFILE_SECTION(NAME)
	#define RDS_PROFILE_TAG(STR)					NMSP_PROFILE_TAG(STR)
	#define RDS_PROFILE_LOG(TEXT)					NMSP_PROFILE_LOG(TEXT)
	#define RDS_PROFILE_VALUE(TEXT, VALUE)			NMSP_PROFILE_VALUE(TEXT, VALUE)

	#define RDS_PROFILE_ALLOC(PTR, SIZE)			NMSP_PROFILE_ALLOC(PTR, SIZE)
	#define RDS_PROFILE_FREE(PTR, SIZE)				NMSP_PROFILE_FREE(PTR, SIZE)

	#define RDS_PROFILE_SET_THREAD_NAME(NAME)		NMSP_PROFILE_SET_THREAD_NAME(NAME)

#else

	#define RDS_PROFILE_SCOPED()					
	#define RDS_PROFILE_FRAME()						
	#define RDS_PROFILE_SECTION(NAME)				
	#define RDS_PROFILE_TAG(STR)					
	#define RDS_PROFILE_LOG(TEXT)					
	#define RDS_PROFILE_VALUE(TEXT, VALUE)			

	#define RDS_PROFILE_ALLOC(PTR, SIZE)			
	#define RDS_PROFILE_FREE(PTR, SIZE)				

	#define RDS_PROFILE_SET_THREAD_NAME(NAME)		

#endif // RDS_ENABLE_PROFILER

#endif


