#pragma once

#if 0

#include "rds_core_base-config.h"
#include "rds_core_base_traits.h"

#if !EASTL_DLL // If building a regular library and not building EASTL as a DLL...
// It is expected that the application define the following
// versions of operator new for the application. Either that or the
// user needs to override the implementation of the allocator class.
inline void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) { return malloc(size); }
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) 
	{
		#if RDS_OS_WINDOWS
		return _aligned_malloc(size, alignment);
		#else
		return std::aligned_alloc(alignment, size);
		#endif
	}
	#endif


#endif // 0


#include "rdsNmsplib_Common.h"

#include "rds_core_base-config.h"
#include "rds_core_base_traits.h"

#include "../detect_platform/rds_detect_platform.h"

#include "rds_core_base/marco/rds_core_base_marco.h"

#include "rdsDataType_Common.h"
#include "rdsEnum_Common.h"
#include "rdsError_Common.h"
#include "rdsFunction_Common.h"
#include "rdsTypeTraits_Common.h"
#include "rdsClass_Common.h"
#include "rdsUtil_Common.h"
#include "rdsFmt_Common.h"

#include "rds_core_base/memory/rds_memory.h"


namespace rds
{
#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1


#endif
}

namespace rds 
{
#if 0
#pragma mark --- BitUtil-Impl ---
#endif // 0
#if 1
using ::nmsp::BitUtil;
#endif

using MemoryLeakReportScope = ::nmsp::MemoryLeakReportScope;

}

