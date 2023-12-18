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

#if 0
#pragma mark --- rdsTexture_CreateDesc-Impl ---
#endif // 0
#if 1

// TextureUsageFlags
#define TextureFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(ShaderResource,) \
	E(UnorderedAccess,) \
	E(RenderTarget,) \
	E(DepthStencil,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(TextureFlags, u8);
RDS_ENUM_ALL_OPERATOR(TextureFlags);

//struct Texture_DescData
//{
//	TextureFlags	flag			= TextureFlags::ShaderResource;
//	ColorType		format			= ColorType::RGBAb;
//	SamplerState	samplerState;
//	Tuple3u			size			= {};
//	u8				mipCount		= 1;
//	u8				sampleCount		= 1;
//};
//

//struct Texture_Desc_Base : public RenderResource_CreateDesc

struct Texture_Desc;
struct Texture_CreateDesc : public RenderResource_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	TextureFlags	flag			= TextureFlags::ShaderResource;
	ColorType		format			= ColorType::RGBAb;
	SamplerState	samplerState;
	Tuple3u			size			= {};
	u8				mipCount		= 1;
	u8				sampleCount		= 1;

public:
	Texture_CreateDesc() = default;
	Texture_CreateDesc(const Texture_Desc& desc);

protected:
	void loadImage(const Image& uploadImage)
	{
		if (uploadImage.isValidSize())
		{
			format	= uploadImage.colorType();
			size.set(sCast<u32>(uploadImage.width()), sCast<u32>(uploadImage.height()), sCast<u32>(1));
		}
	}

	void move(Texture_CreateDesc&& rhs)
	{
		*this = rhs;
	}
};

struct Texture_Desc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	TextureFlags	flag			= TextureFlags::ShaderResource;
	ColorType		format			= ColorType::RGBAb;
	SamplerState	samplerState;
	Tuple3u			size			= {};
	u8				mipCount		= 1;
	u8				sampleCount		= 1;

public:
	Texture_Desc() = default;
	Texture_Desc(const Texture_CreateDesc& cDesc)
		: flag(cDesc.flag), format(cDesc.format), samplerState(cDesc.samplerState)
		, size(cDesc.size), mipCount(cDesc.mipCount), sampleCount(cDesc.sampleCount)
	{

	}
};

inline
Texture_CreateDesc::Texture_CreateDesc(const Texture_Desc& desc)
	: flag(desc.flag), format(desc.format), samplerState(desc.samplerState)
	, size(desc.size), mipCount(desc.mipCount), sampleCount(desc.sampleCount)
{

}

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

	Texture2D_CreateDesc(u32 width, u32 height, ColorType format_, u32 mipCount_, TextureFlags flag_, u32 sampleCount_ = 1)
	{
		create(width, height, format_, mipCount_, flag_, sampleCount_);
	}

	Texture2D_CreateDesc(Tuple2u size, ColorType format_, u32 mipCount_, TextureFlags flag_, u32 sampleCount_ = 1)
		: Texture2D_CreateDesc(size.x, size.y, format_, mipCount_, flag_, sampleCount_)
	{
	}

	Texture2D_CreateDesc(const Texture_Desc& desc)
		: Base(desc)
	{
		_hasDataToUpload = false;
	}

	Texture2D_CreateDesc(Texture2D_CreateDesc&& rhs) { move(rds::move(rhs)); }
	void operator=		(Texture2D_CreateDesc&& rhs) { RDS_CORE_ASSERT(this != &rhs, ""); move(rds::move(rhs)); }

	void create(StrView filename)
	{
		_filename = filename;
		_hasDataToUpload = true;
	}

	void create(Image&& uploadImage)
	{
		_uploadImage = rds::move(uploadImage);
		_hasDataToUpload = true;
	}

	void create(const u8* data, ColorType format_, u32 width, u32 height)
	{
		format = format_;
		size.set(width, height, sCast<u32>(1));
		_uploadImage.create(format, width, height);
		_uploadImage.copyToPixelData(ByteSpan(data, width * height * ColorUtil::pixelByteSize(format_)));

		_hasDataToUpload = true;
	}

	void create(u32 width, u32 height, ColorType format_, u32 mipCount_, TextureFlags flag_, u32 sampleCount_ = 1)
	{
		size.set(width, height, sCast<u32>(1));
		format		= format_;
		flag		= flag_;
		mipCount	= sCast<u8>(mipCount_);
		sampleCount = sCast<u8>(sampleCount_);

		_hasDataToUpload = false;
	}

	const Image&	uploadImage()		const	{ return _uploadImage; }
	bool			hasDataToUpload()	const	{ return _hasDataToUpload; }

protected:
	void loadImage()
	{
		if (!hasDataToUpload())
		{
			//RDS_CORE_LOG("_uploadImage: {}, size: {}, filename: {}", _uploadImage.info().size, size, _filename);
			return;
		}
		/*RDS_CORE_ASSERT( !(
		!(_filename.is_empty() && !_uploadImage.dataPtr()) 
		&& (!_filename.is_empty() && _uploadImage.dataPtr())
		), "Create Texture2D should use either filename or imageUpload, not both");*/

		if (!_filename.is_empty())
		{
			_uploadImage.load(_filename);
		}
		else if(_uploadImage.dataPtr())
		{

		}
		//RDS_CORE_LOG("_uploadImage: {}, size: {}", _uploadImage.info().size, size);

		Base::loadImage(_uploadImage);
	}

protected:
	void move(Texture2D_CreateDesc&& rhs)
	{
		Base::move(rds::move(rhs));
		//_size				= rhs._size;
		_uploadImage		= rds::move(rhs._uploadImage);
		_filename			= rds::move(rhs._filename);
		_hasDataToUpload	= rhs._hasDataToUpload;
	}

protected:
	//Tuple2u	_size = {0, 0};
	Image	_uploadImage;
	String	_filename;
	bool	_hasDataToUpload : 1;
};

#endif

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
	using Desc			= Texture_Desc;

public:
	virtual ~Texture()				= default;

	void create	(CreateDesc& cDesc);
	void destroy();

public:
	RenderDataType		type()			const;
	const Desc&			desc()			const;

	TextureFlags		flag()			const;
	ColorType			format()		const;
	const SamplerState&	samplerState()	const;
	u8					mipCount()		const;

	Vec3u				size()			const;

protected:
	Texture(RenderDataType type);

	RenderDataType	_type;
	Desc			_desc;
};

inline RenderDataType				Texture::type()			const { return _type; }
inline const Texture::Desc&			Texture::desc()			const { return _desc; }

inline TextureFlags					Texture::flag()		const { return desc().flag; }
inline ColorType					Texture::format()		const { return desc().format; }
inline const SamplerState&			Texture::samplerState()	const { return desc().samplerState; }
inline u8							Texture::mipCount()		const { return desc().mipCount; }

inline Vec3u						Texture::size()			const { return desc().size; }

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
	using Desc			= Base::Desc;

	using Size			= Tuple2u;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Texture2D>	make(CreateDesc& cDesc);

public:
	Texture2D();
	virtual ~Texture2D();

	void create	(CreateDesc& cDesc);
	void destroy();

	void _internal_uploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

	Size size() const;

protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

private:
	void _create(CreateDesc& cDesc);

protected:
	//Size _size;
};

inline Texture2D::Size Texture2D::size() const { return Tuple2u{Base::size().x, Base::size().y}; }

#endif

class Texture3D : public Texture
{

};

class TextureCube : public Texture
{

};

}