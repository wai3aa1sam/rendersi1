#include "rds_render_api_layer-pch.h"
#include "rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

void 
checkRenderThreadExclusive(RDS_DEBUG_SRCLOC_PARAM)
{
	#if !RDS_USE_RENDER_SINGLE_THREAD_MODE
	throwIf(!RenderApiLayerTraits::isRenderThread() , "only could execute in render thread, {}", RDS_DEBUG_SRCLOC_ARG);
	#endif // RDS_USE_RENDER_SINGLE_THREAD_MODE
}

#endif
}
