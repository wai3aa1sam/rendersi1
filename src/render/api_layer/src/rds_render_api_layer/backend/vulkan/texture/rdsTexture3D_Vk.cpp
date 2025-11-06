#include "rds_render_api_layer-pch.h"
#include "rdsTexture3D_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTexture3D_Vk-Impl ---
#endif // 0
#if 1

Texture3D_Vk::Texture3D_Vk()
{
}

Texture3D_Vk::~Texture3D_Vk()
{
	
}

void 
Texture3D_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);

	const auto& srcImage = cDesc.uploadImage;
	if (srcImage.isValid())
	{
		transferFrameVk().uploadToStagingBuf(cmd->_stagingHnd, srcImage.data());
	}
}

#endif


}
#endif