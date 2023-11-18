#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

struct RenderGpuBuffer_CreateDesc;
struct RenderGpuBuffer_Desc;

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

struct RenderGpuBuffer_CreateDesc : public RenderResource_CreateDesc
{
public:
	static constexpr SizeType s_kAlign = 16;	

public:
	RenderGpuBufferTypeFlags	typeFlags	= {};
	SizeType					bufSize		= 0;
	SizeType					stride		= s_kAlign;


public:
	RenderGpuBuffer_CreateDesc() = default;
	RenderGpuBuffer_CreateDesc(const RenderGpuBuffer_Desc& desc)
	{
		create(desc);
	}
	void operator=(const RenderGpuBuffer_Desc& desc) { create(desc); }
	
	void create(const RenderGpuBuffer_Desc& desc);
};

struct RenderGpuBuffer_Desc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Type		= RenderGpuBufferTypeFlags;

public:
	static constexpr SizeType s_kAlign = 16;

public:
	RenderGpuBufferTypeFlags	typeFlags	= {};
	SizeType					bufSize		= 0;
	SizeType					stride		= s_kAlign;

public:
	RenderGpuBuffer_Desc() = default;
	RenderGpuBuffer_Desc(const RenderGpuBuffer_Desc& desc)
		: typeFlags(desc.typeFlags), bufSize(desc.bufSize), stride(desc.stride)
	{

	}
	RenderGpuBuffer_Desc(const RenderGpuBuffer_CreateDesc& cDesc)
		: typeFlags(cDesc.typeFlags), bufSize(cDesc.bufSize), stride(cDesc.stride)
	{

	}
};

inline
void 
RenderGpuBuffer_CreateDesc::create(const RenderGpuBuffer_Desc& desc)
{
	typeFlags	= desc.typeFlags;
	bufSize		= desc.bufSize;
	stride		= desc.stride;
}


class RenderGpuBuffer : public RenderResource
{
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer_CreateDesc;
	using Desc			= RenderGpuBuffer_Desc;
	
public:
	static constexpr SizeType s_kAlign = CreateDesc::s_kAlign;

public:
	static CreateDesc				makeCDesc();
	static SPtr<RenderGpuBuffer>	make(CreateDesc& cDesc);

public:
	RenderGpuBuffer();
	virtual ~RenderGpuBuffer();

	void create(CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	const Desc& desc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onUploadToGpu(ByteSpan data, SizeType offset);

protected:
	Desc _desc;
};

inline const RenderGpuBuffer::Desc& RenderGpuBuffer::desc() const { return _desc; }

inline RenderGpuBuffer::SizeType RenderGpuBuffer::stride()			const { return _desc.stride; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::bufSize()			const { return _desc.bufSize; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::elementCount()	const { return _desc.bufSize / _desc.stride; }

#endif
}

