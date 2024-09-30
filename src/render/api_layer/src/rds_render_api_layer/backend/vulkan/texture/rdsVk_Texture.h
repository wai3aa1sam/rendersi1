#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_Texture-Decl ---
#endif // 0
#if 1

struct Vk_Texture : public NonCopyable
{
	template<class FN, class... ARGS>
	static 
		decltype(auto)
		executeTexture_Vk(Texture* tex, FN&& fn, ARGS&&... args)
	{
		switch (tex->type())
		{
			case RenderDataType::Texture2D:			{ return rds::invoke(rds::forward<FN>(fn), sCast<Texture2D_Vk*>(tex),		rds::forward<ARGS>(args)...); }	break;
			case RenderDataType::Texture3D:			{ return rds::invoke(rds::forward<FN>(fn), sCast<Texture3D_Vk*>(tex),		rds::forward<ARGS>(args)...); }	break;
			case RenderDataType::TextureCube:		{ return rds::invoke(rds::forward<FN>(fn), sCast<TextureCube_Vk*>(tex),		rds::forward<ARGS>(args)...); }	break;
			case RenderDataType::Texture2DArray:	{ return rds::invoke(rds::forward<FN>(fn), sCast<Texture2DArray_Vk*>(tex),	rds::forward<ARGS>(args)...); }	break;
			default: { throwError("invalid texture type"); } \
		}
		return rds::invoke(rds::forward<FN>(fn), tex, rds::forward<ARGS>(args)...);
	}

	#define RDS_VK_TEXTURE_INVOKE(TEX, FN) \
	{ \
		switch (TEX->type()) \
		{ \
			case RenderDataType::Texture2D:			{ sCast<Texture2D_Vk*>(			TEX)->FN; } break; \
			case RenderDataType::Texture3D:			{ sCast<Texture3D_Vk*>(			TEX)->FN; } break; \
			case RenderDataType::TextureCube:		{ sCast<TextureCube_Vk*>(		TEX)->FN; } break; \
			case RenderDataType::Texture2DArray:	{ sCast<Texture2DArray_Vk*>(	TEX)->FN; } break; \
			default: { throwIf(true, "invalid texture type"); } \
		} \
	} \
	// ---

	#define RDS_VK_TEXTURE_INVOKE_R(TEX, FN) \
	{ \
		switch (TEX->type()) \
		{ \
			case RenderDataType::Texture2D:			{ return sCast<Texture2D_Vk*>(		TEX)->FN; } break; \
			case RenderDataType::Texture3D:			{ return sCast<Texture3D_Vk*>(		TEX)->FN; } break; \
			case RenderDataType::TextureCube:		{ return sCast<TextureCube_Vk*>(	TEX)->FN; } break; \
			case RenderDataType::Texture2DArray:	{ return sCast<Texture2DArray_Vk*>(	TEX)->FN; } break; \
			default: { throwIf(true, "invalid texture type"); } \
		} \
	} \
	// ---

	static Vk_Image*		getVkImage(				Texture* tex);
	static Vk_ImageView*	getSrvVkImageView(		Texture* tex);
	static Vk_ImageView*	getUavVkImageView(		Texture* tex, u32 mipLevel);
	static Vk_Sampler*		getVkSampler(			Texture* tex);

	static Vk_Image_T*		getVkImageHnd(			Texture* tex);
	static Vk_ImageView_T*	getSrvVkImageViewHnd(	Texture* tex);
	static Vk_ImageView_T*	getUavVkImageViewHnd(	Texture* tex, u32 mipLevel);
	static Vk_Sampler_T*	getVkSamplerHnd(		Texture* tex);

	template<class TEX_VK>
	static void createVkResource(TEX_VK* tex)
	{ 
		if (!tex->isValid())
			return;

		auto* rdDevVk = tex->renderDeviceVk();
		createVkImage(		getVkImage(tex), tex, rdDevVk);
		createVkImageView(	getSrvVkImageView(tex), tex, 0, tex->mipCount(), rdDevVk);

		if (tex->hasMipmapView())
		{
			auto nMipmapView = tex->mipmapViewCount();
			tex->_uavVkImageViews.resize(nMipmapView);
			for (u32 i = 0; i < nMipmapView; i++)
			{
				createVkImageView(getUavVkImageView(tex, i), tex, i, 1, rdDevVk);
			}
		}
	}

	static void createVkImage(		Vk_Image*		o, Texture* tex,RenderDevice_Vk* rdDevVk);
	static void createVkImageView(	Vk_ImageView*	o, Texture* tex, u32 baseMipLevel, u32 mipCount, u32 baseLayerLevel, u32 layerCount,	RenderDevice_Vk* rdDevVk);
	static void createVkImageView(	Vk_ImageView*	o, Texture* tex, u32 baseMipLevel, u32 mipCount, RenderDevice_Vk* rdDevVk);
	//static void createVkSampler		(Vk_Sampler*	o, Texture* tex, RenderDevice_Vk* rdDevVk);
};
#endif

}
#endif