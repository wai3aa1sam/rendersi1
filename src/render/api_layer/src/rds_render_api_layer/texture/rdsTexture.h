#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsSamplerState-Decl ---
#endif // 0
#if 1

#define SamplerFilter_ENUM_LIST(E) \
	E(None, = 0) \
	E(Nearest,) \
	E(Linear,) \
	E(Bilinear,) \
	E(Trilinear,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(SamplerFilter, u8);

#define SamplerWrap_ENUM_LIST(E) \
	E(None, = 0) \
	E(Repeat,) \
	E(Mirrored,) \
	E(ClampToEdge,) \
	E(ClampToBorder,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(SamplerWrap, u8);

struct SamplerState
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Filter	= SamplerFilter;
	using Wrap		= SamplerWrap;

public:
	Filter	minFliter = Filter::Linear;
	Filter	magFliter = Filter::Linear;

	Wrap	wrapU = Wrap::Repeat;
	Wrap	wrapV = Wrap::Repeat;
	Wrap	wrapS = Wrap::Repeat;

	// TODO: Border Color

	float	minLod	= 0;
	float	maxLod	= math::inf<float>();

	bool isAnisotropy : 1;

	u32 sampleCount = 1;

public:
	SamplerState()
		: isAnisotropy(true)
	{}
};

#endif // 1

#define TextureUsage_ENUM_LIST(E) \
	E(None, = 0) \
	E(ShaderResource,) \
	E(UnorderedAccess,) \
	E(RenderTarget,) \
	E(DepthStencil,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(TextureUsage, u8);

struct Texture_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	TextureUsage	usage			= TextureUsage::ShaderResource;
	ColorType		format			= ColorType::RGBAb;
	SamplerState	samplerState;
	u8				mipCount		= 1;

	class RenderContext* rdCtx = nullptr;		// TODO: remove
};

struct Texture2D_CreateDesc : public Texture_CreateDesc
{
	void create(StrView filename)
	{
		auto imageCDesc = Image::makeCDesc();
		uploadImage.load(filename);
		size	= Vec2u::s_cast(uploadImage.info().size);
		format	= uploadImage.colorType();
	}

	Vec2u size = {0, 0};
	Image uploadImage;
};

#if 0
#pragma mark --- rdsTexture-Decl ---
#endif // 0
#if 1

class Texture : public RenderResource
{
public:
	using Base			= RenderResource;
	using This			= Texture;
	using CreateDesc	= Texture_CreateDesc;

public:
	virtual ~Texture()				= default;

	RenderDataType		type() const;
	const CreateDesc&	textureDesc() const;

	TextureUsage		usage()			const;
	ColorType			format()		const;
	const SamplerState&	samplerState()	const;
	u8					mipCount()		const;

protected:
	Texture(RenderDataType type);

	RenderDataType	_type;
	CreateDesc		_desc;
};

inline RenderDataType				Texture::type()			const { return _type; }
inline const Texture::CreateDesc&	Texture::textureDesc()	const { return _desc; }

inline TextureUsage			Texture::usage()		const	{ return textureDesc().usage; }
inline ColorType			Texture::format()		const	{ return textureDesc().format; }
inline const SamplerState&	Texture::samplerState()	const	{ return textureDesc().samplerState; }
inline u8					Texture::mipCount()		const	{ return textureDesc().mipCount; }

#endif

class Texture1D : public Texture
{

};

#if 0
#pragma mark --- rdsTexture2D-Decl ---
#endif // 0
#if 1

class Texture2D : public Texture
{
	friend class Renderer;
public:
	using Base			= Texture;
	using This			= Texture2D;
	using CreateDesc	= Texture2D_CreateDesc;
	using TextureDesc	= Base::CreateDesc;
	using Desc			= CreateDesc;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Texture2D>	make(const CreateDesc& cDesc);

public:
	Texture2D();
	virtual ~Texture2D();

	void create	(const CreateDesc& cDesc);
	void destroy();

	const Vec2u& size() const;

protected:

	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

protected:
	Vec2u		_size;
};

inline const Vec2u& Texture2D::size() const { return _size; }

#endif

class Texture3D : public Texture
{

};

class TextureCube : public Texture
{

};
}