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
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderGpuBufferTypeFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderGpuBufferTypeFlags);

struct RenderGpuBuffer_CreateDesc
{
public:
	using Type		= RenderGpuBufferTypeFlags;
	using SizeType	= RenderApiLayerTraits::SizeType;

public:
	RenderGpuBufferTypeFlags	typeFlags	= {};
	SizeType					bufSize		= 0;
	SizeType					stride		= 16;
};

class RenderGpuBuffer : public RefCount_Base
{
	friend class Renderer;
public:
	using Base = RefCount_Base;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	using Util = RenderApiUtil;

	using SizeType = CreateDesc::SizeType;

public:
	static constexpr SizeType s_kAlign = 16;

public:
	static CreateDesc				makeCDesc();
	static SPtr<RenderGpuBuffer>	make(const CreateDesc& cDesc);

public:
	RenderGpuBuffer();
	virtual ~RenderGpuBuffer();

	void create(const CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	const CreateDesc& cDesc() const;

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

inline const RenderGpuBuffer::CreateDesc& RenderGpuBuffer::cDesc() const { return _cDesc; }

inline RenderGpuBuffer::SizeType RenderGpuBuffer::stride()			const { return _cDesc.stride; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::bufSize()			const { return _cDesc.bufSize; }
inline RenderGpuBuffer::SizeType RenderGpuBuffer::elementCount()	const { return _cDesc.bufSize / (BitUtil::toByte(_cDesc.stride)); }

#endif
}

