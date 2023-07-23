#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "command/rdsTransferRequest.h"

namespace rds
{


#if 0
#pragma mark --- rdsRenderFrameUploadBuffer-Decl ---
#endif // 0
#if 1

class RenderFrameUploadBuffer : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= Traits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= Traits::s_kFrameInFlightCount;
	static constexpr SizeType s_kLocalSize			= 64;

private:
	struct InlineUploadBuffer;

public:
	RenderFrameUploadBuffer();
	~RenderFrameUploadBuffer();

	void clear();

	void uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags);
	void uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags, RenderGpuMultiBuffer* parent);

	void addParent(RenderGpuMultiBuffer* parent);

	u32 totalDataSize();

	const InlineUploadBuffer* getInlineUploadBuffer();

protected:
	RDS_NODISCARD TransferCommand_UploadBuffer* _addData(ByteSpan data);

private:
	struct InlineUploadBuffer
	{
		LinearAllocator										allocator;
		Vector<TransferCommand_UploadBuffer*, s_kLocalSize> uploadBufCmds;
		LinearAllocator										bufData;
		Vector<u32, s_kLocalSize>							bufOffsets;
		Vector<u32, s_kLocalSize>							bufSizes;
		Vector<SPtr<RenderGpuMultiBuffer>, s_kLocalSize>	parents;
	};

	Atm<u32> _totalDataSize = 0;
	MutexProtected<InlineUploadBuffer> _inlineUploadBuffer; // TODO: better thread-safe approach (eg. SMtx if assumed only 1 LinearAllocator::Chunk, or per thread LinearAlloc)
};

inline u32 RenderFrameUploadBuffer::totalDataSize() { return _totalDataSize; }
inline const RenderFrameUploadBuffer::InlineUploadBuffer* RenderFrameUploadBuffer::getInlineUploadBuffer() { return _inlineUploadBuffer.scopedULock().operator->(); }


#endif

#if 0
#pragma mark --- rdsRenderFrame-Decl ---
#endif // 0
#if 1

class RenderFrame : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= Traits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= Traits::s_kFrameInFlightCount;

public:
	RenderFrame();
	~RenderFrame();

	RenderFrame(RenderFrame&&)		{ throwIf(true, ""); };
	void operator=(RenderFrame&&)	{ throwIf(true, ""); };

	void clear();

	void uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags);
	void uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags, RenderGpuMultiBuffer* parent);
	void addUploadBufferParent(RenderGpuMultiBuffer* parent);
	u32  totalUploadDataSize();

	RenderFrameUploadBuffer&	renderFrameUploadBuffer();
	TransferRequest&			transferRequest();

protected:
	TransferRequest			_transferReq;
	RenderFrameUploadBuffer _rdfUploadBuffer;
};

inline RenderFrameUploadBuffer& RenderFrame::renderFrameUploadBuffer()	{ return _rdfUploadBuffer; }
inline TransferRequest& RenderFrame::transferRequest()					{ return _transferReq; }

inline u32 RenderFrame::totalUploadDataSize() { return _rdfUploadBuffer.totalDataSize(); }


#endif

#if 0
#pragma mark --- rdsRenderFrameContext-Decl ---
#endif // 0
#if 1


class RenderFrameContext : public StackSingleton<RenderFrameContext>
{
public:
	using Base = StackSingleton<RenderFrameContext>;
	using Traits	= RenderApiLayerTraits;
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= Traits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= Traits::s_kFrameInFlightCount;

public:
	RenderFrameContext();
	~RenderFrameContext();

	void clear();
	void rotate();

	RenderFrame& renderFrame();

protected:
	Atm<u32> iFrame = 0;
	Vector<RenderFrame,				s_kFrameInFlightCount> _renderFrames;
	Vector<RenderFrameUploadBuffer, s_kFrameInFlightCount> _renderFrameUploadBufs;

};

inline RenderFrame& RenderFrameContext::renderFrame() { return _renderFrames[iFrame]; }


#endif

}