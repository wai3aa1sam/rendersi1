#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResourceHandle.h"

namespace rds
{

#if 0
#pragma mark --- rdsBindlessResourceHandle-Impl ---
#endif // 0
#if 1

BindlessResourceHandle::IndexT
BindlessResourceHandle::getResourceIndex() const
{
	RDS_CORE_ASSERT(isValid(), "invalid bindless resource");
	return _hnd;
}

bool 
BindlessResourceHandle::isValid() const
{
	return _hnd != s_kInvalid;
}


#endif

}