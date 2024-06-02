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
	return _value;
}

BindlessResourceHandle::IndexT  
BindlessResourceHandle::getResourceIndex(u32 mipLevel)  const
{
	RDS_CORE_ASSERT(isValid() && mipLevel < sCast<u32>(_subRscCount), "invalid bindless resource");
	return _value + mipLevel;
}

bool 
BindlessResourceHandle::isValid() const
{
	return _value != s_kInvalid;
}

u32     
BindlessResourceHandle::subResouceCount() const
{
	return _subRscCount;
}

void 
BindlessResourceHandle::setValue(u32 v)
{
	_value = v;
}

void 
BindlessResourceHandle::setSubResourceCount(u32 v)
{
	_subRscCount = sCast<u8>(v);
}

u32 
BindlessResourceHandle::getValue() const
{
	return _value;
}

#endif

}