#include "rds_render-pch.h"
#include "rdsRenderPassPipeline.h"
#include "rdsRenderPassFeature.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderPassPipeline-Impl ---
#endif // 0
#if 1

RenderPassPipeline::RenderPassPipeline()
{

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