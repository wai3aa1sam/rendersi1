#include "rds_render_api_layer-pch.h"
#include "rdsTexture_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<Texture2D> 
RenderDevice_Vk::onCreateTexture2D(Texture2D_CreateDesc& cDesc)
{
	auto p = SPtr<Texture2D>(makeSPtr<Texture2D_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture_Vk-Impl ---
#endif // 0
#if 1

Texture2D_Vk::Texture2D_Vk()
{
}

Texture2D_Vk::~Texture2D_Vk()
{
	destroy();
}

void 
Texture2D_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Texture2D_Vk::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);

}

void 
Texture2D_Vk::onDestroy()
{
	Base::onDestroy();

	auto* rdDevVk = renderDeviceVk();

	_vkSampler.destroy(rdDevVk);
	_vkImageView.destroy(rdDevVk);
	_vkImage.destroy();
}

void 
Texture2D_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_TODO("defer destroy and create new, or multi buffer if if upload frequently, but texture should not upload frequently,");
	destroy();

	Base::onUploadToGpu(cDesc, cmd);

	auto* rdDevVk	= renderDeviceVk();
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	auto&		srcImage	= cDesc.uploadImage();
	auto		bytes		= sCast<VkDeviceSize>(srcImage.totalByteSize());
	const auto& imageSize	= size();
	auto*		stagingBuf	= vkTransferFrame().requestStagingBuffer(cmd->_stagingIdx, bytes, rdDevVk);

	_vkSampler.create(cDesc.samplerState, rdDevVk);

	{
		auto memmap = Vk_ScopedMemMapBuf{ stagingBuf };
		memory_copy(memmap.data<u8*>(), srcImage.dataPtr(), bytes);
	}

	{
		VkFormat vkFormat = Util::toVkFormat_srgb(Util::toVkFormat(cDesc.format));

		Vk_AllocInfo allocInfo = {};
		allocInfo.usage = RenderMemoryUsage::GpuOnly;

		_vkImage.create(rdDevVk, vkAlloc, &allocInfo
			, imageSize.x, imageSize.y
			, vkFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, QueueTypeFlags::Transfer);
	}

	_vkImageView.create(this, rdDevVk);

	_setDebugName();
}

void 
Texture2D_Vk::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME(_vkSampler);
	RDS_VK_SET_DEBUG_NAME(_vkImage);
	RDS_VK_SET_DEBUG_NAME(_vkImageView);
}

#endif

#if 0
#pragma mark --- rdsVk_Sampler-Impl ---
#endif // 0
#if 1

void 
Vk_Sampler::create(const SamplerState& samplerState, RenderDevice_Vk* rdDevVk)
{
	const auto& adapterInfo = rdDevVk->adapterInfo();

	VkSamplerCreateInfo cInfo = {};
	cInfo.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	cInfo.magFilter					= Util::toVkFilter(samplerState.magFliter);
	cInfo.minFilter					= Util::toVkFilter(samplerState.minFliter);
	cInfo.addressModeU				= Util::toVkSamplerAddressMode(samplerState.wrapU);
	cInfo.addressModeV				= Util::toVkSamplerAddressMode(samplerState.wrapV);
	cInfo.addressModeW				= Util::toVkSamplerAddressMode(samplerState.wrapS);
	cInfo.anisotropyEnable			= adapterInfo.feature.hasSamplerAnisotropy;
	cInfo.maxAnisotropy				= adapterInfo.limit.maxSamplerAnisotropy;
	cInfo.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	cInfo.unnormalizedCoordinates	= VK_FALSE;
	cInfo.compareEnable				= VK_FALSE;
	cInfo.compareOp					= VK_COMPARE_OP_ALWAYS;
	cInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	cInfo.mipLodBias				= 0.0f;
	cInfo.minLod					= samplerState.minLod;
	cInfo.maxLod					= samplerState.maxLod;

	auto* vkDev				= rdDevVk->vkDevice();
	auto* vkAllocCallBacks	= rdDevVk->allocCallbacks();

	auto ret = vkCreateSampler(vkDev, &cInfo, vkAllocCallBacks, hndForInit());
	Util::throwIfError(ret);
}

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Impl ---
#endif // 0
#if 1

void 
Vk_ImageView::create(Texture2D_Vk* tex2DVk, RenderDevice_Vk* rdDevVk)
{
	throwIf(!tex2DVk->vkImageHnd(), "no VkImage while creating image view");

	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	if (tex2DVk->usage() == TextureUsage::DepthStencil)
	{
		aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageViewCreateInfo cInfo = {};
	cInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	cInfo.image								= tex2DVk->vkImageHnd();
	cInfo.viewType							= Util::toVkImageViewType(tex2DVk->type());
	cInfo.format							= Util::toVkFormat_ShaderTexture(Util::toVkFormat(tex2DVk->format()));
	
	cInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;

	cInfo.subresourceRange.aspectMask		= aspectFlags;
	cInfo.subresourceRange.baseMipLevel		= 0;
	cInfo.subresourceRange.baseArrayLayer	= 0;
	cInfo.subresourceRange.levelCount		= tex2DVk->mipCount();
	cInfo.subresourceRange.layerCount		= 1;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateImageView(vkDev, &cInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

#endif // 1


}
#endif