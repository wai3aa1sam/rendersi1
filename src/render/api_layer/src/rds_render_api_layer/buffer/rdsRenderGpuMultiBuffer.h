#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGpuBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuMultiBuffer-Decl ---
#endif // 0
#if 1

class RenderGpuMultiBuffer : public RefCount_Base
{
	friend class Renderer;

public:
	using Base = RefCount_Base;
	using CreateDesc = RenderGpuBuffer::CreateDesc;

	using Util = RenderApiUtil;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kFrameInFlightCount	= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	static CreateDesc					makeCDesc();
	static SPtr<RenderGpuMultiBuffer>	make(const CreateDesc& cDesc);

public:
	RenderGpuMultiBuffer();
	virtual ~RenderGpuMultiBuffer();

	void create(const CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	void rotate();

	const CreateDesc& cDesc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

	RenderGpuBuffer* renderGpuBuffer();
	const RenderGpuBuffer* renderGpuBuffer() const;

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onUploadToGpu(ByteSpan data, SizeType offset);

	SPtr<RenderGpuBuffer>& nextBuffer(SizeType bufSize);

protected:
	Atm<u32> iFrame = 0;
	Vector<SPtr<RenderGpuBuffer>, s_kFrameInFlightCount> _renderGpuBuffers;
};

inline const RenderGpuMultiBuffer::CreateDesc& RenderGpuMultiBuffer::cDesc() const { return _renderGpuBuffers[iFrame]->cDesc(); }

inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::stride()		const { return _renderGpuBuffers[iFrame]->stride(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::bufSize()		const { return _renderGpuBuffers[iFrame]->bufSize(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::elementCount()	const { return _renderGpuBuffers[iFrame]->elementCount(); }

inline RenderGpuBuffer*			RenderGpuMultiBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[iFrame]; }
inline const RenderGpuBuffer*	RenderGpuMultiBuffer::renderGpuBuffer() const	{ return _renderGpuBuffers[iFrame]; }

#endif
}

