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
RenderResource::create(const RenderResource_CreateDesc& cDesc)
{
	RDS_CORE_ASSERT(!hasCreated() || cDesc._isBypassChecking, "!hasCreated() || cDesc._isBypassChecking");
	RDS_CORE_ASSERT(cDesc._rdDev, "cDesc._rdDev");
	RDS_CORE_ASSERT(!hasCreated() || (cDesc._rdDev && _rdDev && cDesc._rdDev == _rdDev), "RenderDevice is not the same as the original one");

	_rdDev = cDesc._rdDev;

	#if RDS_DEVELOPMENT
	RDS_DEBUG_SRCLOC = cDesc._debugSrcLoc;
	#endif // RDS_DEVELOPMENT
}

void 
RenderResource::create(RenderDevice* rdDev)
{
	CreateDesc cDesc;
	cDesc._internal_create(rdDev);
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

bool 
RenderResource::hasCreated() const
{
	return _rdDev != nullptr;
}

Renderer*			RenderResource::renderer()			{ return Renderer::instance(); }
RenderDevice*		RenderResource::renderDevice()		{ return _rdDev; }
RenderFrame&		RenderResource::renderFrame()		{ return renderDevice()->renderFrame(); }

TransferContext&	RenderResource::transferContext()	{ return renderDevice()->transferContext(); }

TransferRequest&	RenderResource::transferRequest()	{ return renderDevice()->transferRequest(); }


#endif

}