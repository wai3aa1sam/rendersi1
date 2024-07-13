#include "rds_render-pch.h"
#include "rdsRenderPassPipeline.h"
#include "rdsRenderPassFeature.h"

#include "rds_render/pass_feature/utility/image/rdsRpfClearImage2D.h"
#include "rds_render/pass_feature/utility/image/rdsRpfClearImage3D.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderPassPipeline-Impl ---
#endif // 0
#if 1

RenderPassPipeline::RenderPassPipeline()
{
	_rpfClearImage2D	= addRenderPassFeature<RpfClearImage2D>();
	_rpfClearImage3D	= addRenderPassFeature<RpfClearImage3D>();
	_rpfScreenQuad		= addRenderPassFeature<RpfScreenQuad>();
}

RenderPassPipeline::~RenderPassPipeline()
{
	for (auto* e : _rpfs)
	{
		e->~RenderPassFeature();
	}
	_rpfs.clear();
	_alloc.clear();
}

void 
RenderPassPipeline::reset(RenderGraph* renderGraph, DrawData_Base* drawData)
{
	_rdGraph	= renderGraph;
	_drawData	= drawData;
}

#endif

}