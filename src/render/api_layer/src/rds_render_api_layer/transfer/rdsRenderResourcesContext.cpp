#include "rds_render_api_layer-pch.h"
#include "rdsRenderResourcesContext.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResourcesContext-Impl ---
#endif // 0
#if 1

RenderResourcesContext::CreateDesc				
RenderResourcesContext::makeCDesc(RenderDevice* rdDev)
{
	auto o = CreateDesc{};
	o.renderDevice = rdDev;
	return o;
}

RenderResourcesContext::RenderResourcesContext()
{

}

RenderResourcesContext::~RenderResourcesContext()
{

}

void 
RenderResourcesContext::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);
}

void 
RenderResourcesContext::destroy()
{
	onDestroy();
}

void 
RenderResourcesContext::commit(const RenderFrameParam& rdFrameParam)
{
	onCommit(rdFrameParam);
}

void 
RenderResourcesContext::createRenderResource( RenderResource* rdResource)
{
	auto data = _createQueue.scopedULock();
	data->emplace_back(rdResource);
}

void 
RenderResourcesContext::destroyRenderResoruce(RenderResource* rdResource)
{
	auto data = _destroyQueue.scopedULock();
	data->emplace_back(rdResource);
}

void 
RenderResourcesContext::onCreate(const CreateDesc& cDesc)
{
	_rdDev = cDesc.renderDevice;
}

void 
RenderResourcesContext::onDestroy()
{

}

void 
RenderResourcesContext::onCommit(const RenderFrameParam& rdFrameParam)
{
	
}

#endif

}