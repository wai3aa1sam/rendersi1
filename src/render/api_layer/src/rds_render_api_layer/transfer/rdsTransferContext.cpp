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

	_prevTsfFrames.clear();
	{
		auto data = _tsfFramePool.scopedULock();
		data->clear();
	}

	destroyRenderResources(renderFrameParam());

	onDestroy();
	Base::destroy();
}

void 
TransferContext::transferBegin()
{
	RDS_PROFILE_SCOPED();
	createRenderResources(renderFrameParam());		// should pass from fn
	onTransferBegin();
}

void TransferContext::transferEnd()
{
	RDS_PROFILE_SCOPED();
	onTransferEnd();
	releasePreviousTransferFrame();
	destroyRenderResources(renderFrameParam());		// should pass from fn
}
void 
TransferContext::commit(RenderFrameParam& rdFrameParam, UPtr<TransferFrame>&& tsfFrame_, bool isWaitImmediate)
{
	RDS_PROFILE_SCOPED();

	_curTsfFrame = rds::move(tsfFrame_);

	auto* rdDev	= renderDevice();
	auto& tsfFrame = _curTsfFrame;
	onCommit(rdFrameParam, tsfFrame->transferRequest(), isWaitImmediate);
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
	RDS_TODO("rework command data member for debug SRCLOC, transfer and render also need to rework!!!");
	auto* cmd = newCommand<TransferCommand_CreateTexture>(_createRdRscQueue);
	cmd->dst = texture;
}

void 
TransferContext::destroyRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	OsUtil::sleep_ms(1);

	auto* cmd = newCommand<TransferCommand_DestroyRenderGpuBuffer>(_destroyRdRscQueue);
	cmd->dst = buffer;
	RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::destroyTexture(Texture* texture)
{
	OsUtil::sleep_ms(1);

	auto* cmd = newCommand<TransferCommand_DestroyTexture>(_destroyRdRscQueue);
	cmd->dst = texture;
	//RDS_CORE_ASSERT(cmd->dst->isRefCount0(), "only call when refCount is 0");
}

void 
TransferContext::onCreate(const CreateDesc& cDesc)
{
	#if 0
	{
		auto lock = _tsfFramePool.scopedULock();
		auto& data = *lock;
		data.resize(s_kFrameSafeInFlightCount);
		for (auto& e : data)
		{
			auto tsfFrameCDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
			e = renderDevice()->createTransferFrame(tsfFrameCDesc);
		}
	}
	#endif // 0


	#if 0
	{
		auto data = _createRdRscQueue.scopedULock();
		data->reset(RDS_NEW(TransferCommandBuffer));
	}
	{
		auto data = _destroyRdRscQueue.scopedULock();
		data->reset(RDS_NEW(TransferCommandBuffer));
	}
	#endif // 0
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
	TransferCommandBuffer rscQueue;
	bool isSuccess = TransferRequest::tryPopTransferCommandSafeBuffer(rscQueue, _createRdRscQueue);
	if (isSuccess)
	{
		onCommitRenderResources(rscQueue, true);
	}
}

void 
TransferContext::destroyRenderResources(const RenderFrameParam& rdFrameParam)
{
	//OsUtil::sleep_ms(1);

	TransferCommandBuffer rscQueue;
	bool isSuccess = TransferRequest::tryPopTransferCommandSafeBuffer(rscQueue, _destroyRdRscQueue);
	if (isSuccess)
	{
		onCommitRenderResources(rscQueue, false);
	}
}

void 
TransferContext::onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate)
{

}

void 
TransferContext::releasePreviousTransferFrame()
{
	// here is to reset the tsf frame of prev same index
	auto frameIdx = frameIndex();
	
	bool isFristFrame = _prevTsfFrames.is_empty(); // emplace_back to detect first cycle
	if (isFristFrame)
	{
		_prevTsfFrames.resize(s_kFrameInFlightCount);		
	}

	auto& prevTsfFrame = _prevTsfFrames[frameIdx];
	// auto& toBeRecycle = isFristFrame ? _curTsfFrame : prevTsfFrame; // useless
	auto& toBeRecycle = prevTsfFrame;

	if (toBeRecycle)
	{
		toBeRecycle->reset();
		{
			auto lock = _tsfFramePool.scopedULock();
			lock->emplace_back(rds::move(toBeRecycle));
		}
	}
	else
	{
		RDS_LOG_DEBUG("_prevTsfFrames[{}] == nullptr", frameIdx);
	}

	prevTsfFrame = rds::move(_curTsfFrame);
}

UPtr<TransferFrame> 
TransferContext::allocTransferFrame()
{
	UPtr<TransferFrame> p;
	{
		auto lock = _tsfFramePool.scopedULock();

		if (lock->is_empty())		// maybe a spin lock to wait, when it is empty, better then create a new one
		{
			auto tsfFrameCDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
			lock->emplace_back(renderDevice()->createTransferFrame(tsfFrameCDesc));
		}
		RDS_ASSERT(lock->size()		<= s_kFrameInFlightCount, "lock->size():	 {}, s_kFrameInFlightCount assumption is wrong, need modify", lock->size());
		RDS_ASSERT(lock->capacity() == s_kFrameInFlightCount, "lock->capacity(): {}, s_kFrameInFlightCount assumption is wrong, need modify", lock->capacity());

		auto tmp = lock->moveBack();
		RDS_ASSERT(tmp, "_tsfFramePool has no TransferFrame");
		swap(p, tmp);
	}
	return p;
}

TransferFrame& TransferContext::transferFrame() { return *_curTsfFrame; }

#endif

}