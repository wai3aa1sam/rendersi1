#include "rds_render_api_layer-pch.h"
#include "rdsTexture.h"
#include "rdsTexture3D.h"
#include "rdsTextureCube.h"
#include "rdsTexture2DArray.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

namespace rds
{

RDS_Define_TypeInfo(Texture2D);

SPtr<Texture>
RenderDevice::createTexture(Texture_CreateDesc& cDesc)
{
	SPtr<Texture> o;
	switch (cDesc.type)
	{
		case RenderDataType::Texture2D:			{ auto p = createTexture2D(			sCast<Texture2D_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::Texture3D:			{ auto p = createTexture3D(			sCast<Texture3D_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::TextureCube:		{ auto p = createTextureCube(		sCast<TextureCube_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::Texture2DArray:	{ auto p = createTexture2DArray(	sCast<Texture2DArray_CreateDesc&>(	cDesc)); o.reset(p.ptr()); } break;
		default: { RDS_THROW("invalid texture type"); } break;
	}
	return o;
}

SPtr<Texture2D> 
RenderDevice::createTexture2D(Texture2D_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture2D(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsSamplerState-Impl ---
#endif // 0
#if 1

//SamplerState::SamplerState()
//	: isAnisotropy(true)
//{
//	minFliter	= Filter::Linear;
//	magFliter	= Filter::Linear;
//
//	wrapU		= Wrap::Repeat;
//	wrapV		= Wrap::Repeat;
//	wrapS		= Wrap::Repeat;
//}

SamplerState::HashValue 
SamplerState::hash() const
{
	HashValue o = 0;

	math::hashCombine(o, enumInt(minFliter));
	math::hashCombine(o, enumInt(magFliter));

	math::hashCombine(o, enumInt(wrapU));
	math::hashCombine(o, enumInt(wrapV));
	math::hashCombine(o, enumInt(wrapS));

	/*
	* hashing these 2 value will crash in Release
	*/
	//Hash<decltype(minLod)> hasher;
	//math::hashCombine(o, hasher(minLod));
	//math::hashCombine(o, hasher(maxLod));

	//math::hashCombine(o, isAnisotropy);

	return o;
}

bool 
SamplerState::operator<(const SamplerState & rhs) const
{
	return hash() < rhs.hash();
}

bool 
SamplerState::operator>(const SamplerState& rhs) const
{
	return hash() > rhs.hash();
}

bool 
SamplerState::operator==(const SamplerState& rhs) const
{
	return hash() == rhs.hash();
}

bool 
SamplerState::operator!=(const SamplerState& rhs) const
{
	return !operator==(rhs);
}

#endif

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture::Texture(RenderDataType type)
{
	_desc.type = type;
}

Texture::~Texture()
{
	//RDS_LOG_ERROR("~Texture() {}", debugName());
}

//void 
//Texture::create	(CreateDesc& cDesc)
//{
//	Base::create(cDesc);
//	_desc = cDesc;
//}

void 
Texture::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
Texture::onCreate(TextureCreateDesc& cDesc)
{
	isValid(cDesc);
	_desc = cDesc;
	_internal_setSubResourceCount(desc().mipCount);

	auto*	rdDev		= renderDevice();
	auto&	bindlessRsc	= rdDev->bindlessResource();
	auto	usageFlags	= desc().usageFlags;

	if (BitUtil::hasAny(usageFlags, TextureUsageFlags::ShaderResource))
	{
		_bindlessHnd	= bindlessRsc.allocTexture(this);
		renderDevice()->textureStock().textures.add(this);		// TextureStock only store ShaderResource
	}

	if (BitUtil::hasAny(usageFlags, TextureUsageFlags::UnorderedAccess))
	{
		_uavBindlessHnd	= bindlessRsc.allocImage(this);
	}

	auto& tsfCtx = rdDev->transferContext();
	tsfCtx.createTexture(this);
}

void 
Texture::onDestroy()
{
	auto* rdDev = renderDevice();

	if (bindlessHandle().isValid())
	{
		rdDev->textureStock().textures.remove(this);		// TextureStock only store ShaderResource, must before remove bindless
		rdDev->bindlessResource().freeTexture(this);
	}

	if (uavBindlessHandle().isValid())
	{
		rdDev->bindlessResource().freeImage(this);
	}
}

void 
Texture::_internal_requestDestroyObject()
{
	Base::_internal_requestDestroyObject();

	auto* rdDev	 = renderDevice();
	auto& tsfCtx = rdDev->transferContext();
	tsfCtx.destroyTexture(this);
}

bool 
Texture::isValid(const Texture_Desc& desc) const
{
	bool isValid = desc.size.x > 0 && desc.size.y > 0 && desc.size.z > 0;
	return isValid;
}

bool 
Texture::isValid() const
{
	return isValid(desc());
}

void
Texture::checkValid(const Texture_Desc& desc) const
{
	RDS_CORE_ASSERT(isValid(desc), "invalid desc");
}

bool	Texture::hasMipmapView()	const { return BitUtil::has(usageFlags(), TextureUsageFlags::UnorderedAccess); }
u32		Texture::mipmapViewCount()	const { return hasMipmapView() ? mipCount() : 1; }

bool	Texture::isArray()			const { return type() == RenderDataType::Texture1DArray || type() == RenderDataType::Texture2DArray || type() == RenderDataType::Texture3DArray || type() == RenderDataType::TextureCubeArray; }

bool	Texture::isTexture()		const { return BitUtil::hasAny(usageFlags(), TextureUsageFlags::ShaderResource | TextureUsageFlags::DepthStencil); }
bool	Texture::isImage()			const { return BitUtil::hasAny(usageFlags(), TextureUsageFlags::UnorderedAccess); }

#endif

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture2D::CreateDesc	
Texture2D::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	auto o = CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
	o.type = RenderDataType::Texture2D;
	return o;
}

SPtr<Texture2D>
Texture2D::make(CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createTexture2D(cDesc);
}

Texture2D::Texture2D()
	: Base(RenderDataType::Texture2D)
{
}

Texture2D::~Texture2D()
{
	RDS_PROFILE_SCOPED();
}


void 
Texture2D::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void 
Texture2D::uploadToGpu(CreateDesc& cDesc)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& tsfReq	= transferContext().transferRequest();
	auto* cmd		= tsfReq.uploadTexture(this);

	checkValid(cDesc);
	destroy();
	onUploadToGpu(cDesc, cmd);
}

void 
Texture2D::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	if (cDesc.uploadImage.isValid())
	{
		uploadToGpu(cDesc);
	}
}

void 
Texture2D::onPostCreate(CreateDesc& cDesc)
{

}

void 
Texture2D::onDestroy()
{


	Base::onDestroy();
}

void 
Texture2D::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd, "");
	Base::create(cDesc);
}

#endif


}