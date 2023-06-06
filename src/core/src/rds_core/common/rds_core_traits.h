#pragma once

#include <rds_core/common/rds_core_traits.h>
#include "rds_core-config.h"

namespace rds 
{
#if 0
#pragma mark --- CoreTraits-Impl ---
#endif // 0
#if 1
struct CoreDefaultTraits_T : public CoreBaseTraits
{
public:
	using Base = CoreBaseTraits;


public:

};


#if !RDS_RENDER_CUSTOM_TRAITS

using CoreTraits = CoreDefaultTraits_T;

#else

#endif // 

#endif

}
