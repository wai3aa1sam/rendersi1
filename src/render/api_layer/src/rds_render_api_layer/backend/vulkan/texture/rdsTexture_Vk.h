#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rdsVk_Texture.h"

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

	virtual void setNull() override;

public:
	void createRenderResource( const RenderFrameParam& rdFrameParam);
	void destroyRenderResource(const RenderFrameParam& rdFrameParam);

	virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

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

protected:
	virtual void onCreate		(CreateDesc& cDesc) override;
	virtual void onPostCreate	(CreateDesc& cDesc) override;
	virtual void onDestroy		() override;

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd) override;

public:
	void createRenderResource( const RenderFrameParam& rdFrameParam);
	void destroyRenderResource(const RenderFrameParam& rdFrameParam);

protected:

};

#endif

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

template<class TEX_BASE> inline void 
Texture_Vk<TEX_BASE>::createRenderResource( const RenderFrameParam& rdFrameParam)
{
	Vk_Texture::createVkResource(this);
}

template<class TEX_BASE> inline void 
Texture_Vk<TEX_BASE>::destroyRenderResource(const RenderFrameParam& rdFrameParam)
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
	

	Base::onDestroy();
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);
}

template<class TEX_BASE> inline 
void 
Texture_Vk<TEX_BASE>::onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd)
{
	auto& name = cmd->name;
	Base::onRenderResouce_SetDebugName(cmd);

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