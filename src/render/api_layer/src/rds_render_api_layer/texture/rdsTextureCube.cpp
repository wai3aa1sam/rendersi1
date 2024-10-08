#include "rds_render_api_layer-pch.h"
#include "rdsTextureCube.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

namespace rds
{

SPtr<TextureCube> 
RenderDevice::createTextureCube(TextureCube_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTextureCube(cDesc);
	return p;
}


#if 0
#pragma mark --- rdsTextureCube-Impl ---
#endif // 0
#if 1

TextureCube::CreateDesc	
TextureCube::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	return CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
}

SPtr<TextureCube> 
TextureCube::make(CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createTextureCube(cDesc);
}

TextureCube::TextureCube()
	: Base(RenderDataType::TextureCube)
{

}

TextureCube::~TextureCube()
{

}

void TextureCube::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
TextureCube::uploadToGpu(CreateDesc& cDesc)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& tsfReq	= transferContext().transferRequest();
	auto* cmd		= tsfReq.uploadTexture(this);

	checkValid(cDesc);
	onUploadToGpu(cDesc, cmd);
}

void 
TextureCube::onCreate(CreateDesc& cDesc)
{
	RDS_CORE_ASSERT(cDesc.size.x == cDesc.size.y, "cube must be same size");
	Base::onCreate(cDesc);

	if (!cDesc.uploadImages.is_empty())
	{
		uploadToGpu(cDesc);
	}
}

void 
TextureCube::onPostCreate(CreateDesc& cDesc)
{

}

void 
TextureCube::onDestroy()
{


	Base::onDestroy();
}

void 
TextureCube::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd, "");
	
}


#endif

}