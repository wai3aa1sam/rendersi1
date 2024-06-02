#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTexture.h"

namespace rds
{

class Texture;
class Texture2D;
class Backbuffers;

class TransferCommand_UploadTexture;

struct Texture3D_CreateDesc : public Texture_CreateDesc
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
	RDS_RenderResource_CreateDesc_COMMON_BODY(Texture3D_CreateDesc);
	Texture3D_CreateDesc(u32 width_, u32 height_, u32 depth_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_ = 1, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture3D, width_, height_, depth_, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}

	Texture3D_CreateDesc(Tuple3u size_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_, u32 sampleCount_, const SamplerState& samplerState_)
	{
		Base::create(RenderDataType::Texture3D, size_.x, size_.y, size_.z, format_, usageFlags_, mipCount_, 1, sampleCount_, samplerState_);
	}

	Texture3D_CreateDesc(Tuple3u size_, ColorType format_, TextureUsageFlags usageFlags_, const SamplerState& samplerState_)
	{
		Base::create(RenderDataType::Texture3D, size_.x, size_.y, size_.z, format_, usageFlags_, 1, 1, 1, samplerState_);
	}

	Texture3D_CreateDesc(Tuple3u size_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_ = 1, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture3D, size_.x, size_.y, size_.z, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}

	Texture3D_CreateDesc(Tuple3u size_, ColorType format_, u32 mipCount_, TextureUsageFlags usageFlags_, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture3D, size_.x, size_.y, size_.z, format_, usageFlags_, mipCount_, 1, sampleCount_, {});
	}
	
	Texture3D_CreateDesc(Tuple3u size_, bool isUseMip, ColorType format_, TextureUsageFlags usageFlags_, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::Texture3D, size_.x, size_.y, size_.z, format_, isUseMip, usageFlags_, 1, sampleCount_, {});
	}

	void create(StrView filename, TextureUsageFlags usageFlags_ = TextureUsageFlags::ShaderResource)
	{
		uploadImage.load(filename);
		Base::create(RenderDataType::Texture3D
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
		Base::create(RenderDataType::Texture3D
			, sCast<u32>(uploadImage.width()), sCast<u32>(uploadImage.height()), 1
			, uploadImage.info().colorType, usageFlags_
			, 1, 1, 1, {});
	}

protected:
	void move(Texture3D_CreateDesc&& rhs)
	{
		Base::move(rds::move(rhs));
		uploadImage	= rds::move(rhs.uploadImage);
	}
};

#if 0
#pragma mark --- rdsTexture3D-Decl ---
#endif // 0
#if 1

class Texture3D : public Texture
{
	friend class Renderer;
public:
	using Base			= Texture;
	using This			= Texture3D;
	using CreateDesc	= Texture3D_CreateDesc;
	using TextureDesc	= Base::CreateDesc;
	using Desc			= Base::Desc;

	using Size			= Tuple3u;

public:
	static CreateDesc		makeCDesc(RDS_DEBUG_SRCLOC_PARAM = {});
	static SPtr<Texture3D>	make(CreateDesc& cDesc);

public:
	Texture3D();
	virtual ~Texture3D();

	void create		(CreateDesc& cDesc);
	void uploadToGpu(CreateDesc& cDesc);

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

#endif

}