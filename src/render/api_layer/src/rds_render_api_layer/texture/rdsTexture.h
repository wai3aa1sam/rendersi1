#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/shader/rdsBindlessResourceHandle.h"

namespace rds
{

class Texture;
class Texture2D;
class Backbuffers;

class TransferCommand_UploadTexture;

u32 Texture_mipCount(u32 width_, u32 height_);
u32 Texture_mipCount(u32 size);

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

	using HashValue	= SizeType;

public:
	static constexpr SamplerState makeNearestRepeat();
	static constexpr SamplerState makeNearestMirrored();
	static constexpr SamplerState makeNearestClampToEdge();
	static constexpr SamplerState makeNearestClampToBorder();

	static constexpr SamplerState makeLinearRepeat();
	static constexpr SamplerState makeLinearMirrored();
	static constexpr SamplerState makeLinearClampToEdge();
	static constexpr SamplerState makeLinearClampToBorder();

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

	//u32 sampleCount = 1;

public:
	bool		isValidMaxLod() const { return maxLod != math::inf<float>(); }
	HashValue	hash()			const;

	bool operator<( const SamplerState& rhs) const;
	bool operator>( const SamplerState& rhs) const;
	bool operator==(const SamplerState& rhs) const;
	bool operator!=(const SamplerState& rhs) const;
};

#endif // 1

#if 0
#pragma mark --- rdsSamplerState-Impl ---
#endif // 0
#if 1

#define RDS_Fliter(	o, v)	o.minFliter = v;	o.magFliter = v
#define RDS_Wrap(	o, v)	o.wrapU		= v;	o.wrapV		= v;	o.wrapS		= v

inline constexpr SamplerState SamplerState::makeNearestRepeat()				{ SamplerState o; RDS_Fliter(o, Filter::Nearest); RDS_Wrap(o, Wrap::Repeat);		return o; }
inline constexpr SamplerState SamplerState::makeNearestMirrored()			{ SamplerState o; RDS_Fliter(o, Filter::Nearest); RDS_Wrap(o, Wrap::Mirrored);		return o; }
inline constexpr SamplerState SamplerState::makeNearestClampToEdge()		{ SamplerState o; RDS_Fliter(o, Filter::Nearest); RDS_Wrap(o, Wrap::ClampToEdge);	return o; }
inline constexpr SamplerState SamplerState::makeNearestClampToBorder()		{ SamplerState o; RDS_Fliter(o, Filter::Nearest); RDS_Wrap(o, Wrap::ClampToBorder); return o; }

inline constexpr SamplerState SamplerState::makeLinearRepeat()				{ SamplerState o; RDS_Fliter(o, Filter::Linear); RDS_Wrap(o, Wrap::Repeat);			return o; }
inline constexpr SamplerState SamplerState::makeLinearMirrored()			{ SamplerState o; RDS_Fliter(o, Filter::Linear); RDS_Wrap(o, Wrap::Mirrored);		return o; }
inline constexpr SamplerState SamplerState::makeLinearClampToEdge()			{ SamplerState o; RDS_Fliter(o, Filter::Linear); RDS_Wrap(o, Wrap::ClampToEdge);	return o; }
inline constexpr SamplerState SamplerState::makeLinearClampToBorder()		{ SamplerState o; RDS_Fliter(o, Filter::Linear); RDS_Wrap(o, Wrap::ClampToBorder);	return o; }

#undef RDS_Fliter
#undef RDS_Wrap

#endif

#if 0
#pragma mark --- rdsTexture_CreateDesc-Impl ---
#endif // 0
#if 1

struct Texture_Desc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderDataType		type			= RenderDataType::None;
	Tuple3u				size			= {};
	ColorType			format			= ColorType::RGBAb;
	TextureUsageFlags	usageFlags		= TextureUsageFlags::ShaderResource;
	SamplerState		samplerState;
	u8					mipCount		= 1;
	u8					sampleCount		= 1;
	u16					layerCount		= 1;
	bool				isSrgb			= false;

public:
	Texture_Desc()
		: type(RenderDataType::None)
	{

	}

	SizeType totalByteSize() const { return ColorUtil::pixelByteSize(format) * size.x * size.y * size.z; }

protected:
	void move(Texture_Desc&& rhs)
	{
		*this = rhs;
	}
};

