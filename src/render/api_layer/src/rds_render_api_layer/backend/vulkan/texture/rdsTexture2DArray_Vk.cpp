#include "rds_render_api_layer-pch.h"
#include "rdsTexture2DArray_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<Texture2DArray> 
RenderDevice_Vk::onCreateTexture2DArray(Texture2DArray_CreateDesc& cDesc)
{
	auto p = SPtr<Texture2DArray>(makeSPtr<Texture2DArray_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture2DArray_Vk-Impl ---
#endif // 0
#if 1


Texture2DArray_Vk::Texture2DArray_Vk()
{
}

Texture2DArray_Vk::~Texture2DArray_Vk()
{
	destroy();
}

void 
Texture2DArray_Vk::createRenderResource( const RenderFrameParam& rdFrameParam)
{
	Base::createRenderResource(rdFrameParam);

	if (isValid())
	{
		auto layerCount = this->layerCount();
		_srvLayerVkImageViews.resize(layerCount);
		for (u32 i = 0; i < layerCount; i++)
		{
			Vk_Texture::createVkImageView(&_srvLayerVkImageViews[i], this, 0, mipCount(), i, 1, renderDeviceVk());
		}
	}
}

void 
Texture2DArray_Vk::destroyRenderResource(const RenderFrameParam& rdFrameParam)
{
	Base::destroyRenderResource(rdFrameParam);

	auto* rdDevVk = renderDeviceVk();
	for (auto& e : _srvLayerVkImageViews)
	{
		e.destroy(rdDevVk);
	}
}

void 
Texture2DArray_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	
}

void 
Texture2DArray_Vk::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void 
Texture2DArray_Vk::onDestroy()
{
	Base::onDestroy();
}

void 
Texture2DArray_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
Texture2DArray_Vk::setDebugName(StrView name)
{
	Base::setDebugName(name);
	
	u32 i = 0;
	for (auto& e : _srvLayerVkImageViews)
	{
		if (!e.hnd())
			continue;
		RDS_VK_SET_DEBUG_NAME_FMT(_srvLayerVkImageViews[i],	"{}-{}-layer{}-[{}:{}]",	name, "_srvLayerVkImageViews", i,	RDS_DEBUG_SRCLOC.func, RDS_DEBUG_SRCLOC.line);
		++i;
	}
}

// only use  for swapchain
void 
Texture2DArray_Vk::setNull()
{
	Base::setNull();
}

Vk_ImageView_T*	Texture2DArray_Vk::srvLayerVkImageViewHnd(u32 layerIndex) { return _srvLayerVkImageViews[layerIndex].hnd(); }

#endif


}
#endif