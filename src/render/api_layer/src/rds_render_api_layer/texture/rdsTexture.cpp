#include "rds_render_api_layer-pch.h"
#include "rdsTexture.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

namespace rds
{

SPtr<Texture2D> 
RenderDevice::createTexture2D(Texture2D_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture2D(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture::Texture(RenderDataType type)
{
	_desc.type = type;
}

Texture::~Texture()
{
	
}

//void 
//Texture::create	(CreateDesc& cDesc)
//{
//	Base::create(cDesc);
//	_desc = cDesc;
//}

void 
Texture::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
Texture::onCreate(TextureCreateDesc& cDesc)
{
	_desc = cDesc;
	
	setSubResourceCount(desc().mipCount);
	if (BitUtil::has(desc().usageFlags, TextureUsageFlags::TransferDst))
	{
		_internal_setRenderResourceState(RenderResourceStateFlags::Transfer_Dst);
	}

	if (BitUtil::has(desc().usageFlags, TextureUsageFlags::ShaderResource))
	{
		_bindlessHnd = renderDevice()->bindlessResource().allocTexture(this);
	}
}

void 
Texture::onDestroy()
{
	if (/*!isNull() || */bindlessHandle().isValid())
	{
		renderDevice()->bindlessResource().freeTexture(this);
	}
}

bool 
Texture::isValid(TextureCreateDesc& cDesc)
{
	bool isValid = cDesc.size.x > 0 && cDesc.size.y > 0 && cDesc.size.z > 0;
	return isValid;
}

void
Texture::checkValid(TextureCreateDesc& cDesc)
{
	RDS_CORE_ASSERT(isValid(cDesc), "invalid cDesc");
}

#endif

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture2D::CreateDesc	
Texture2D::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	auto o = CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
	o.type = RenderDataType::Texture2D;
	return o;
}

SPtr<Texture2D>
Texture2D::make(CreateDesc& cDesc)
{
	return Renderer::rdDev()->createTexture2D(cDesc);
}

Texture2D::Texture2D()
	: Base(RenderDataType::Texture2D)
{
}

Texture2D::~Texture2D()
{
	RDS_PROFILE_SCOPED();
}


void 
Texture2D::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void 
Texture2D::uploadToGpu(CreateDesc& cDesc)
{
	using UploadTextureJob = TransferRequest_UploadTextureJob;

	throwIf(!OsTraits::isMainThread(), "uploadToGpu must in main thread, otherwise use uploadToGpuAsync instead");

	auto* rdDev		= renderDevice();
	auto& tsfCtx	= rdDev->transferContext();
	auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

	checkValid(cDesc);

	#if 0
	TransferRequest_UploadTextureJob* job = nullptr;
	{
		RDS_TODO("revisit this part, is upload on every thread needed?");
		auto lockedData = tsfReq._uploadTextureJobs.scopedULock();

		RDS_TODO("allocator for UploadTextureJob");
		job = lockedData->emplace_back(makeUPtr<UploadTextureJob>(this, rds::move(cDesc), uploadTexCmds().uploadTexture()));
}

	auto hnd = job->dispatch(_uploadTextureJobParentHnd)->setName("up"); RDS_UNUSED(hnd);
	#endif // 0
	
	destroy();

	auto* cmd = tsfReq.uploadTexCmds().uploadTexture();
	cmd->dst	= this;
	onUploadToGpu(cDesc, cmd);
}

void 
Texture2D::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Texture2D::onPostCreate(CreateDesc& cDesc)
{

}

void 
Texture2D::onDestroy()
{


	Base::onDestroy();
}

void 
Texture2D::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd, "");
	Base::create(cDesc);
}

#endif
}