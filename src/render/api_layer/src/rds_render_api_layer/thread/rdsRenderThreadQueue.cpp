#include "rds_render_api_layer-pch.h"
#include "rdsRenderThreadQueue.h"
#include "rdsRenderThread.h"

namespace rds
{

void 
RenderData_RenderJob::create(RenderGraph* renderGraph_, RenderRequest* renderReq)
{
	renderGraph			= renderGraph_;
	renderRequest		= renderReq;
	renderGraphFrameIdx = renderGraph->frameIndex();

	//throwIf(true, "need to modify RenderGraph, compile to rotateFrame() [if retain mode then rotateFrame in execute()]; and iFrame() -> frameIndex()");
}

#if 0
#pragma mark --- rdsRenderThreadQueue-Impl ---
#endif // 0
#if 1

RenderThreadQueue::RenderThreadQueue()
{

}

RenderThreadQueue::~RenderThreadQueue()
{
	destroy();
}

void 
RenderThreadQueue::create(RenderThread* renderThread)
{
	destroy();

	_rdThread = renderThread;
}

void 
RenderThreadQueue::destroy()
{
	if (_rdThread)
	{
		waitFrame(_rdThread->currentFrameCount());
	}
	_rdThread = nullptr;
}

void 
RenderThreadQueue::submit(RenderDevice* renderDevice, u64 frameCount, RenderJob&& renderJob)
{
	auto rdData = makeUPtr<RenderData>();
	rdData->frameCount		= frameCount;
	rdData->renderDevice	= renderDevice;
	{
		auto& rdJob = rdData->renderJobs.emplace_back();
		rdJob = rds::move(renderJob);
	}

	_rdThread->requestRender(rds::move(rdData));
}

void 
RenderThreadQueue::waitFrame(u64 frameCount)
{
	while (!isSignaled(frameCount))
	{
		OsUtil::sleep_ms(1);
	}
}

u64 RenderThreadQueue::currentFrameCount()		const { return _rdThread->currentFrameCount(); }

bool
RenderThreadQueue::isSignaled(u64 engineFrameCount) const
{
	auto rdThreadCurrentFrameCount = currentFrameCount();
	bool isSameFrameIndex = RenderApiLayerTraits::rotateFrame(engineFrameCount) == RenderApiLayerTraits::rotateFrame(rdThreadCurrentFrameCount);
	bool shdWait = (isSameFrameIndex && !_rdThread->isFrameFinished(engineFrameCount));	// not work for RenderApiLayerTraits::s_kFrameInFlightCount == 1
	return !shdWait;
}

#endif

}