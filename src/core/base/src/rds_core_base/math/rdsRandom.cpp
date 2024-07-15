#include <rds_core_base-pch.h>
#include "rdsRandom.h"

namespace rds
{
#if 0
#pragma mark --- rdsRandom-Impl ---
#endif // 0
#if 1

Random* 
Random::instance() 
{ 
	static thread_local Random s_instance; 
	return &s_instance; 
}



#endif
}