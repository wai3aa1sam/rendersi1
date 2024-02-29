#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderContext_Vk;
class Vk_Swapchain;

struct Vk_Texture;

template<class TEX_BASE>
class Texture_Vk : public RenderResource_Vk<TEX_BASE>
{
	friend class RenderContext_Vk;
	friend class Vk_Swapchain;
public:
	using Base = TEX_BASE;
	using CreateDesc = typename Base::CreateDesc;

public:
	Texture_Vk();
	virtual ~Texture_Vk();

	Vk_Image_T*		vkImageHnd();
	Vk_ImageView_T* vkImageViewHnd();
	Vk_Sampler_T*	vkSamplerHnd();

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

	virtual void setDebugName(StrView name) override;
	virtual void setNull() override;

protected:
	Vk_Image		_vkImage;
	Vk_ImageView	_vkImageView;
	Vk_Sampler		_vkSampler;		// TODO: sampler in RenderDevice_Vk, shared globally;
};

#if 0
#pragma mark --- rdsTexture2D_Vk-Decl ---
#endif // 0
#if 1

class Texture2D_Vk : public RenderResource_Vk<Texture2D>
{
	friend class RenderContext_Vk;
	friend class Vk_Swapchain;
	friend struct Vk_Texture;
public:
	Texture2D_Vk();
	virtual ~Texture2D_Vk();

	virtual void setDebugName(StrView name) override;

public:
	virtual bool isNull() const;

public:
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
	Vk_ImageView	_vkImageView;	// TODO: if only stencil, then must have a separate view
	Vk_Sampler		_vkSampler;		// TODO: sampler in RenderDevice_Vk, shared globally;
};

inline Vk_Image*		Texture2D_Vk::vkImage()			{ return &_vkImage; }
inline Vk_ImageView*	Texture2D_Vk::vkImageView()		{ return &_vkImageView; }	
inline Vk_Sampler*		Texture2D_Vk::vkSampler()		{ return &_vkSampler; }	

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
			case RenderDataType::Texture2D:		{ return fn(sCast<Texture2D_Vk*>(tex)); }	break;
			case RenderDataType::TextureCube:	{ return fn(sCast<TextureCube_Vk*>(tex)); } break;
		}
		throwIf(true, "");
		return fn(tex);
	}

	#define RDS_VK_TEXTURE_EXECUTE(TEX, FN) \
	switch (TEX->type()) \
	{ \
			case RenderDataType::Texture2D:		{ return sCast<Texture2D_Vk*>	(TEX)->FN; } break; \
			case RenderDataType::TextureCube:	{ return sCast<TextureCube_Vk*>	(TEX)->FN; } break; \
	} \
	throwIf(true, ""); \
	// ---

	static Vk_Image*		getVkImage		(Texture* tex);
	static Vk_ImageView*	getVkImageView	(Texture* tex);
	static Vk_Sampler*		getVkSampler	(Texture* tex);

	static Vk_Image_T*		getVkImageHnd		(Texture* tex);
	static Vk_ImageView_T*	getVkImageViewHnd	(Texture* tex);
	static Vk_Sampler_T*	getVkSamplerHnd		(Texture* tex);

	template<class TEX_VK>
	static void createVkResource	(TEX_VK* tex)
	{ 
		auto* rdDevVk = tex->renderDeviceVk();
		createVkImage		(getVkImage		(tex), tex, rdDevVk);
		createVkImageView	(getVkImageView	(tex), tex, rdDevVk);
		createVkSampler		(getVkSampler	(tex), tex, rdDevVk);

		//tex->_vkImgViewShaderRsc.create(getVkImage(tex)->hnd(), tex->desc(), TextureUsageFlags::ShaderResource, rdDevVk);

		#if RDS_ENABLE_RenderResouce_DEBUG_NAME
		tex->setDebugName(tex->_debugName);
		#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME
	}

	static void createVkImage		(Vk_Image*		o, Texture* tex, RenderDevice_Vk* rdDevVk);
	static void createVkImageView	(Vk_ImageView*	o, Texture* tex, RenderDevice_Vk* rdDevVk);
	static void createVkSampler		(Vk_Sampler*	o, Texture* tex, RenderDevice_Vk* rdDevVk);
};

}
#endif