struct Texture_CreateDesc : public RenderResource_CreateDescT<Texture_Desc>
{
	friend class Texture2D;
	friend class Renderer;
	friend class TransferContext;
	friend class TransferRequest;
public:
	using Base = RenderResource_CreateDescT<Texture_Desc>;

public:
	RDS_RenderResource_CreateDesc_COMMON_BODY(Texture_CreateDesc);
	//Texture_CreateDesc(RenderDataType type_, u32 width_, u32 height_, u32 depth_, ColorType format_, TextureUsageFlags usageFlags_
	//	, u32 mipCount_, u32 layerCount_, u32 sampleCount_, const SamplerState& samplerState_)
	//	//: type(type_), format(format), flags(usageFlags_), samplerState(samplerState_)
	//	//, size(Tuple3u{width_, height_, depth_}), layerCount(layerCount_), mipCount(mipCount_), sampleCount(sampleCount_)
	//{

	//}

	void create(RenderDataType type_, u32 width_, u32 height_, u32 depth_, ColorType format_, TextureUsageFlags usageFlags_
		, u32 mipCount_, u32 layerCount_, u32 sampleCount_, const SamplerState& samplerState_)
	{
		type			= type_;
		format			= format_;
		usageFlags		= usageFlags_;
		samplerState	= samplerState_;
		size			= Tuple3u{width_, height_, depth_};
		mipCount		= sCast<u8>( mipCount_);
		layerCount		= sCast<u16>(layerCount_);
		sampleCount		= sCast<u8>( sampleCount_);

		samplerState.maxLod = sampleCount;
	}

	void create(RenderDataType type_, u32 width_, u32 height_, u32 depth_, ColorType format_, bool isUseMip, TextureUsageFlags usageFlags_
				, u32 layerCount_, u32 sampleCount_, const SamplerState& samplerState_)
	{
		create(type_, width_, height_, depth_, format_, usageFlags_, isUseMip ? Texture_mipCount(width_, height_) : 1, layerCount_, sampleCount_, samplerState_);
	}

	void create(const Texture_Desc& desc)
	{
		sCast<Texture_Desc&>(*this) = desc;
	}
};

struct Texture2D_CreateDesc : public Texture_CreateDesc
{
	friend class Texture2D;
	friend class Renderer;
	friend class TransferContext;
	friend class TransferRequest;
public:
	using Base = Texture_CreateDesc;
	using Base::create;

public:
	Image uploadImage;

public:
	RDS_RenderResource_CreateDesc_COMMON_BODY(Texture2D_CreateDesc);
	Texture2D_CreateDesc(u32 width_, u32 height_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_ = 1, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture2D, width_, height_, 1, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}

	Texture2D_CreateDesc(Tuple2u size_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_, u32 sampleCount_, const SamplerState& samplerState_)
	{
		Base::create(RenderDataType::Texture2D, size_.x, size_.y, 1, format_, usageFlags_, mipCount_, 1, sampleCount_, samplerState_);
	}

	Texture2D_CreateDesc(Tuple2u size_, ColorType format_, TextureUsageFlags usageFlags_, const SamplerState& samplerState_)
	{
		Base::create(RenderDataType::Texture2D, size_.x, size_.y, 1, format_, usageFlags_, 1, 1, 1, samplerState_);
	}

	Texture2D_CreateDesc(Tuple2u size_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_ = 1, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture2D, size_.x, size_.y, 1, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}

	Texture2D_CreateDesc(Tuple2u size_, ColorType format_, u32 mipCount_, TextureUsageFlags usageFlags_, u32 sampleCount_)
	{
		Base::create(RenderDataType::Texture2D, size_.x, size_.y, 1, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}

	Texture2D_CreateDesc(Tuple2u size_, bool isUseMip, ColorType format_, TextureUsageFlags usageFlags_, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture2D, size_.x, size_.y, 1, format_, isUseMip, usageFlags_, 1, sampleCount_, {});
	}

	void create(StrView filename, TextureUsageFlags usageFlags_ = TextureUsageFlags::ShaderResource)
	{
		uploadImage.load(filename);
		Base::create(RenderDataType::Texture2D
					, sCast<u32>(uploadImage.width()), sCast<u32>(uploadImage.height()), 1
					, uploadImage.info().colorType, usageFlags_
					, 1, 1, 1, {});
	}

	void create(const u8* data, u32 width, u32 height, ColorType format_, TextureUsageFlags usageFlags_= TextureUsageFlags::ShaderResource)
	{
		format = format_;
		size.set(width, height, sCast<u32>(1));
		uploadImage.create(format, width, height);
		uploadImage.copyToPixelData(ByteSpan(data, width * height * ColorUtil::pixelByteSize(format_)));
		Base::create(RenderDataType::Texture2D
			, sCast<u32>(uploadImage.width()), sCast<u32>(uploadImage.height()), 1
			, uploadImage.info().colorType, usageFlags_
			, 1, 1, 1, {});
	}

protected:
	void move(Texture2D_CreateDesc&& rhs)
	{
		Base::move(rds::move(rhs));
		uploadImage	= rds::move(rhs.uploadImage);
	}
};

