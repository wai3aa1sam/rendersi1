#include "rds_core-pch.h"
#include "rds_core_common.h"


namespace rds
{

void 
checkMainThreadExclusive(RDS_DEBUG_SRCLOC_PARAM)
{
	throwIf(!OsTraits::isMainThread(), "only could execute in main thread, {}", RDS_DEBUG_SRCLOC_ARG);
}

}