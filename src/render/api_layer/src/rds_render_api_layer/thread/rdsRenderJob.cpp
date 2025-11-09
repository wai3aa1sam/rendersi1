#include "rds_render_api_layer-pch.h"
#include "rdsRenderJob.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderContext.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderJob-Impl ---
#endif // 0
#if 1

RenderJob::RenderJob()
{

}

RenderJob::~RenderJob()
{
	reset(nullptr, nullptr, 0);
}

void 
RenderJob::create(CreateDesc& cDesc)
{
	onCreate(cDesc);
}

void RenderJob::destroy()
{
	onDestroy();
}

void 
RenderJob::reset(RenderDevice* renderDevice_, RenderContext* rdCtx, u64 frameCount_)
{
	renderDevice	= renderDevice_;
	frameCount		= frameCount_;

	if (_transferFrame)
		_transferFrame->reset();
	_transferFrame	= nullptr;

	renderGraph().reset(rdCtx);
	renderRequest().reset(rdCtx);
}

bool 
RenderJob::isDoneUploading()
{
	return onCheckUploadCompleted();
}

bool 
RenderJob::isDoneRendering()
{
	return onCheckRenderCompleted();
}

void 
RenderJob::onCreate(CreateDesc& cDesc)
{

}

void 
RenderJob::onDestroy()
{

}

void 
RenderJob::onReset(RenderContext* rdCtx)
{

}

#endif

}