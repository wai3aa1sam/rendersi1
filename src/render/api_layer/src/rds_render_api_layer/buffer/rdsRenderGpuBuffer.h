#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/shader/rdsBindlessResourceHandle.h"

namespace rds
{

struct RenderGpuBuffer_CreateDesc;
struct RenderGpuBuffer_Desc;

class TransferCommand_UploadBuffer;

#if 0
#pragma mark --- rdsRenderGpuBuffer-Decl ---
#endif // 0
#if 1

#define RenderGpuBufferFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(CpuPrefer,) \
	E(GpuPrefer, ) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderGpuBufferFlags, u8);


#if 0
#pragma mark --- rdsRenderGpuBuffer_CreateDesc-Decl ---
#endif // 0
#if 1

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
};

struct RenderGpuBuffer_CreateDesc : public RenderResource_CreateDescT<RenderGpuBuffer_Desc>
{
public:
	RDS_RenderResource_CreateDesc_COMMON_BODY(RenderGpuBuffer_CreateDesc);

public:
	SizeType offset	= 0;

	void create(const RenderGpuBuffer_Desc& desc)
	{
		sCast<RenderGpuBuffer_Desc&>(*this) = desc;
	}

	RenderGpuBuffer_CreateDesc(SizeType	bufSize_, SizeType stride_, RenderGpuBufferTypeFlags usageFlags_, SizeType offset_ = 0)
	{
		bufSize		= bufSize_;
		stride		= stride_;
		typeFlags	= usageFlags_;
		offset		= offset_;
	}
};

#endif

class RenderGpuBuffer : public RenderResource_T<RenderGpuBuffer, RenderResourceType::RenderGpuBuffer>
{
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer_CreateDesc;
	using Desc			= RenderGpuBuffer_Desc;

	using TypeFlags		= RenderGpuBufferTypeFlags;
	
public:
	static constexpr SizeType s_kAlign = CreateDesc::s_kAlign;

public:
	static CreateDesc				makeCDesc(RDS_DEBUG_SRCLOC_PARAM);
	static SPtr<RenderGpuBuffer>	make(CreateDesc& cDesc);

public:
	RenderGpuBuffer();
	virtual ~RenderGpuBuffer();

	void create(CreateDesc& cDesc);
	void destroy();

	void uploadToGpu		(ByteSpan data, SizeType offset = 0);
	void uploadToGpuAsync	(CreateDesc& cDesc);

	virtual void	onUploadToGpu	(TransferCommand_UploadBuffer* cmd);		// should not call directly

public:
	const Desc& desc() const;

	SizeType	stride()		const;
	SizeType	bufSize()		const;
	SizeType	elementCount()	const;
	TypeFlags	typeFlags()		const;

	bool isConstantBuffer()	const;
	bool isComputeBuffer()	const;

	BindlessResourceHandle	bindlessHandle() const;

public:
	void _internal_requestDestroyObject();

protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

protected:
	Desc					_desc;
	BindlessResourceHandle	_bindlessHnd;
};

inline const RenderGpuBuffer::Desc& RenderGpuBuffer::desc() const { return _desc; }

inline RenderGpuBuffer::SizeType	RenderGpuBuffer::stride()			const { return _desc.stride; }
inline RenderGpuBuffer::SizeType	RenderGpuBuffer::bufSize()			const { return _desc.bufSize; }
inline RenderGpuBuffer::SizeType	RenderGpuBuffer::elementCount()		const { return _desc.bufSize / _desc.stride; }
inline RenderGpuBuffer::TypeFlags	RenderGpuBuffer::typeFlags()		const { return _desc.typeFlags; }

inline BindlessResourceHandle		RenderGpuBuffer::bindlessHandle()	const { return _bindlessHnd; }

inline bool							RenderGpuBuffer::isConstantBuffer()	const { return BitUtil::hasAny(typeFlags(), RenderGpuBufferTypeFlags::Constant); }
inline bool							RenderGpuBuffer::isComputeBuffer()	const { return BitUtil::hasAny(typeFlags(), RenderGpuBufferTypeFlags::Compute); }

#endif
}

