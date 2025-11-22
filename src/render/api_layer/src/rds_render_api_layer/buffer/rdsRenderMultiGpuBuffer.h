#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGpuBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderMultiGpuBuffer-Decl ---
#endif // 0
#if 1

class RenderMultiGpuBuffer : public RenderResource
{
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderGpuBuffer::CreateDesc;
	using Desc			= RenderGpuBuffer::Desc;

public:
	static CreateDesc					makeCDesc(RDS_DEBUG_SRCLOC_PARAM);
	static SPtr<RenderMultiGpuBuffer>	make(CreateDesc& cDesc);

public:
	RenderMultiGpuBuffer();
	virtual ~RenderMultiGpuBuffer();

	/*
	* should not let other to use the buffer before upload
	*/
	void create(CreateDesc& cDesc);
	void destroy();

	void uploadToGpu(ByteSpan data, SizeType offset = 0);

	void rotate();

	virtual void setDebugName(StrView name) override;

public:
	int		iFrame()	const;
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
	Atm<int>	_iFrame = 0;
	Desc		_desc;

	static constexpr SizeType s_kMaxBufferCount = s_kMaxFrameAheadCountHardLimit;
	using Buffers = Vector<SPtr<RenderGpuBuffer>, s_kMaxBufferCount>;
	Buffers _renderGpuBuffers;
};

inline int RenderMultiGpuBuffer::iFrame() const { return _iFrame; }
inline bool RenderMultiGpuBuffer::isEmpty() const { return _renderGpuBuffers.is_empty(); }

inline const RenderMultiGpuBuffer::Desc& RenderMultiGpuBuffer::desc()		const	{ return _renderGpuBuffers[_iFrame]->desc(); }

inline RenderMultiGpuBuffer::SizeType RenderMultiGpuBuffer::stride()		const	{ return _renderGpuBuffers[_iFrame]->stride(); }
inline RenderMultiGpuBuffer::SizeType RenderMultiGpuBuffer::bufSize()		const	{ return _renderGpuBuffers[_iFrame]->bufSize(); }
inline RenderMultiGpuBuffer::SizeType RenderMultiGpuBuffer::elementCount()	const	{ return _renderGpuBuffers[_iFrame]->elementCount(); }

inline			RenderGpuBuffer* RenderMultiGpuBuffer::renderGpuBuffer()			{ return _renderGpuBuffers[_iFrame]; }
inline const	RenderGpuBuffer* RenderMultiGpuBuffer::renderGpuBuffer() const		{ return _renderGpuBuffers[_iFrame]; }

inline			RenderGpuBuffer* RenderMultiGpuBuffer::previousBuffer()				{ return _renderGpuBuffers[(sCast<int>(_iFrame) - 1) % s_kMaxBufferCount]; }
inline const	RenderGpuBuffer* RenderMultiGpuBuffer::previousBuffer() const		{ return _renderGpuBuffers[(sCast<int>(_iFrame) - 1) % s_kMaxBufferCount]; }


#endif
}

