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

	commitRenderResources(renderFrameParam());

	onDestroy();
	Base::destroy();
}

void TransferContext::transferBegin()
{
	commitRenderResources(renderFrameParam());		// should pass from fn
	onTransferBegin();
}

void TransferContext::transferEnd()
{
	onTransferEnd();
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
	auto* cmd = newCommand<TransferCommand_SetDebugName>(_rdRscCreateQueue);
	cmd->dst	= rdRsc;
	cmd->name	= name;
}

void 
TransferContext::createRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	auto* cmd = newCommand<TransferCommand_CreateRenderGpuBuffer>(_rdRscCreateQueue);
	cmd->dst = buffer;
}

void 
TransferContext::createTexture(Texture* texture)
{
	RDS_TODO("rework command debug SRCLOC, transfer and render also need to rework!!!");
	auto* cmd = newCommand<TransferCommand_CreateTexture>(_rdRscCreateQueue);
	cmd->dst = texture;
}

void 
TransferContext::destroyRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	auto* cmd = newCommand<TransferCommand_DestroyRenderGpuBuffer>(_rdRscDestroyQueue);
	cmd->dst = buffer;
	RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::destroyTexture(Texture* texture)
{
	auto* cmd = newCommand<TransferCommand_DestroyTexture>(_rdRscDestroyQueue);
	cmd->dst = texture;
	RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::onCreate(const CreateDesc& cDesc)
{
	{
		auto data = _rdRscCreateQueue.scopedULock();
		data->reset(RDS_NEW(TransferCommandBuffer));
	}
	{
		auto data = _rdRscDestroyQueue.scopedULock();
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
TransferContext::commitRenderResources(const RenderFrameParam& rdFrameParam)
{
	UPtr<TransferCommandBuffer> createQueue;
	UPtr<TransferCommandBuffer> destroyQueue;
	{
		{
			auto data	= _rdRscCreateQueue.scopedULock();
			createQueue = rds::move(*data);
			data->reset(RDS_NEW(TransferCommandBuffer));
		}

		{
			auto data		= _rdRscDestroyQueue.scopedULock();
			destroyQueue	= rds::move(*data);
			data->reset(RDS_NEW(TransferCommandBuffer));
		}
	}
	onCommitRenderResources(*createQueue, *destroyQueue);
}

void 
TransferContext::onCommitRenderResources(TransferCommandBuffer& createQueue, TransferCommandBuffer& destroyQueue)
{

}


#endif

}