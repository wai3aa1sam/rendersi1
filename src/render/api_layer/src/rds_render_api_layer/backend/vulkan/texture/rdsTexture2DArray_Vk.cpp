#include "rds_render_api_layer-pch.h"
#include "rdsTexture2DArray_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTexture2DArray_Vk-Impl ---
#endif // 0
#if 1


Texture2DArray_Vk::Texture2DArray_Vk()
{
}

Texture2DArray_Vk::~Texture2DArray_Vk()
{
	
}

void 
Texture2DArray_Vk::onTransferCommand_Create(CmdCreate* cmd)
{
	if (isValid())
	{
		Base::onTransferCommand_Create(cmd);

		auto layerCount = this->layerCount();
		_srvLayerVkImageViews.resize(layerCount);
		for (u32 i = 0; i < layerCount; i++)
		{
			Vk_Texture::createVkImageView(&_srvLayerVkImageViews[i], this, 0, mipCount(), i, 1, renderDeviceVk());
		}
	}
}

void 
Texture2DArray_Vk::onTransferCommand_Destroy()
{
	auto* rdDevVk = renderDeviceVk();
	for (auto& e : _srvLayerVkImageViews)
	{
		e.destroy(rdDevVk);
	}

	Base::onTransferCommand_Destroy();
}

void 
Texture2DArray_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
Texture2DArray_Vk::onRenderResouce_SetDebugName(TransferCommand_SetDebugLabel* cmd)
{
	Base::onRenderResouce_SetDebugName(cmd);

	u32 i = 0;
	for (auto& e : _srvLayerVkImageViews)
	{
		if (!e.hnd())
			continue;
		RDS_VK_SET_DEBUG_LABEL(_srvLayerVkImageViews[i], RDS_DebugLabel("{}_srvLayerVkImageViews[{}]", cmd->DebugLabel_getName(), i), renderDeviceVk());
		++i;
	}
}

Vk_ImageView_T*	Texture2DArray_Vk::srvLayerVkImageViewHnd(u32 layerIndex) { return _srvLayerVkImageViews[layerIndex].hnd(); }

#endif


}
#endif