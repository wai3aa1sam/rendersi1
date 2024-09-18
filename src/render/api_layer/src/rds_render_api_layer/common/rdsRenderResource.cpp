#include "rds_render_api_layer-pch.h"
#include "rdsRenderResource.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResource-Impl ---
#endif // 0
#if 1

RenderResource::RenderResource()
{

}

RenderResource::~RenderResource()
{
	RDS_CORE_ASSERT(!hasCreated(), " has not call destroy()");
}

void 
RenderResource::create(RenderDevice* rdDev)
{
	CreateDesc cDesc;
	cDesc._internal_create(rdDev);
	create(cDesc);
}

void 
RenderResource::create(RenderDevice* rdDev, bool isBypassChecking)
{
	CreateDesc cDesc;
	cDesc._internal_create(rdDev, isBypassChecking);
	create(cDesc);
}

void 
RenderResource::create(RenderDevice* rdDev, bool isBypassChecking, const SrcLoc& debugSrcLoc_)
{
	CreateDesc cDesc;
	cDesc._internal_create(rdDev, isBypassChecking, debugSrcLoc_);
	create(cDesc);
}

void 
RenderResource::destroy()
{
	_rdDev = nullptr;
}

void 
RenderResource::setDebugName(StrView name)
{
	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	_debugName = name;
	#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME
}

void 
RenderResource::_internal_setSubResourceCount(SizeType n)
{
	_rdState.setSubResourceCount(n);
}

void 
RenderResource::_internal_setRenderResourceState(RenderResourceStateFlags state, u32 subResource)
{
	_rdState.setState(state, subResource);
}

bool 
RenderResource::hasCreated() const
{
	return _rdDev != nullptr;
}

RenderApiType			RenderResource::apiType()		const				{ return renderDevice()->apiType(); }

u64						RenderResource::engineFrameCount()	const			{ return renderDevice()->engineFrameCount(); }
u32						RenderResource::engineFrameIndex()	const			{ return renderDevice()->engineFrameIndex(); }
u64						RenderResource::frameCount()		const			{ return renderDevice()->frameCount(); }
u32						RenderResource::frameIndex()		const			{ return renderDevice()->frameIndex(); }

Renderer*				RenderResource::renderer()							{ return Renderer::instance(); }

RenderFrameParam&		RenderResource::renderFrameParam()					{ return renderDevice()->renderFrameParam(); }

RenderFrame&			RenderResource::renderFrame(u64 frameIdx)			{ return renderDevice()->renderFrame(frameIdx); }

TransferContext&		RenderResource::transferContext()					{ return renderDevice()->transferContext(); }

TransferRequest&		RenderResource::transferRequest()					{ return renderDevice()->transferRequest(engineFrameIndex()); }
TransferRequest&		RenderResource::transferRequest(u64 frameIdx)		{ return renderDevice()->transferRequest(frameIdx); }


#endif

}