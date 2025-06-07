#include "rds_render_api_layer-pch.h"
#include "rds_render_api_layer_common.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

void 
checkRenderThreadExclusive(RDS_DEBUG_SRCLOC_PARAM)
{
	throwIf(!RenderApiLayerTraits::isRenderThread() && Renderer::renderDevice()->adapterInfo().isMultiThread, "only could execute in render thread, {}", RDS_DEBUG_SRCLOC_ARG);
}

#endif
}
