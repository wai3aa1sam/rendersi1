#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "command/rds_transfer_command.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferContext-Impl ---
#endif // 0
#if 1

TransferContext::CreateDesc				
TransferContext::makeCDesc()
{
	return CreateDesc{};
}

SPtr<TransferContext>	
TransferContext::make(const CreateDesc& cDesc)
{
	_notYetSupported(RDS_SRCLOC);
	return nullptr;
	//Renderer::renderDevice()->cr
}

TransferContext::TransferContext()
{

}

TransferContext::~TransferContext()
{

}

void 
TransferContext::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
TransferContext::destroy()
{
	if (!hasCreated())
		return;

	destroyRenderResources(renderFrameParam());

	onDestroy();
	Base::destroy();
}

void 
TransferContext::transferBegin()
{
	createRenderResources(renderFrameParam());		// should pass from fn
	onTransferBegin();
}

void TransferContext::transferEnd()
{
	onTransferEnd();
	destroyRenderResources(renderFrameParam());		// should pass from fn
}
void 
TransferContext::commit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)
{
	auto* rdDev	= renderDevice();

	onCommit(rdFrameParam, tsfReq, isWaitImmediate);
	rdDev->bindlessResource().commit();
}

void 
TransferContext::waitFrameFinished(RenderFrameParam& rdFrameParam)
{
	RDS_ASSERT(false, "shd not be called");
}

void 
TransferContext::setRenderResourceDebugName(RenderResource* rdRsc, StrView name)
{
	auto* cmd = newCommand<TransferCommand_SetDebugName>(_createRdRscQueue);
	cmd->dst	= rdRsc;
	cmd->name	= name;
}

void 
TransferContext::createRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	auto* cmd = newCommand<TransferCommand_CreateRenderGpuBuffer>(_createRdRscQueue);
	cmd->dst = buffer;
}

void 
TransferContext::createTexture(Texture* texture)
{
	RDS_TODO("rework command debug SRCLOC, transfer and render also need to rework!!!");
	auto* cmd = newCommand<TransferCommand_CreateTexture>(_createRdRscQueue);
	cmd->dst = texture;
}

void 
TransferContext::destroyRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	auto* cmd = newCommand<TransferCommand_DestroyRenderGpuBuffer>(_destroyRdRscQueue);
	cmd->dst = buffer;
	RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::destroyTexture(Texture* texture)
{
	auto* cmd = newCommand<TransferCommand_DestroyTexture>(_destroyRdRscQueue);
	cmd->dst = texture;
	RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::onCreate(const CreateDesc& cDesc)
{
	{
		auto data = _createRdRscQueue.scopedULock();
		data->reset(RDS_NEW(TransferCommandBuffer));
	}
	{
		auto data = _destroyRdRscQueue.scopedULock();
		data->reset(RDS_NEW(TransferCommandBuffer));
	}
}

void 
TransferContext::onDestroy()
{

}

void 
TransferContext::onTransferBegin()
{

}

void 
TransferContext::onTransferEnd()
{

}

void 
TransferContext::onCommit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)
{

}

void 
TransferContext::createRenderResources(const RenderFrameParam& rdFrameParam)
{
	UPtr<TransferCommandBuffer> rscQueue;
	bool isSuccess = _tryPopRenderResourceQueue(rscQueue, _createRdRscQueue, rdFrameParam);
	if (isSuccess)
	{
		onCommitRenderResources(*rscQueue, true);
	}
}

void 
TransferContext::destroyRenderResources(const RenderFrameParam& rdFrameParam)
{
	UPtr<TransferCommandBuffer> rscQueue;
	bool isSuccess = _tryPopRenderResourceQueue(rscQueue, _destroyRdRscQueue, rdFrameParam);
	if (isSuccess)
	{
		onCommitRenderResources(*rscQueue, false);
	}
}

void 
TransferContext::onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate)
{

}

bool
TransferContext::_tryPopRenderResourceQueue(UPtr<TransferCommandBuffer>& out, ResourceQueue& rscQue, const RenderFrameParam& rdFrameParam)
{
	{
		auto* p = RDS_NEW(TransferCommandBuffer);
		auto data	= rscQue.scopedULock();
		out = rds::move(*data);
		data->reset(p);
	}
	return out;
}

#endif

}