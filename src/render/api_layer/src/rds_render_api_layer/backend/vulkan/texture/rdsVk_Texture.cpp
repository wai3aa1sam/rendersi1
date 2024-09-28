#include "rds_render_api_layer-pch.h"
#include "rdsVk_Texture.h"

#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture3D_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTextureCube_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture2DArray_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

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

Vk_Image*		Vk_Texture::getVkImage			(Texture* tex)					{ RDS_VK_TEXTURE_INVOKE_R(tex, vkImage());					return nullptr; }
Vk_ImageView*	Vk_Texture::getSrvVkImageView	(Texture* tex)					{ RDS_VK_TEXTURE_INVOKE_R(tex, srvVkImageView());			return nullptr; }
Vk_ImageView*	Vk_Texture::getUavVkImageView	(Texture* tex, u32 mipLevel)	{ RDS_VK_TEXTURE_INVOKE_R(tex, uavVkImageView(mipLevel));	return nullptr; }
//Vk_Sampler*		Vk_Texture::getVkSampler	(Texture* tex) { RDS_VK_TEXTURE_INVOKE_R(tex, vkSampler()); }

Vk_Image_T*		Vk_Texture::getVkImageHnd		(Texture* tex)					{ return getVkImage(		tex)->hnd(); }
Vk_ImageView_T*	Vk_Texture::getSrvVkImageViewHnd(Texture* tex)					{ return getSrvVkImageView(	tex)->hnd(); }
Vk_ImageView_T*	Vk_Texture::getUavVkImageViewHnd(Texture* tex, u32 mipLevel)	{ return getUavVkImageView(	tex, mipLevel)->hnd(); }
//Vk_Sampler_T*	Vk_Texture::getVkSamplerHnd		(Texture* tex) { return getVkSampler	(tex)->hnd(); }

#endif // 1

}

#endif