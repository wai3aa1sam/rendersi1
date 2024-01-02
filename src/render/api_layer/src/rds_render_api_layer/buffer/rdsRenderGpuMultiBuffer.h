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
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer::CreateDesc;
	using Desc			= RenderGpuBuffer::Desc;

public:
	static CreateDesc					makeCDesc();
	static SPtr<RenderGpuMultiBuffer>	make(CreateDesc& cDesc);

public:
	RenderGpuMultiBuffer();
	virtual ~RenderGpuMultiBuffer();

	void create(CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	void rotate();

	const Desc& desc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

			SPtr<RenderGpuBuffer>& renderGpuBuffer();
	const	SPtr<RenderGpuBuffer>& renderGpuBuffer() const;

			SPtr<RenderGpuBuffer>& previousBuffer();
	const	SPtr<RenderGpuBuffer>& previousBuffer() const;


protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onUploadToGpu(ByteSpan data, SizeType offset);

	SPtr<RenderGpuBuffer>& makeNextBuffer(SizeType bufSize);

	SPtr<RenderGpuBuffer> _makeNewBuffer(SizeType bufSize);

protected:
	Atm<u32>	_iFrame = 0;
	Desc		_desc;
	Vector<SPtr<RenderGpuBuffer>, s_kFrameInFlightCount> _renderGpuBuffers;
};

inline const RenderGpuMultiBuffer::Desc& RenderGpuMultiBuffer::desc()		const { return _renderGpuBuffers[_iFrame]->desc(); }

inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::stride()		const { return _renderGpuBuffers[_iFrame]->stride(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::bufSize()		const { return _renderGpuBuffers[_iFrame]->bufSize(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::elementCount()	const { return _renderGpuBuffers[_iFrame]->elementCount(); }

inline			SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[_iFrame]; }
inline const	SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::renderGpuBuffer() const	{ return _renderGpuBuffers[_iFrame]; }

inline			SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::previousBuffer()			{ return _renderGpuBuffers[(_iFrame - 1) % s_kFrameInFlightCount]; }
inline const	SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::previousBuffer() const		{ return _renderGpuBuffers[(_iFrame - 1) % s_kFrameInFlightCount]; }


#endif
}

