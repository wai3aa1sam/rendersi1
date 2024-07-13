#include "rds_render_api_layer-pch.h"
#include "rdsTexture_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rdsTexture3D_Vk.h"
#include "rdsTextureCube_Vk.h"
#include "rdsTexture2DArray_Vk.h"

#include "../transfer/rdsTransferContext_Vk.h"

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


#endif

#if 0
#pragma mark --- rdsTexture2D_Vk-Impl ---
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

	if (cDesc.uploadImage.isValid())
	{
		uploadToGpu(cDesc);
	}
	else if (isValid(cDesc) && !BitUtil::has(cDesc.usageFlags, TextureUsageFlags::BackBuffer))
	{
		Vk_Texture::createVkResource(this);
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
}

void 
Texture2D_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);

	const auto& srcImage = cDesc.uploadImage;
	if (srcImage.isValid())
	{
		transferContextVk().uploadToStagingBuf(cmd->_stagingHnd, srcImage.data());
	}

	Vk_Texture::createVkResource(this);
}

void 
Texture2D_Vk::setDebugName(StrView name)
{
	Base::setDebugName(name);
}

// only use  for swapchain
void 
Texture2D_Vk::setNull()
{
	Base::setNull();
}

#endif

#if 0
#pragma mark --- rdsVk_Image-Impl ---
#endif // 0
#if 1

