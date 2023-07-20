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
public:
	using Base = RefCount_Base;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	using Util = RenderApiUtil;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kFrameInFlightCount			= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	static CreateDesc					makeCDesc();
	static SPtr<RenderGpuMultiBuffer>	make(const CreateDesc& cDesc);

public:
	RenderGpuMultiBuffer();
	virtual ~RenderGpuMultiBuffer();

	void create(const CreateDesc& cDesc);
	void destroy();

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

	void rotate();
	SPtr<RenderGpuBuffer>& nextBuffer();

protected:
	u32 iFrame = 0;	// TODO: add a global check lock to ensure only one thread call rotate()
	Vector<SPtr<RenderGpuBuffer>, s_kFrameInFlightCount> _renderGpuBuffers;
};

inline const RenderGpuMultiBuffer::CreateDesc& RenderGpuMultiBuffer::cDesc() const { return _renderGpuBuffers[0]->cDesc(); }

inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::stride()		const { return _renderGpuBuffers[0]->stride(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::bufSize()		const { return _renderGpuBuffers[0]->bufSize(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::elementCount()	const { return _renderGpuBuffers[0]->elementCount(); }

inline RenderGpuBuffer*			RenderGpuMultiBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[iFrame]; }
inline const RenderGpuBuffer*	RenderGpuMultiBuffer::renderGpuBuffer() const	{ return _renderGpuBuffers[iFrame]; }

#endif
}

