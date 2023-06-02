#pragma once

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


struct CoreBaseDefaultTraits_T : public ::nmsp::CoreBaseTraits
{
	
};

#if !RDS_CORE_BASE_CUSTOM_TRAITS

using CoreBaseTraits = CoreBaseDefaultTraits_T;

#else


#endif // 


#endif

}

