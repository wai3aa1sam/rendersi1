#include "rds_render_api_layer-pch.h"
#include "rdsTexture.h"
#include "rds_render_api_layer/rdsRenderer.h"

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
	: _type(type)
{

}

void 
Texture::create	(CreateDesc& cDesc)
{
	Base::create(cDesc);
	_desc = cDesc;
}

void 
Texture::destroy()
{
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture2D::CreateDesc	
Texture2D::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	return CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
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
	destroy();

	onCreate(cDesc);
	onPostCreate(cDesc);
}

void 
Texture2D::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
Texture2D::_internal_uploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd, "");

	cmd->dst	= this;

	cDesc.loadImage();
	cDesc._isBypassChecking = true;
	_create(cDesc);

	onUploadToGpu(cDesc, cmd);
}

void 
Texture2D::onCreate(CreateDesc& cDesc)
{
	_create(cDesc);
	if (cDesc.hasDataToUpload())
	{
		transferRequest().uploadTexture(this, rds::move(cDesc));
	}
}

void 
Texture2D::onPostCreate(CreateDesc& cDesc)
{

}

void 
Texture2D::onDestroy()
{

}

void 
Texture2D::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cDesc.size.x > 0 && cDesc.size.y > 0, "cDesc.size.x > 0 && cDesc.size.y > 0 but cDesc.size=[{}], cDesc._filename: {}", cDesc.size, cDesc._filename);
	_create(cDesc);
}

void 
Texture2D::_create(CreateDesc& cDesc)
{
	Base::create(cDesc);

	if (cDesc.hasDataToUpload())
	{
		//_desc.size.set(sCast<u32>(cDesc.uploadImage().width()), sCast<u32>(cDesc.uploadImage().height()), sCast<u32>(1));
	}
}

#endif
}