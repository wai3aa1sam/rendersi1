#include "rds_render_api_layer-pch.h"

#include "rdsRenderFrame.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderFrameUploadBuffer-Impl ---
#endif // 0
#if 1

RenderFrameUploadBuffer::RenderFrameUploadBuffer()
{
}

RenderFrameUploadBuffer::~RenderFrameUploadBuffer()
{
	clear();
}

void 
RenderFrameUploadBuffer::clear()
{
	_totalDataSize = 0;

	auto& uploadBuf = *_inlineUploadBuffer.scopedULock();

	for (auto& e: uploadBuf.uploadBufCmds)
	{
		e->~TransferCommand_UploadBuffer();
	}
	uploadBuf.uploadBufCmds.clear();

	uploadBuf.allocator.clear();
	uploadBuf.bufData.clear();
	uploadBuf.bufOffsets.clear();
	uploadBuf.bufSizes.clear();
	uploadBuf.parents.clear();
}

void 
RenderFrameUploadBuffer::uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags)
{
	if (data.is_empty())
	{
		return;
	}

	auto* cmd = _addData(data);
	cmd->dst			= dst;
	cmd->data			= data;
	cmd->queueTypeflags = queueTypeflags;
	RDS_ASSERT(BitUtil::has(queueTypeflags, QueueTypeFlags::Transfer), "");
}

void 
RenderFrameUploadBuffer::uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags, RenderGpuMultiBuffer* parent)
{
	uploadBuffer(dst, data, queueTypeflags);
	addParent(parent);
}

void 
RenderFrameUploadBuffer::addParent(RenderGpuMultiBuffer* parent)
{
	auto& uploadBuf = *_inlineUploadBuffer.scopedULock();
	uploadBuf.parents.emplace_back(parent);
}

TransferCommand_UploadBuffer* 
RenderFrameUploadBuffer::_addData(ByteSpan data)
{
	SizeType	bufOffset	= 0;
	u32			bufSize		= sCast<u32>(data.size());

	_totalDataSize += bufSize;

	auto& uploadBuf = *_inlineUploadBuffer.scopedULock();

	auto* dst = reinCast<u8*>(uploadBuf.bufData.alloc(data.size(), RenderGpuBuffer::s_kAlign, 0, &bufOffset));
	memory_copy(dst, data.data(), data.size());
	uploadBuf.bufOffsets.emplace_back(sCast<u32>(bufOffset));
	uploadBuf.bufSizes.emplace_back(bufSize);

	using Cmd_UploadBuf = TransferCommand_UploadBuffer;
	auto* buf = uploadBuf.allocator.alloc(sizeof(Cmd_UploadBuf));
	auto* cmd = new(buf) Cmd_UploadBuf();
	uploadBuf.uploadBufCmds.emplace_back(cmd);

	return cmd;
}

#endif

#if 0
#pragma mark --- rdsRenderFrame-Impl ---
#endif // 0
#if 1

RenderFrame::RenderFrame()
{

}

RenderFrame::~RenderFrame()
{
	clear();
}

void 
RenderFrame::clear()
{
	_transferReq.clear();
	_rdfUploadBuffer.clear();
}

void 
RenderFrame::uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags)
{
	_rdfUploadBuffer.uploadBuffer(dst, data, queueTypeflags);
}

void 
RenderFrame::uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags, RenderGpuMultiBuffer* parent)
{
	_rdfUploadBuffer.uploadBuffer(dst, data, queueTypeflags, parent);
}

void 
RenderFrame::addUploadBufferParent(RenderGpuMultiBuffer* parent)
{
	_rdfUploadBuffer.addParent(parent);
}

#endif

#if 0
#pragma mark --- rdsRenderFrameContext-Impl ---
#endif // 0
#if 1

RenderFrameContext* RenderFrameContext::s_instance = nullptr;

RenderFrameContext::RenderFrameContext()
	: Base()
{
	_renderFrames.resize(s_kFrameInFlightCount);
}

RenderFrameContext::~RenderFrameContext()
{
}

void 
RenderFrameContext::clear()
{
	_renderFrames.clear();
}

void 
RenderFrameContext::rotate()
{
	iFrame = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
	renderFrame().clear();
}

#endif

}