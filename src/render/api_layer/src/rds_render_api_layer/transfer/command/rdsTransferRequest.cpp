#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/transfer/command/rds_transfer_command.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferRequest-Impl ---
#endif // 0
#if 1

bool 
TransferRequest::tryPopTransferCommandSafeBuffer(TransferCommandBuffer& dst, TransferCommandSafeBuffer& src)
{
	{
		//auto* p = RDS_NEW(TransferCommandBuffer);
		auto data	= src.scopedULock();
		swap(dst, *data);
	}
	return true;
}

TransferRequest::TransferRequest()
{
	
}

TransferRequest::~TransferRequest()
{
	reset(nullptr, nullptr);
}

void 
TransferRequest::reset(TransferContext* tsfCtx, TransferFrame* tsfFrame)
{
	if (!tsfCtx || !tsfFrame)
		return;

	_tsfCtx	= tsfCtx;

	{
		auto lock = _tsfCmdBuf.scopedULock();
		lock->clear();
	}
}

//void 
//TransferRequest::commit(RenderFrameParam& rdFrameParam, bool isWaitImmediate)
//{
//	RDS_PROFILE_SCOPED();
//	_tsfCtx->commit(rdFrameParam, *this, isWaitImmediate);
//}

TransferCommand_UploadTexture*
TransferRequest::uploadTexture(Texture* tex)
{
	auto* cmd = newCommand<TransferCommand_UploadTexture>();
	cmd->dst = tex;
	return cmd;
}

void 
TransferRequest::uploadBuffer(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset)
{
	#if 0
	TransferCommand_UploadBuffer temp;
	TransferCommand_UploadBuffer* cmd = nullptr;
	if (!rdBuf->isConstantBuffer())
	{
		auto& cmdBuf = transferCommandBuffer();
		cmd = cmdBuf.uploadBuffer();
	}
	else
	{
		cmd = &temp;
	}
	#endif // 0
	//auto& cmdBuf = transferCommandBuffer();
	auto* cmd = newCommand<TransferCommand_UploadBuffer>();

	cmd->dst	= rdBuf;
	cmd->data	= data;
	cmd->offset = offset;

	rdBuf->onUploadToGpu(cmd);
}

void 
TransferRequest::uploadBufferAsync(RenderGpuBuffer* rdBuf, Vector<u8>&& data)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
TransferRequest::setSwapchainSize(RenderContext* rdCtx, const Tuple2f& size)
{
	RDS_CORE_ASSERT(rdCtx);

	//auto& cmdBuf	= transferCommandBuffer();
	auto* cmd			= newCommand<TransferCommand_SetSwapchainSize>();
	cmd->renderContext	= rdCtx;
	cmd->size			= size;
}

#if 1

TransferCommandSafeBuffer& 
TransferRequest::transferCommandBuffer()
{
	//checkMainThreadExclusive(RDS_SRCLOC);
	return _tsfCmdBuf;
}

#endif // 0


#endif

}