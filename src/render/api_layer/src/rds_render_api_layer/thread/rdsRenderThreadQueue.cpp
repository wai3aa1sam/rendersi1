#include "rds_render_api_layer-pch.h"
#include "rdsRenderThreadQueue.h"
#include "rdsRenderThread.h"
#include "rds_render_api_layer/rdsRenderContext.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderThreadQueue-Impl ---
#endif // 0
#if 0

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


	{
		//auto lock = _rdFramePool.scopedULock();
		for (size_t i = 0; i < RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit; i++)
		{
			_rdJobProducerQueue.push(makeUPtr<RenderJob>());
		}
	}
}

void 
RenderThreadQueue::destroy()
{
	if (_rdThread)
	{
		_rdThread->terminate();
	}
	_rdThread = nullptr;
}

UPtr<RenderJob> 
RenderThreadQueue::newRenderJob(u64 frameCount)
{
	UPtr<RenderJob> o;
	if (_rdJobProducerQueue.try_pop(o))
	{
		o->frameCount = frameCount;
	}
	return o;
}

void RenderThreadQueue::submit(UPtr<RenderJob> renderJob)
{
	RDS_CORE_ASSERT(false);
}

void
RenderThreadQueue::submit(RenderDevice* renderDevice, u64 frameCount, RenderJob&& renderJob)
{
	RDS_CORE_ASSERT(false);
}

#if 0
void 
RenderThreadQueue::submit(RenderDevice* renderDevice, u64 frameCount, RenderJob&& renderJob)
{
	auto rdData = makeUPtr<RenderData>();
	rdData->frameCount		= frameCount;
	rdData->renderDevice	= renderDevice;
	rdData->transferFrame	= renderDevice->releaseTransferFrame();
	{
		auto& rdJob = rdData->renderJobs.emplace_back();
		rdJob = rds::move(renderJob);
	}

	_rdThread->requestRender(rds::move(rdData));
}
#endif // 0

void 
RenderThreadQueue::waitFrame(u64 frameCount, int sleepMs)
{
	while (!isSignaled(frameCount))
	{
		OsUtil::sleep_ms(sleepMs);
	}
}

u64 RenderThreadQueue::currentFrameCount()		const { return _rdThread->currentFrameCount(); }

bool
RenderThreadQueue::isSignaled(u64 engineFrameCount) const
{
	auto rdThreadCurrentFrameCount = currentFrameCount();
	bool isSameFrameIndex	= RenderApiLayerTraits::rotateFrame(engineFrameCount) == RenderApiLayerTraits::rotateFrame(rdThreadCurrentFrameCount);
	bool isLeadingRender	= engineFrameCount >= rdThreadCurrentFrameCount + RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit;

	bool shdWait = isLeadingRender || isSameFrameIndex; // && !_rdThread->isFrameFinished(engineFrameCount));	// not work for RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit == 1
	return !shdWait || _rdThread->isFrameFinished(engineFrameCount);
	//  (|| _rdThread->isReadyToProcess()) is wrong, since the RenderThread could have job on queue but not pop yet
}

bool 
RenderThreadQueue::isFinished(u64 engineFrameCount) const
{
	return _rdThread->isFrameFinished(engineFrameCount);
}

UPtr<RenderJob> RenderThreadQueue::consumeRenderJob()
{
	return UPtr<RenderJob>();
}

void RenderThreadQueue::freeRenderJob(UPtr<RenderJob> renderJob)
{
}

#endif

}