void 
Vk_Image::create(RenderDevice_Vk* rdDevVk, const Texture_Desc& texDesc, QueueTypeFlags queueTypeFlags
	, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo
	, VkMemoryPropertyFlags vkMemPropFlags)
{
	u32 width	= texDesc.size.x;
	u32 height	= texDesc.size.y;
	u32 depth	= texDesc.size.z;

	RDS_CORE_ASSERT(width && height && depth);

	VkFormat vkFormat = Util::toVkFormat(texDesc.format);
	if (texDesc.isSrgb)
		vkFormat = Util::toVkFormat_srgb(vkFormat);

	VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::ShaderResource))	{ usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT; }
	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::UnorderedAccess))	{ usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT; }
	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::RenderTarget))		{ usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; }
	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::DepthStencil))		{ usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; }
	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::TransferSrc))		{ usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; }
	if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::TransferDst))		{ usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; }

	auto& vkQueueIndices = rdDevVk->queueFamilyIndices();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType		= Util::toVkImageType(texDesc.type);
	imageInfo.extent.width	= width;
	imageInfo.extent.height	= height;
	imageInfo.extent.depth	= depth;
	imageInfo.mipLevels		= texDesc.mipCount;
	imageInfo.arrayLayers	= texDesc.layerCount;
	imageInfo.format		= vkFormat;
	imageInfo.tiling		= VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// must be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED 
	imageInfo.usage			= usageFlags;
	imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples		= Util::toVkSampleCountFlagBits(texDesc.sampleCount);
	imageInfo.flags			= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; // Optional

	if (texDesc.type == RenderDataType::TextureCube)
		imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
	if (queueCount > 1)
	{
		imageInfo.sharingMode			= VK_SHARING_MODE_CONCURRENT;
		imageInfo.queueFamilyIndexCount	= queueCount;
		imageInfo.pQueueFamilyIndices	= queueIdices.data();
	}

	create(vkAlloc, &imageInfo, allocInfo, vkMemPropFlags);

	#if 0
	// this wait immediately
	//RDS_TODO("put to transferContext to process instead of wait immediately, also the whole upload texture flow should be revise");
	//auto& tsfCtxVk = rdDevVk->transferContextVk(); RDS_UNUSED(tsfCtxVk);
	//if		(BitUtil::has(texDesc.usageFlags, TextureUsageFlags::TransferSrc)) { tsfCtxVk.transitImageLayout(hnd(), vkFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, queueTypeFlags); }
	//if		(BitUtil::has(texDesc.usageFlags, TextureUsageFlags::TransferDst))		{ tsfCtxVk.transitImageLayout(hnd(), texDesc, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, queueTypeFlags); }
	//else if (BitUtil::has(texDesc.usageFlags, TextureUsageFlags::UnorderedAccess))	{ tsfCtxVk.transitImageLayout(hnd(), texDesc, VK_IMAGE_LAYOUT_GENERAL,				queueTypeFlags); }
	#endif // 0
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
	cInfo.mipmapMode				= samplerState.magFliter == SamplerFilter::Linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
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
Vk_ImageView::create(Vk_Image_T* vkImageHnd, const Texture_Desc& desc, u32 baseMipLevel, u32 mipCount, u32 baseLayerLevel, u32 layerCount, RenderDevice_Vk* rdDevVk)
{
	throwIf(!vkImageHnd, "no VkImage while creating image view");
	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	if		(BitUtil::has(desc.usageFlags, TextureUsageFlags::DepthStencil)) { aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT; }	// view cannot have both depth and stencil

	// when the format is SRGB, vulkan will convert it to linear when sampling
	VkImageViewCreateInfo cInfo = {};
	cInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	cInfo.image								= vkImageHnd;
	cInfo.viewType							= Util::toVkImageViewType(desc.type);
	cInfo.format							= Util::toVkFormat(desc.format);

	if (desc.isSrgb)
		cInfo.format = Util::toVkFormat_srgb(cInfo.format);		// if is srgb, vulkan will decode ( ^2.2 ) when sample, so change back to linear

	cInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
	cInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;

	cInfo.subresourceRange.aspectMask		= aspectFlags;
	cInfo.subresourceRange.baseMipLevel		= baseMipLevel;
	cInfo.subresourceRange.levelCount		= mipCount;
	cInfo.subresourceRange.baseArrayLayer	= baseLayerLevel;
	cInfo.subresourceRange.layerCount		= layerCount;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateImageView(vkDev, &cInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

#endif // 1


#if 0
#pragma mark --- rdsVk_Texture-Impl ---
#endif // 0
#if 1

void 
Vk_Texture::createVkImage(Vk_Image*		o, Texture* tex, RenderDevice_Vk* rdDevVk)
{
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::GpuOnly;
	o->create(rdDevVk, tex->desc(), QueueTypeFlags::Graphics
		, vkAlloc, &allocInfo);
}

void 
Vk_Texture::createVkImageView(Vk_ImageView*	o, Texture* tex, u32 baseMipLevel, u32 mipCount, u32 baseLayerLevel, u32 layerCount, RenderDevice_Vk* rdDevVk)
{
	o->create(getVkImageHnd(tex), tex->desc(), baseMipLevel, mipCount, baseLayerLevel, layerCount, rdDevVk);
}

void 
Vk_Texture::createVkImageView(Vk_ImageView*	o, Texture* tex, u32 baseMipLevel, u32 mipCount, RenderDevice_Vk* rdDevVk)
{
	createVkImageView(o ,tex, baseMipLevel, mipCount, 0, tex->layerCount(), rdDevVk);
}

//void 
//Vk_Texture::createVkSampler		(Vk_Sampler*	o, Texture* tex, RenderDevice_Vk* rdDevVk)
//{
//	o->create(tex->samplerState(), rdDevVk);
//}

Vk_Image*		Vk_Texture::getVkImage			(Texture* tex)					{ RDS_VK_TEXTURE_EXECUTE(tex, vkImage());					return nullptr; }
Vk_ImageView*	Vk_Texture::getSrvVkImageView	(Texture* tex)					{ RDS_VK_TEXTURE_EXECUTE(tex, srvVkImageView());			return nullptr; }
Vk_ImageView*	Vk_Texture::getUavVkImageView	(Texture* tex, u32 mipLevel)	{ RDS_VK_TEXTURE_EXECUTE(tex, uavVkImageView(mipLevel));	return nullptr; }
//Vk_Sampler*		Vk_Texture::getVkSampler	(Texture* tex) { RDS_VK_TEXTURE_EXECUTE(tex, vkSampler());		return nullptr; }

Vk_Image_T*		Vk_Texture::getVkImageHnd		(Texture* tex)					{ return getVkImage(		tex)->hnd(); }
Vk_ImageView_T*	Vk_Texture::getSrvVkImageViewHnd(Texture* tex)					{ return getSrvVkImageView(	tex)->hnd(); }
Vk_ImageView_T*	Vk_Texture::getUavVkImageViewHnd(Texture* tex, u32 mipLevel)	{ return getUavVkImageView(	tex, mipLevel)->hnd(); }
//Vk_Sampler_T*	Vk_Texture::getVkSamplerHnd		(Texture* tex) { return getVkSampler	(tex)->hnd(); }

#endif // 1


}
#endif