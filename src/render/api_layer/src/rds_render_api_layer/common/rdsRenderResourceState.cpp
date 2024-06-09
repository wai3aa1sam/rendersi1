#include "rds_render_api_layer-pch.h"
#include "rdsRenderResourceState.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResourceState-Impl ---
#endif // 0
#if 1

bool 
RenderResourceState::isTransitionNeeded(RenderResourceStateFlags src, RenderResourceStateFlags dst)
{
	bool srcIsRead = RenderResourceStateFlagsUtil::getRenderAccess(src) == RenderAccess::Read;
	bool dstIsRead = RenderResourceStateFlagsUtil::getRenderAccess(dst) == RenderAccess::Read;

	auto srcTexUsage = RenderResourceStateFlagsUtil::getTextureUsageFlags(src);
	auto dstTexUsage = RenderResourceStateFlagsUtil::getTextureUsageFlags(dst);

	auto srcBufUsage = RenderResourceStateFlagsUtil::getBufferUsageFlags(src);
	auto dstBufUsage = RenderResourceStateFlagsUtil::getBufferUsageFlags(dst);

	bool isBothRead		= (srcIsRead && dstIsRead);
	bool isSameTexUsage = (srcTexUsage == dstTexUsage);
	bool isSameBufUsage	= (srcBufUsage == dstBufUsage);

	if (src == dst && isBothRead)						return false;
	if (isBothRead && isSameTexUsage && isSameBufUsage)	return false;

	return true;
}

RenderResourceState::RenderResourceState()
	: _isCommonState(true)
{
}

RenderResourceState::~RenderResourceState()
{

}

void 
RenderResourceState::setSubResourceCount(SizeType n)
{
	RDS_CORE_ASSERT(n <= s_kMaxMipCount);
	_stateFlags.resize(n);
}

void 
RenderResourceState::setState(RenderResourceStateFlags state, u32 subResource)
{
	RDS_CORE_ASSERT(!_stateFlags.is_empty(), "forgot to call setSubResourceCount() ?");

	if (isAllSubResource(subResource))
	{
		_stateFlagsCommon	= state;
		_isCommonState		= true;
		return;
	}

	_notYetSupported(RDS_SRCLOC);
	if (_isCommonState && _stateFlagsCommon == state)
	{
	}

	_stateFlags[subResource]	= state;
	_isCommonState				= false;
}


#endif

}