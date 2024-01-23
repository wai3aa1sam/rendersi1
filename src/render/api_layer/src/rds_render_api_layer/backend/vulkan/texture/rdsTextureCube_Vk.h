#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTextureCube_Vk-Decl ---
#endif // 0
#if 1

class TextureCube_Vk : public RenderResource_Vk<TextureCube>
{
	friend class RenderContext_Vk;
	friend class Vk_Swapchain;
	friend struct Vk_Texture;
public:
	using Base = RenderResource_Vk<TextureCube>;
	using Base::size;

public:
	TextureCube_Vk();
	virtual ~TextureCube_Vk();

	virtual void setDebugName(StrView name) override;

	Vk_Image*		vkImage();
	Vk_ImageView*	vkImageView();
	Vk_Sampler*		vkSampler();

	Vk_Image_T*		vkImageHnd();
	Vk_ImageView_T* vkImageViewHnd();
	Vk_Sampler_T*	vkSamplerHnd();

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

protected:
	void _setDebugName(StrView name);

	virtual void setNull() override;

protected:
	Vk_Image		_vkImage;
	Vk_ImageView	_vkImageView;
	Vk_Sampler		_vkSampler;		// TODO: sampler in RenderDevice_Vk, shared globally;
};

inline Vk_Image*		TextureCube_Vk::vkImage()			{ return &_vkImage; }
inline Vk_ImageView*	TextureCube_Vk::vkImageView()		{ return &_vkImageView; }	
inline Vk_Sampler*		TextureCube_Vk::vkSampler()			{ return &_vkSampler; }	

inline Vk_Image_T*		TextureCube_Vk::vkImageHnd()		{ return _vkImage.hnd(); }
inline Vk_ImageView_T*	TextureCube_Vk::vkImageViewHnd()	{ return _vkImageView.hnd(); }
inline Vk_Sampler_T*	TextureCube_Vk::vkSamplerHnd()		{ return _vkSampler.hnd(); }

#endif

}
#endif