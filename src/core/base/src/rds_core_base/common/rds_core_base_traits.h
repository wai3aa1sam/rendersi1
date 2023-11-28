#pragma once

#include "rds_core_base/common/rdsNmsplib_Common.h"

#include "rds_core_base-config.h"

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "rds_core_base/detect_platform/rds_detect_platform.h"
#include "rds_core_base/marco/rds_core_base_marco.h"

#include "rdsDataType_Common.h"
#include "rdsFunction_Common.h"

#include <new>

namespace rds {

#if 0
#pragma mark --- rdsCoreBaseTraits-Impl ---
#endif // 0
#if 1

struct CoreBaseDefaultTraits_T : public NmspCoreBaseTraits
{
public:
	static constexpr SizeType s_kLogicalThreadCount	= NMSP_JOB_SYSTEM_LOGICAL_THREAD_COUNT;

};

#if !RDS_CORE_BASE_CUSTOM_TRAITS

using CoreBaseTraits = CoreBaseDefaultTraits_T;

#else


#endif // 


#endif

}

#define RDS_OVERRIDE_NEW_OP 0
#if RDS_OVERRIDE_NEW_OP

#error("RDS_OVERRIDE_NEW_OP is not support right now")

RDS_NODISCARD void*	operator new  (size_t size);
RDS_NODISCARD void*	operator new[](size_t size);
RDS_NODISCARD void*	operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
RDS_NODISCARD void*	operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);

void	operator delete  (void* ptr)					NMSP_NOEXCEPT;
void	operator delete  (void* ptr, std::size_t size)	NMSP_NOEXCEPT;
void	operator delete[](void* ptr)					NMSP_NOEXCEPT;
void	operator delete[](void* ptr, std::size_t size)	NMSP_NOEXCEPT;

#endif // NMSP_OVERRIDE_NEW