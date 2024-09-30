#include "rds_render_api_layer-pch.h"
#include "rdsTexture3D_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<Texture3D> 
RenderDevice_Vk::onCreateTexture3D(Texture3D_CreateDesc& cDesc)
{
	auto p = SPtr<Texture3D>(makeSPtr<Texture3D_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture3D_Vk-Impl ---
#endif // 0
#if 1


Texture3D_Vk::Texture3D_Vk()
{
}

Texture3D_Vk::~Texture3D_Vk()
{
	destroy();
}

void 
Texture3D_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	
}

void 
Texture3D_Vk::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void 
Texture3D_Vk::onDestroy()
{
	Base::onDestroy();
}

void 
Texture3D_Vk::createRenderResource( const RenderFrameParam& rdFrameParam)
{
	Base::createRenderResource(rdFrameParam);
}

void 
Texture3D_Vk::destroyRenderResource(const RenderFrameParam& rdFrameParam)
{

	Base::destroyRenderResource(rdFrameParam);
}

void 
Texture3D_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);

	const auto& srcImage = cDesc.uploadImage;
	if (srcImage.isValid())
	{
		transferContextVk().uploadToStagingBuf(cmd->_stagingHnd, srcImage.data());
	}
}

void 
Texture3D_Vk::setDebugName(StrView name)
{
	Base::setDebugName(name);
}

// only use  for swapchain
void 
Texture3D_Vk::setNull()
{
	Base::setNull();
}

#endif


}
#endif