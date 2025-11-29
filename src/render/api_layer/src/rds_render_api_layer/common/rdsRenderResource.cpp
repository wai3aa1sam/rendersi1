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
RenderResource::create(RenderDevice* rdDev, bool isBypassChecking, const SrcLoc& debugSrcLoc_)
{
	CreateDesc cDesc;
	cDesc._internal_create(rdDev, isBypassChecking, debugSrcLoc_);
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
RenderResource::setDebugName(StrView name)
{
	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	if (transferContextPtr())
	{
		transferContext().transferFrame().setRenderResourceDebugName(this, name);
	}
	#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME
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
RenderResource::onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd)
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

const char* 
RenderResource::debugName() const
{
	return RDS_DebugLabel_GET_NAME(RDS_DebugLabel_VAR_NAME);
}

bool				
RenderResource::Debug_hasName()			const
{
	#if RDS_ENABLE_DebugLabel
	return StrUtil::len(debugName()) > 0;
	#else
	return false;
	#endif // 0
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