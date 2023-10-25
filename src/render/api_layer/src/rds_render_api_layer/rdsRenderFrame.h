#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "command/rdsTransferRequest.h"
#include "command/rdsRenderCommand.h"
#include "command/rdsRenderQueue.h"

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

	using RenderCommandPool = Vector<UPtr<RenderCommandBuffer>, 12>;

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

	RenderCommandBuffer* requestCommandBuffer();

	RenderFrameUploadBuffer&	renderFrameUploadBuffer();
	TransferRequest&			transferRequest();
	RenderQueue& renderQueue();

	LinearAllocator& renderCommandAllocator();

protected:
	Vector<UPtr<LinearAllocator>,	Traits::s_kThreadCount>	_renderCommandAllocators;
	Vector<RenderCommandPool,		Traits::s_kThreadCount>	_renderCommandPools;

	TransferRequest			_transferReq;
	RenderFrameUploadBuffer _rdfUploadBuffer;

	RenderQueue _renderQueue;
};

inline RenderFrameUploadBuffer& RenderFrame::renderFrameUploadBuffer()	{ return _rdfUploadBuffer; }
inline TransferRequest& RenderFrame::transferRequest()					{ return _transferReq; }

inline u32 RenderFrame::totalUploadDataSize() { return _rdfUploadBuffer.totalDataSize(); }

inline LinearAllocator& RenderFrame::renderCommandAllocator() { auto tlid = OsTraits::threadLocalId(); return *_renderCommandAllocators[tlid]; }

inline RenderQueue& RenderFrame::renderQueue() { return _renderQueue; }

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
	
	void destroy();

	void clear();
	void rotate();

	RenderFrame& renderFrame();

protected:
	Atm<u32> iFrame = 0;
	Vector<RenderFrame,				s_kFrameInFlightCount> _renderFrames;
};

inline RenderFrame& RenderFrameContext::renderFrame() { return _renderFrames[iFrame]; }

#endif

}


