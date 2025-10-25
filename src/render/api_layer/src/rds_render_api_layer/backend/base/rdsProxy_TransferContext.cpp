#include "rds_render_api_layer-pch.h"
#include "rdsProxy_TransferContext.h"
#include "rds_render_api_layer/thread/rdsRenderThreadQueue.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsProxy_TransferContext-Impl ---
#endif // 0
#if 1

void 
Proxy_TransferContext::reset(SPtr<TransferFrame>& tsfFrame)
{
	_tsfFrame = tsfFrame;
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
Proxy_TransferContext::createRenderResources()
{
	auto& tsfFrame = _tsfFrame;
	auto lock = tsfFrame->createRenderResourceBuffer().scopedULock();
	onCommitRenderResources(*lock, true);
}

void 
Proxy_TransferContext::destroyRenderResources()
{
	auto& tsfFrame = _tsfFrame;
	auto lock = tsfFrame->destroyRenderResourceBuffer().scopedULock();
	onCommitRenderResources(*lock, true);
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