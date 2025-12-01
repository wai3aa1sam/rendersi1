#include "rds_render_api_layer-pch.h"
#include "rdsRenderResource.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

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
	//RDS_CORE_ASSERT(!hasCreated(), " has not call destroy()");
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
RenderResource::destroy()
{
	checkMainThreadExclusive(RDS_SRCLOC);
	onDestroy();
	//_rdDev = nullptr;
}

void 
RenderResource::setDebugLabel(RDS_DebugLabel_PARAM)
{
	#if RDS_ENABLE_DebugLabel
	RDS_DebugLabel_ASSIGN();
	if (transferContextPtr())
	{
		transferContext().transferFrame().setRenderResourceDebugLabel(this, RDS_DebugLabel_ARG);
	}
	#endif // RDS_ENABLE_DebugLabel
}

void 
RenderResource::setSubResourceCount(SizeType n)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	_rdState.setSubResourceCount(n);
}

void 
RenderResource::setRenderResourceState(RenderResourceStateFlags state, u32 subResource)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	_rdState.setState(state, subResource);
}

void 
RenderResource::onRenderResouce_SetDebugLabel(TransferCommand_SetDebugLabel* cmd)
{

}

void 
RenderResource::onDestroy()
{
	
}

void 
RenderResource::RenderResource_CreateEnd()
{
	//renderDevice()->_internal_createRenderResource(this);
}

bool 
RenderResource::hasCreated() const
{
	return _rdDev != nullptr;
}

RenderApiType			RenderResource::apiType()				const		{ return renderDevice()->apiType(); }
RenderResourceType		RenderResource::renderResourceType()	const		{ return _rdRscType; }

Renderer*				RenderResource::renderer()							{ return Renderer::instance(); }

TransferContext&		RenderResource::transferContext()					{ return renderDevice()->transferContext(); }
TransferContext*		RenderResource::transferContextPtr()				{ return renderDevice()->transferContextPtr(); }
TransferRequest&		RenderResource::transferRequest()					{ return renderDevice()->transferRequest(); }

RenderResourceStateFlags 
RenderResource::renderResourceStateFlags(u32 subResource) const 
{ 
	checkMainThreadExclusive(RDS_SRCLOC); 
	return _rdState.state(subResource); 
}


#endif

}