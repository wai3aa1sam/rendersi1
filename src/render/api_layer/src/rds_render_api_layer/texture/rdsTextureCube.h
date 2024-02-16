#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTexture.h"

namespace rds
{

#define CubemapFace_ENUM_LIST(E) \
	E(None, = 0) \
	E(PositiveX,) \
	E(NegativeX,) \
	E(PositiveY,) \
	E(NegativeY,) \
	E(PositiveZ,) \
	E(NegativeZ,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(CubemapFace, u8);

struct TextureCube_CreateDesc : public Texture_CreateDesc
{
public:
	friend class TextureCube;
	friend class Renderer;
	friend class TransferContext;
	friend class TransferRequest;
	RDS_RenderResource_CreateDesc_COMMON_BODY(TextureCube_CreateDesc);

public:
	using Base = Texture_CreateDesc;

public:
	static constexpr SizeType s_kFaceCount = 6;

public:
	Vector<Image, s_kFaceCount> uploadImages;

public:
	void create(Span<StrView> filenames, TextureUsageFlags usageFlags_ = TextureUsageFlags::ShaderResource)
	{
		for (auto& e : filenames)
		{
			auto& uploadImage = uploadImages.emplace_back();
			uploadImage.load(e);

			Base::create(RenderDataType::TextureCube
				, sCast<u32>(uploadImage.width()), sCast<u32>(uploadImage.height()), 1
				, uploadImage.info().colorType, usageFlags_
				, 1, s_kFaceCount, 1, {});
		}
	}

	void create(u32 size_, ColorType format_, TextureUsageFlags usageFlags_, u32 mipCount_ = 1, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::TextureCube, size_, size_, 1, format_, usageFlags_, mipCount_, s_kFaceCount, sampleCount_, {});
	}

	void create(u32 size_, ColorType format_, bool isUseMip, TextureUsageFlags usageFlags_, u32 sampleCount_ = 1)
	{
		Base::create(RenderDataType::TextureCube, size_, size_, 1, format_, isUseMip, usageFlags_, s_kFaceCount, sampleCount_, {});
	}

	void create(u32 size_, ColorType format_, bool isUseMip, TextureUsageFlags usageFlags_, const SamplerState& samplerState_)
	{
		Base::create(RenderDataType::TextureCube, size_, size_, 1, format_, isUseMip, usageFlags_, s_kFaceCount, 1, samplerState_);
	}
	
};


#if 0
#pragma mark --- rdsTextureCube-Decl ---
#endif // 0
#if 1

class TextureCube : public Texture
{
public:
	using Base			= Texture;
	using This			= TextureCube;
	using CreateDesc	= TextureCube_CreateDesc;
	using TextureDesc	= Base::CreateDesc;
	using Desc			= Base::Desc;

	using Size			= u32;

public:
	static constexpr SizeType s_kFaceCount = CreateDesc::s_kFaceCount;

public:
	static CreateDesc			makeCDesc(RDS_DEBUG_SRCLOC_PARAM = {});
	static SPtr<TextureCube>	make(CreateDesc& cDesc);

public:
	TextureCube();
	virtual ~TextureCube();

	void create		(CreateDesc& cDesc);
	void uploadToGpu(CreateDesc& cDesc);

	Size size() const;

protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onUploadToGpu	(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd);

};

inline TextureCube::Size TextureCube::size() const { return Base::size().x; }


#endif


}