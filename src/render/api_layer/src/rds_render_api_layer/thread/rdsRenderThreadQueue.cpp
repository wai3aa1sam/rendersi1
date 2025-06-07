#include "rds_render_api_layer-pch.h"
#include "rdsRenderThreadQueue.h"
#include "rdsRenderThread.h"
#include "../graph/rdsRenderGraph.h"

namespace rds
{

void 
RenderJob::create(RenderGraph* renderGraph_, RenderRequest* renderReq)
{
	renderGraph			= renderGraph_;
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
	
	{
		auto lock = _rdFramePool.scopedULock();
		lock->resize(RenderApiLayerTraits::s_kFrameSafeInFlightCount);
		for (size_t i = 0; i < RenderApiLayerTraits::s_kFrameSafeInFlightCount; i++)
		{
			lock->at(i) = makeUPtr<RenderJob>();
			if (i < RenderApiLayerTraits::s_kFrameInFlightCount)
			{
				_rdJobProducerQueue.push(lock->moveFront());
			}
			//lock->pop_front_unsorted()
		}
	}
}

void 
RenderThreadQueue::destroy()
{
	{
		auto lock = _rdFramePool.scopedULock();
		lock->clear();
	}

	if (_rdThread)
	{
		_rdThread->requestTerminate();
	}
	_rdThread = nullptr;

}

UPtr<RenderJob> 
RenderThreadQueue::newRenderJob(RenderDevice* renderDevice, u64 frameCount)
{
	UPtr<RenderJob> o;
	_rdJobProducerQueue.try_pop(o);

	if (o)
	{
		o->renderDevice = renderDevice;
		o->frameCount	= frameCount;

		if (!o->transferFrame)
		{
			auto tsfFrameCDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
			o->transferFrame = renderDevice->createTransferFrame(tsfFrameCDesc);
		}
		o->transferFrame->reset();

		RDS_CORE_ASSERT(renderDevice == o->transferFrame->renderDevice());
	}
	return o;
}

void 
RenderThreadQueue::submit(UPtr<RenderJob> rdJob)
{
	_rdJobConsumerQueue.push(rds::move(rdJob));
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
RenderThreadQueue::isSignaled() const
{
	return !this->_rdJobProducerQueue.isEmpty();
}

bool
RenderThreadQueue::isSignaled(u64 engineFrameCount) const
{
	auto rdThreadCurrentFrameCount = currentFrameCount();
	bool isSameFrameIndex	= RenderApiLayerTraits::rotateFrame(engineFrameCount) == RenderApiLayerTraits::rotateFrame(rdThreadCurrentFrameCount);
	bool isLeadingRender	= engineFrameCount >= rdThreadCurrentFrameCount + RenderApiLayerTraits::s_kFrameInFlightCount;

	bool shdWait = isLeadingRender || isSameFrameIndex; // && !_rdThread->isFrameFinished(engineFrameCount));	// not work for RenderApiLayerTraits::s_kFrameInFlightCount == 1
	return !shdWait || _rdThread->isFrameFinished(engineFrameCount);
	//  (|| _rdThread->isReadyToProcess()) is wrong, since the RenderThread could have job on queue but not pop yet
}

UPtr<RenderJob> 
RenderThreadQueue::consumeRenderJob()
{
	UPtr<RenderJob> o;
	_rdJobConsumerQueue.try_pop(o);
	return o;
}

void
RenderThreadQueue::freeRenderJob(UPtr<RenderJob> renderJob)
{
	UPtr<RenderJob> o;
	{
		auto lock = _rdFramePool.scopedULock();
		o = lock->moveFront();
		lock->emplace_back(rds::move(renderJob));
	}

	RDS_CORE_ASSERT(o);
	_rdJobProducerQueue.push(o);
}

#endif

}