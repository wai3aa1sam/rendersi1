#pragma once

#include "rds_core_base/common/rds_core_base_common.h"

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

#define RDS_PROFILER_ENABLE_DYNAMIC 0

#if RDS_ENABLE_PROFILER

	#define RDS_PROFILE_CALLSTACK_DEPTH							NMSP_PROFILE_CALLSTACK_DEPTH

	#define RDS_PROFILE_FRAME()									NMSP_PROFILE_FRAME()

	#define RDS_PROFILE_SECTION_IMPL(MSrcLocData, ...)			NMSP_PROFILE_SECTION_IMPL(MSrcLocData, __VA_ARGS__)

	#define RDS_PROFILE_SCOPED()								NMSP_PROFILE_SCOPED()
	#define RDS_PROFILE_SECTION(NAME)							NMSP_PROFILE_SECTION(NAME)

	#if RDS_PROFILER_ENABLE_DYNAMIC
	
		#define RDS_PROFILE_DYNAMIC_SCOPED()					NMSP_PROFILE_DYNAMIC_SCOPED()
		#define RDS_PROFILE_DYNAMIC_SECTION(NAME)				NMSP_PROFILE_DYNAMIC_SECTION(NAME)
		#define RDS_PROFILE_DYNAMIC_FMT(FMT, ...)				RDS_FMT_VAR(tstr, TempString, FMT, __VA_ARGS__); NMSP_PROFILE_DYNAMIC_SECTION(RDS_VAR_NAME(tstr).c_str())
	
	#else
	
		#define RDS_PROFILE_DYNAMIC_SCOPED()			
		#define RDS_PROFILE_DYNAMIC_SECTION(NAME)		
		#define RDS_PROFILE_DYNAMIC_FMT(FMT, ...)				RDS_FMT_VAR(tstr, TempString, FMT, __VA_ARGS__); NMSP_PROFILE_SECTION(FMT)

	#endif // RDS_PROFILER_ENABLE_DYNAMIC

	#define RDS_PROFILE_TAG(STR)								NMSP_PROFILE_TAG(STR)
	#define RDS_PROFILE_LOG(TEXT)								NMSP_PROFILE_LOG(TEXT)
	#define RDS_PROFILE_VALUE(TEXT, VALUE)						NMSP_PROFILE_VALUE(TEXT, VALUE)

	#define RDS_PROFILE_ALLOC(PTR, SIZE)						NMSP_PROFILE_ALLOC(PTR, SIZE)
	#define RDS_PROFILE_FREE(PTR, SIZE)							NMSP_PROFILE_FREE(PTR, SIZE)

	#define RDS_PROFILE_SET_THREAD_NAME(NAME)					NMSP_PROFILE_SET_THREAD_NAME(NAME)

#else

	#define RDS_PROFILE_CALLSTACK_DEPTH				0

	#define RDS_PROFILE_FRAME()						

	#define RDS_PROFILE_SCOPED()					
	#define RDS_PROFILE_SECTION(NAME)				
	#define RDS_PROFILE_DYNAMIC_SCOPED()			
	#define RDS_PROFILE_DYNAMIC_SECTION(NAME)
	#define RDS_PROFILE_DYNAMIC_FMT(FMT, ...)

	#define RDS_PROFILE_TAG(STR)					
	#define RDS_PROFILE_LOG(TEXT)					
	#define RDS_PROFILE_VALUE(TEXT, VALUE)			

	#define RDS_PROFILE_ALLOC(PTR, SIZE)			
	#define RDS_PROFILE_FREE(PTR, SIZE)				

	#define RDS_PROFILE_SET_THREAD_NAME(NAME)		

#endif // RDS_ENABLE_PROFILER


namespace rds
{

//using PColor = ::nmsp::PColor;

}

#endif