#endif // 0

#if 0
#pragma mark --- rdsTexture-Decl ---
#endif // 0
#if 1

class Texture : public RenderResource_T<Texture, RenderResourceType::Texture>
{
	friend class Backbuffers;
public:
	using Base				= RenderResource;
	using This				= Texture;
	using CreateDesc		= Texture_CreateDesc;
	using Desc				= Texture_Desc;
	using TextureCreateDesc	= Texture_CreateDesc;

	using Size				= Vec3u;

public:
	virtual ~Texture();
	void	destroy();

public:
	void createRenderResource( const RenderFrameParam& rdFrameParam) { /* dummy function for compatibility */ }
	void destroyRenderResource(const RenderFrameParam& rdFrameParam) { /* dummy function for compatibility */ }

public:
	RenderDataType			type()				const;
	const Desc&				desc()				const;

	TextureUsageFlags		usageFlags()		const;
	ColorType				format()			const;
	const SamplerState&		samplerState()		const;
	u8						mipCount()			const;
	u16						layerCount()		const;
	u8						sampleCount()		const;
	Vec3u					size()				const;

	SizeType				totalByteSize()		const;

	BindlessResourceHandle	bindlessHandle()	const;
	BindlessResourceHandle	uavBindlessHandle()	const;

	bool					hasMipmapView()		const;
	u32						mipmapViewCount()	const;

	bool					isArray()			const;
	bool					isTexture()			const;
	bool					isImage()			const;
	bool					isBackBuffer()		const;

	virtual bool isNull() const = 0;

protected:
	Texture(RenderDataType type);

	virtual void setNull() = 0;	// only use  for swapchain

protected:
	bool isValid(	const Texture_Desc& desc) const;
	bool isValid() const;
	void checkValid(const Texture_Desc& cDesc) const;

protected:
	void onCreate(TextureCreateDesc& cDesc);
	virtual void onDestroy();

public:
	void _internal_requestDestroyObject();

protected:
	Desc					_desc;
	BindlessResourceHandle	_bindlessHnd;
	BindlessResourceHandle	_uavBindlessHnd;
};

inline RenderDataType				Texture::type()					const { return _desc.type; }
inline const Texture::Desc&			Texture::desc()					const { return _desc; }

inline TextureUsageFlags			Texture::usageFlags()			const { return desc().usageFlags; }
inline ColorType					Texture::format()				const { return desc().format; }
inline const SamplerState&			Texture::samplerState()			const { return desc().samplerState; }
inline u8							Texture::mipCount()				const { return desc().mipCount; }
inline u16							Texture::layerCount()			const { return desc().layerCount; }
inline u8							Texture::sampleCount()			const { return desc().sampleCount; }

inline Vec3u						Texture::size()					const { return desc().size; }

inline Texture::SizeType			Texture::totalByteSize()		const { return desc().totalByteSize(); }

inline BindlessResourceHandle		Texture::bindlessHandle()		const { return _bindlessHnd; }
inline BindlessResourceHandle		Texture::uavBindlessHandle()	const { return _uavBindlessHnd; }

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
	static CreateDesc		makeCDesc(RDS_DEBUG_SRCLOC_PARAM = {});
	static SPtr<Texture2D>	make(CreateDesc& cDesc);

public:
	Texture2D();
	virtual ~Texture2D();

	void create(	 CreateDesc& cDesc);
	void uploadToGpu(CreateDesc& cDesc);

public:
	Size size() const;

protected:
	virtual void onCreate(		CreateDesc& cDesc);
	virtual void onPostCreate(	CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

private:
	void _create(CreateDesc& cDesc);

protected:
	//Size _size;
};

inline Texture2D::Size Texture2D::size() const { return Tuple2u{Base::size().x, Base::size().y}; }

#endif

inline
u32 
Texture_mipCount(u32 width_, u32 height_)
{
	const u32 mipCount_ = sCast<u32>(math::floor(sCast<float>(math::log2(math::max(width_, height_))))) + 1;
	return mipCount_;
}

inline
u32 
Texture_mipCount(u32 size)
{
	const u32 mipCount_ = sCast<u32>(math::floor(sCast<float>(math::log2(size)))) + 1;
	return mipCount_;
}

}