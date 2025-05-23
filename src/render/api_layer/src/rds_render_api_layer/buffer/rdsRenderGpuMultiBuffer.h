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
	static CreateDesc					makeCDesc(RDS_DEBUG_SRCLOC_PARAM);
	static SPtr<RenderGpuMultiBuffer>	make(CreateDesc& cDesc);

public:
	RenderGpuMultiBuffer();
	virtual ~RenderGpuMultiBuffer();

	/*
	* should not let other to use the buffer before upload
	*/
	void create(CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	void rotate();

	virtual void setDebugName(StrView name) override;

public:
	u32		iFrame()	const;
	bool	isEmpty()	const;

	const Desc& desc() const;

	SizeType stride()		const;
	SizeType bufSize()		const;
	SizeType elementCount()	const;

	/*
	* only call this after uploadToGpu()
	*/
			RenderGpuBuffer* renderGpuBuffer();
	const	RenderGpuBuffer* renderGpuBuffer() const;

			RenderGpuBuffer* previousBuffer();
	const	RenderGpuBuffer* previousBuffer() const;


protected:
	virtual void onCreate		(CreateDesc& cDesc);
	virtual void onPostCreate	(CreateDesc& cDesc);
	virtual void onDestroy		();

	SPtr<RenderGpuBuffer>& makeBufferOnDemand(SizeType bufSize);

	SPtr<RenderGpuBuffer> _makeNewBuffer(SizeType bufSize);

protected:
	Atm<u32>	_iFrame = 0;
	Desc		_desc;
	Vector<SPtr<RenderGpuBuffer>, s_kFrameInFlightCount> _renderGpuBuffers;
};

inline u32 RenderGpuMultiBuffer::iFrame() const { return _iFrame; }
inline bool RenderGpuMultiBuffer::isEmpty() const { return _renderGpuBuffers.is_empty(); }

inline const RenderGpuMultiBuffer::Desc& RenderGpuMultiBuffer::desc()		const	{ return _renderGpuBuffers[_iFrame]->desc(); }

inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::stride()		const	{ return _renderGpuBuffers[_iFrame]->stride(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::bufSize()		const	{ return _renderGpuBuffers[_iFrame]->bufSize(); }
inline RenderGpuMultiBuffer::SizeType RenderGpuMultiBuffer::elementCount()	const	{ return _renderGpuBuffers[_iFrame]->elementCount(); }

inline			RenderGpuBuffer* RenderGpuMultiBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[_iFrame]; }
inline const	RenderGpuBuffer* RenderGpuMultiBuffer::renderGpuBuffer() const		{ return _renderGpuBuffers[_iFrame]; }

inline			RenderGpuBuffer* RenderGpuMultiBuffer::previousBuffer()				{ return _renderGpuBuffers[(sCast<int>(_iFrame) - 1) % s_kFrameInFlightCount]; }
inline const	RenderGpuBuffer* RenderGpuMultiBuffer::previousBuffer() const		{ return _renderGpuBuffers[(sCast<int>(_iFrame) - 1) % s_kFrameInFlightCount]; }


#endif
}

