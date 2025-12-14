#include "rds_render_api_layer-pch.h"
#include "rdsProxy_TransferContext.h"

#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

namespace rds
{

#if 0
#pragma mark --- rdsProxy_TransferContext-Impl ---
#endif // 0
#if 1

void 
Proxy_TransferContext::reset(SPtr<TransferFrame>& tsfFrame, RenderJob* rdJob)
{
	_tsfFrame	= tsfFrame;
	_frameCount = rdJob->frameCount;
}

void
Proxy_TransferContext::transferBegin()
{
	RDS_PROFILE_SCOPED();
	createRenderResources();		// should pass from fn
	onTransferBegin();
}

void Proxy_TransferContext::transferEnd()
{
	RDS_PROFILE_SCOPED();
	onTransferEnd();
	//releasePreviousTransferFrame();
	destroyRenderResources();		// should pass from fn
	_tsfFrame = nullptr;
}
void 
Proxy_TransferContext::commit(RenderJob* rdJob, bool isWaitImmediate)
{
	RDS_PROFILE_SCOPED();

	auto* rdDev	= rdJob->renderDevice;
	onCommit(rdJob, isWaitImmediate);
	rdDev->bindlessResource().commit();
}

void Proxy_TransferContext::onTransferBegin()
{
}

void Proxy_TransferContext::onTransferEnd()
{
	
}

void Proxy_TransferContext::onCommit(RenderJob* rdJob, bool isWaitImmediate)
{
}

void 
Proxy_TransferContext::onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate)
{

}

void 
Proxy_TransferContext::_onCommitRenderResources(TransferFrame* frame, TransferCommandSafeBuffer& rscQueue)
{
	if (frame)
	{
		auto lock = rscQueue.scopedLock();
		onCommitRenderResources(*lock.data(), true);
	}
}

void 
Proxy_TransferContext::createRenderResources()
{
	_onCommitRenderResources(_tsfFrame, _tsfFrame->renderResourceBuffer_create());
	_onCommitRenderResources(_tsfFrame, _tsfFrame->renderResourceBuffer_setDebugLabel());
}

void 
Proxy_TransferContext::destroyRenderResources()
{
	_onCommitRenderResources(_tsfFrame, _tsfFrame->renderResourceBuffer_destroy());
}

void 
Proxy_TransferContext::_temp_destroyRenderResources(TransferFrame* frame)
{
	_onCommitRenderResources(frame, frame->renderResourceBuffer_destroy());
}

void 
Proxy_TransferContext::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Proxy_TransferContext::onDestroy()
{
	destroyRenderResources();

	Base::onDestroy();
}

#endif

}