#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuBuffer-Decl ---
#endif // 0
#if 1

#define RenderGpuBufferTypeFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(Vertex,		= BitUtil::bit(0)) \
	E(Index,		= BitUtil::bit(1)) \
	E(Const,		= BitUtil::bit(2)) \
	E(TransferSrc,	= BitUtil::bit(3)) \
	E(TransferDst,	= BitUtil::bit(4)) \
	E(Compute,		= BitUtil::bit(5)) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderGpuBufferTypeFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderGpuBufferTypeFlags);

struct RenderGpuBuffer_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Type		= RenderGpuBufferTypeFlags;
	static constexpr SizeType s_kAlign = 16;

public:
	RenderGpuBufferTypeFlags	typeFlags	= {};
	SizeType					bufSize		= 0;
	SizeType					stride		= 16;
};

class RenderGpuBuffer : public RenderResource
{
	friend class Renderer;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer_CreateDesc;
	using Util			= RenderApiUtil;

public:
	static constexpr SizeType s_kAlign = CreateDesc::s_kAlign;

public:
	static CreateDesc				makeCDesc();
	static SPtr<RenderGpuBuffer>	make(const CreateDesc& cDesc);

public:
	RenderGpuBuffer();
	virtual ~RenderGpuBuffer();

	void create(const CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	const CreateDesc& desc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onUploadToGpu(ByteSpan data, SizeType offset);

protected:
	CreateDesc _cDesc;
};

inline const RenderGpuBuffer::CreateDesc& RenderGpuBuffer::desc() const { return _cDesc; }

inline RenderGpuBuffer::SizeType RenderGpuBuffer::stride()			const { return _cDesc.stride; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::bufSize()			const { return _cDesc.bufSize; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::elementCount()	const { return _cDesc.bufSize / _cDesc.stride; }

#endif
}

