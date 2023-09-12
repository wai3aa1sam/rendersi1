#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGpuBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuMultiBuffer-Decl ---
#endif // 0
#if 1

class RenderGpuMultiBuffer : public RenderResource
{
	friend class Renderer;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer::CreateDesc;
	using Util			= RenderApiUtil;

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

	const CreateDesc& desc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

			SPtr<RenderGpuBuffer>& renderGpuBuffer();
	const	SPtr<RenderGpuBuffer>& renderGpuBuffer() const;

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

inline const RenderGpuMultiBuffer::CreateDesc& RenderGpuMultiBuffer::desc() const { return _renderGpuBuffers[iFrame]->desc(); }

inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::stride()		const { return _renderGpuBuffers[iFrame]->stride(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::bufSize()		const { return _renderGpuBuffers[iFrame]->bufSize(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::elementCount()	const { return _renderGpuBuffers[iFrame]->elementCount(); }

inline			SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[iFrame]; }
inline const	SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::renderGpuBuffer() const	{ return _renderGpuBuffers[iFrame]; }

#endif
}

