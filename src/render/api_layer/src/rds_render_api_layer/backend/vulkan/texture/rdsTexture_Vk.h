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
	friend class	RenderContext_Vk;
	friend class	Vk_Swapchain;
	friend struct	Vk_Texture;
public:
	using Base = TEX_BASE;
	using CreateDesc = typename Base::CreateDesc;

public:
	Texture_Vk();
	virtual ~Texture_Vk();

public:
	virtual bool isNull() const;

	Vk_Image*		vkImage();
	Vk_ImageView*	srvVkImageView();
	Vk_ImageView*	uavVkImageView(u32 mipLevel);

	Vk_Image_T*		vkImageHnd();
	Vk_ImageView_T* srvVkImageViewHnd();
	Vk_ImageView_T* uavVkImageViewHnd(u32 mipLevel);

protected:
	virtual void onCreate(		CreateDesc& cDesc) override;
	virtual void onPostCreate(	CreateDesc& cDesc) override;
	virtual void onDestroy() override;

	virtual void onUploadToGpu(	CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

	virtual void onCreateRenderResource();
	virtual void onDestroyRenderResource();

	virtual void setDebugName(StrView name) override;
	virtual void setNull() override;

protected:
	Vk_Image					_vkImage;
	Vk_ImageView				_srvVkImageView;		// for shader resource
	Vector<Vk_ImageView, 10>	_uavVkImageViews;
	// TODO: if only stencil, then must have a separate view
};

#if 0
#pragma mark --- rdsTexture2D_Vk-Decl ---
#endif // 0
#if 1

class Texture2D_Vk : public Texture_Vk<Texture2D>
{
	friend class	RenderContext_Vk;
	friend class	Vk_Swapchain;
	friend struct	Vk_Texture;
public:
	using Base = Texture_Vk<Texture2D>;

public:
	Texture2D_Vk();
	virtual ~Texture2D_Vk();

	void createRenderResource();
	void destroyRenderResource();

public:
	virtual void setDebugName(StrView name) override;
	virtual void setNull() override;

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

protected:

};

#endif


struct Vk_Texture
{
	template<class FN>
	static auto textureExecute(Texture* tex, FN fn)
	{
		throwIf(true, "useless fn");

		switch (tex->type())
		{
			case RenderDataType::Texture2D:			{ return fn(sCast<Texture2D_Vk*>(tex)); }		break;
			case RenderDataType::Texture3D:			{ return fn(sCast<Texture3D_Vk*>(tex)); }		break;
			case RenderDataType::TextureCube:		{ return fn(sCast<TextureCube_Vk*>(tex)); }		break;
			case RenderDataType::Texture2DArray:	{ return fn(sCast<Texture2DArray_Vk*>(tex)); }	break;
		}
		throwIf(true, "");
		return fn(tex);
	}

	#define RDS_VK_TEXTURE_EXECUTE(TEX, FN) \
	switch (TEX->type()) \
	{ \
			case RenderDataType::Texture2D:			{ return sCast<Texture2D_Vk*>(			TEX)->FN; } break; \
			case RenderDataType::Texture3D:			{ return sCast<Texture3D_Vk*>(			TEX)->FN; } break; \
			case RenderDataType::TextureCube:		{ return sCast<TextureCube_Vk*>(		TEX)->FN; } break; \
			case RenderDataType::Texture2DArray:	{ return sCast<Texture2DArray_Vk*>(		TEX)->FN; } break; \
	} \
	throwIf(true, ""); \
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

#if 0
#pragma mark --- rdsTexture_Vk-Impl ---
#endif // 0
#if 1
template<class TEX_BASE> inline
Texture_Vk<TEX_BASE>::Texture_Vk()
{
	//_vkImageViews.resize(1);
}

template<class TEX_BASE> inline
Texture_Vk<TEX_BASE>::~Texture_Vk()
{
	
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onDestroy()
{
	if (!_vkImage)
		return;

	auto* rdDevVk = renderDeviceVk();

	_srvVkImageView.destroy(rdDevVk);
	for (auto& e : _uavVkImageViews)
	{
		e.destroy(rdDevVk);
	}
	_vkImage.destroy();

	Base::onDestroy();
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onCreateRenderResource()
{

}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onDestroyRenderResource()
{

}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::setDebugName(StrView name)
{
	Base::setDebugName(name);

	if (_vkImage.hnd())
	{
		RDS_VK_SET_DEBUG_NAME_FMT(_vkImage,				"{}-{}-[{}:{}]",		name, "_vkImage",				RDS_DEBUG_SRCLOC.func, RDS_DEBUG_SRCLOC.line);
	}

	if (_srvVkImageView.hnd())
	{
		RDS_VK_SET_DEBUG_NAME_FMT(_srvVkImageView,		"{}-{}-[{}:{}]",		name, "_srvVkImageView",		RDS_DEBUG_SRCLOC.func, RDS_DEBUG_SRCLOC.line);
	}

	u32 i = 0;
	for (auto& e : _uavVkImageViews)
	{
		if (!e.hnd())
			continue;
		RDS_VK_SET_DEBUG_NAME_FMT(_uavVkImageViews[i],	"{}-{}-mip{}-[{}:{}]",	name, "_uavVkImageViews", i,	RDS_DEBUG_SRCLOC.func, RDS_DEBUG_SRCLOC.line);
		++i;
	}
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::setNull()
{
	_vkImage.destroy();
	_srvVkImageView.destroy(nullptr);
	for (auto& e : _uavVkImageViews)
	{
		e.destroy(nullptr);
	}
}

template<class TEX_BASE> inline bool			Texture_Vk<TEX_BASE>::isNull()	const					{ return !_vkImage; }

template<class TEX_BASE> inline Vk_Image*		Texture_Vk<TEX_BASE>::vkImage()							{ return &_vkImage; }
template<class TEX_BASE> inline Vk_ImageView*	Texture_Vk<TEX_BASE>::srvVkImageView()					{ return &_srvVkImageView; }
template<class TEX_BASE> inline Vk_ImageView*	Texture_Vk<TEX_BASE>::uavVkImageView(u32 mipLevel)		{ return &_uavVkImageViews[mipLevel]; }

template<class TEX_BASE> inline Vk_Image_T*		Texture_Vk<TEX_BASE>::vkImageHnd()						{ return _vkImage.hnd(); }
template<class TEX_BASE> inline Vk_ImageView_T*	Texture_Vk<TEX_BASE>::srvVkImageViewHnd()				{ return _srvVkImageView.hnd(); }
template<class TEX_BASE> inline Vk_ImageView_T* Texture_Vk<TEX_BASE>::uavVkImageViewHnd(u32 mipLevel)	{ return _uavVkImageViews[mipLevel].hnd(); }

#endif

}
#endif