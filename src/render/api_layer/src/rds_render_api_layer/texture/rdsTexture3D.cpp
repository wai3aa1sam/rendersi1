#include "rds_render_api_layer-pch.h"
#include "rdsTexture3D.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

namespace rds
{

RDS_Define_TypeInfo(Texture3D);

SPtr<Texture3D> 
RenderDevice::createTexture3D(Texture3D_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture3D(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture3D-Impl ---
#endif // 0
#if 1

Texture3D::CreateDesc	
Texture3D::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	auto o = CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
	o.type = RenderDataType::Texture3D;
	return o;
}

SPtr<Texture3D>
Texture3D::make(CreateDesc& cDesc)
{
	return Renderer::rdDev()->createTexture3D(cDesc);
}

Texture3D::Texture3D()
	: Base(RenderDataType::Texture3D)
{
}

Texture3D::~Texture3D()
{
	
}


void 
Texture3D::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void 
Texture3D::uploadToGpu(CreateDesc& cDesc)
{
	_notYetSupported(RDS_SRCLOC);
	using UploadTextureJob = TransferRequest_UploadTextureJob;

	throwIf(!OsTraits::isMainThread(), "uploadToGpu must in main thread, otherwise use uploadToGpuAsync instead");

	auto* rdDev		= renderDevice();
	auto& tsfCtx	= rdDev->transferContext();
	auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

	checkValid(cDesc);

	destroy();

	auto* cmd = tsfReq.uploadTexCmds().uploadTexture();
	cmd->dst	= this;
	onUploadToGpu(cDesc, cmd);
}

void 
Texture3D::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Texture3D::onPostCreate(CreateDesc& cDesc)
{

}

void 
Texture3D::onDestroy()
{


	Base::onDestroy();
}

void 
Texture3D::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	_notYetSupported(RDS_SRCLOC);

	RDS_CORE_ASSERT(cmd, "");
	Base::create(cDesc);
}

#endif
}