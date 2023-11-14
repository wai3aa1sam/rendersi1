#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{
#if 0
#pragma mark --- rdsTexture2D_Vk-Decl ---
#endif // 0
#if 1

class Texture2D_Vk : public RenderResource_Vk<Texture2D>
{
public:
	Texture2D_Vk();
	virtual ~Texture2D_Vk();

	Vk_Image_T*		vkImageHnd();
	Vk_ImageView_T* vkImageViewHnd();
	Vk_Sampler_T*	vkSamplerHnd();

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

protected:
	Vk_Image		_vkImage;
	Vk_ImageView	_vkImageView;
	Vk_Sampler		_vkSampler;		// TODO: sampler in Renderer_Vk;
};

inline Vk_Image_T*		Texture2D_Vk::vkImageHnd()		{ return _vkImage.hnd(); }
inline Vk_ImageView_T*	Texture2D_Vk::vkImageViewHnd()	{ return _vkImageView.hnd(); }
inline Vk_Sampler_T*	Texture2D_Vk::vkSamplerHnd()	{ return _vkSampler.hnd(); }

#endif



struct Vk_Texture
{
	template<class FN>
	static auto textureExecute(Texture* tex, FN fn)
	{
		throwIf(true, "useless fn");

		switch (tex->type())
		{
			case RenderDataType::Texture2D: { return fn(sCast<Texture2D_Vk*>(tex)); } break;
		}
		throwIf(true, "");
		return fn(tex);
	}

	#define RDS_VK_TEXTURE_EXECUTE(TEX, FN) \
	switch (TEX->type()) \
	{ \
			case RenderDataType::Texture2D: { return sCast<Texture2D_Vk*>(TEX)->FN; } break; \
	} \
	throwIf(true, ""); \
	// ---

	static Vk_Image_T*		getImageHnd		(Texture* tex) { RDS_VK_TEXTURE_EXECUTE(tex, vkImageHnd());		return nullptr; }
	static Vk_ImageView_T*	getImageViewHnd	(Texture* tex) { RDS_VK_TEXTURE_EXECUTE(tex, vkImageViewHnd()); return nullptr; }
	static Vk_Sampler_T*	getSamplerHnd	(Texture* tex) { RDS_VK_TEXTURE_EXECUTE(tex, vkSamplerHnd());	return nullptr; }

};

}
#endif