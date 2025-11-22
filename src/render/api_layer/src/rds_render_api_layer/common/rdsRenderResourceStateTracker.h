#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderResourceState.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResourceStateTracker-Decl ---
#endif // 0
#if 1

class RenderResourceStateMiniTracker : public NC_RenderApiLayerCommon_Base
{
public:
	using PendingState = RenderResourceStateFlags;
	using PendingStates = VectorMap<RenderResource*, PendingState>;


protected:
	VectorMap<RenderResource*, PendingState> _pendingStates;
};


class RenderResourceStateTracker : public NC_RenderApiLayerCommon_Base
{
public:
	using PendingState	= RenderResourceState;
	using PendingStates = VectorMap<RenderResource*, PendingState>;


protected:
	VectorMap<RenderResource*, PendingState> _pendingStates;
};


#endif

}