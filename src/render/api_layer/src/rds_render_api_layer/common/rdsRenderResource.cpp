#include "rds_render_api_layer-pch.h"
#include "rdsRenderResource.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResource-Impl ---
#endif // 0
#if 1

Renderer*		RenderResource::renderer()				{ return Renderer::instance(); }
Renderer*		RenderResource::device()				{ return Renderer::instance(); }
RenderFrame&	RenderResource::renderFrame()			{ return device()->renderFrame(); }

TransferContext&	RenderResource::transferContext()	{ return device()->transferContext(); }

TransferRequest&	RenderResource::transferRequest()	{ return device()->transferRequest(); }

#endif

}