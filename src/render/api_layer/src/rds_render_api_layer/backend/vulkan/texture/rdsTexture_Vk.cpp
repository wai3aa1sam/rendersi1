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
	if (!cDesc.hasDataToUpload())
	{
		_createVkResource(cDesc);
	}
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
	RDS_WARN_ONCE("destroy();");

	Base::onUploadToGpu(cDesc, cmd);

	RDS_WARN_ONCE("Base::onUploadToGpu(cDesc, cmd);");

	const auto& srcImage	= cDesc.uploadImage();
	if (!srcImage.data().is_empty())
	{
		//transferContextVk().uploadToStagingBuf(cmd->_stagingIdx, srcImage.data());
		transferContextVk().uploadToStagingBuf(cmd->_stagingHnd, srcImage.data());
		RDS_WARN_ONCE("transferContextVk().uploadToStagingBuf(cmd->_stagingHnd, srcImage.data());");
	}

	_createVkResource(cDesc);
	RDS_WARN_ONCE("_createVkResource(cDesc);");
}

void 
Texture2D_Vk::_createVkResource(const CreateDesc& cDesc)
{
	auto* rdDevVk	= renderDeviceVk();
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	_vkSampler.create(cDesc.samplerState, rdDevVk);

	{
		bool isStoreAsSrgb = false;
		VkFormat vkFormat = Util::toVkFormat(cDesc.format);
		if (isStoreAsSrgb)
			vkFormat = Util::toVkFormat_srgb(vkFormat);

		Vk_AllocInfo allocInfo = {};
		allocInfo.usage = RenderMemoryUsage::GpuOnly;

		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		if (BitUtil::has(flag(), TextureFlags::ShaderResource)) { usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT; }
		if (BitUtil::has(flag(), TextureFlags::UnorderedAccess)){ usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT; }
		if (BitUtil::has(flag(), TextureFlags::RenderTarget))	{ usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; }
		if (BitUtil::has(flag(), TextureFlags::DepthStencil))	{ usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; }
	
		_vkImage.create(rdDevVk, vkAlloc, &allocInfo
			, size().x, size().y
			, vkFormat, VK_IMAGE_TILING_OPTIMAL, usageFlags
			, QueueTypeFlags::Graphics);
	}

	_vkImageView.create(this, rdDevVk);
	RDS_WARN_ONCE("_vkImageView.create(this, rdDevVk);");

	_setDebugName();
	RDS_WARN_ONCE("_setDebugName();");
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
	if (BitUtil::has(tex2DVk->flag(), TextureFlags::DepthStencil)) { aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; }

	// when the format is SRGB, vulkan will convert it to linear when sampling
	VkImageViewCreateInfo cInfo = {};
	cInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	cInfo.image								= tex2DVk->vkImageHnd();
	cInfo.viewType							= Util::toVkImageViewType(tex2DVk->type());
	cInfo.format							= Util::toVkFormat(tex2DVk->format());

	bool isSampleAsLinear = false;
	if (isSampleAsLinear)
		cInfo.format = Util::toVkFormat_srgb(cInfo.format);

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