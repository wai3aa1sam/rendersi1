#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class Texture;
class Texture2D;

class TransferCommand_UploadTexture;

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

public:

protected:
	void loadImage(const Image& uploadImage)
	{
		format = uploadImage.colorType();
	}
};

struct Texture2D_CreateDesc : public Texture_CreateDesc
{
public:
	friend class Texture2D;
	friend class Renderer;
	friend class TransferContext;
	friend class TransferRequest;

public:
	using Base = Texture_CreateDesc;

public:
	Texture2D_CreateDesc() = default;

	Texture2D_CreateDesc(Texture2D_CreateDesc&& rhs) { move(rds::move(rhs)); }
	void operator=		(Texture2D_CreateDesc&& rhs) { RDS_CORE_ASSERT(this != &rhs, ""); move(rds::move(rhs)); }

	void create(StrView filename)
	{
		_filename = filename;
	}

	void create(Image&& uploadImage)
	{
		_uploadImage = rds::move(uploadImage);
	}

	const Image& uploadImage() const { return _uploadImage; }

protected:
	void loadImage()
	{
		RDS_CORE_ASSERT(!_filename.is_empty() && !_uploadImage.dataPtr(), "loadImage fail, forgot to call ::create() ?");
		if (!_filename.is_empty())
		{
			_uploadImage.load(_filename);
		}
		else if(_uploadImage.dataPtr())
		{
		}

		Base::loadImage(_uploadImage);
		_size = Vec2u::s_cast(_uploadImage.info().size).toTuple2();
	}

protected:
	void move(Texture2D_CreateDesc&& rhs)
	{
		_size			= rhs._size;
		_uploadImage	= rds::move(rhs._uploadImage);
		_filename		= rds::move(rhs._filename);
	}

protected:
	Tuple2u	_size = {0, 0};
	Image	_uploadImage;
	String	_filename;
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

	void create	(CreateDesc& cDesc);
	void destroy();

public:
	RenderDataType		type() const;
	const CreateDesc&	textureDesc() const;

	TextureUsage		usage()			const;
	ColorType			format()		const;
	const SamplerState&	samplerState()	const;
	u8					mipCount()		const;

	Vec3u				size()			const;

protected:

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
	static SPtr<Texture2D>	make(CreateDesc& cDesc);

public:
	Texture2D();
	virtual ~Texture2D();

	void create	(CreateDesc& cDesc);
	void destroy();

	void _internal_uploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

	const Vec2u& size() const;

protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

private:
	void _create(CreateDesc& cDesc);

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