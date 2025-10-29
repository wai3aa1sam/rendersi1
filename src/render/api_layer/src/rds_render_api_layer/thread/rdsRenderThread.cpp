
#include "rds_render_api_layer-pch.h"
#include "rdsRenderThread.h"
#include "../rdsRenderDevice.h"
#include "../rdsRenderContext.h"
#include "../transfer/rdsTransferContext.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_render_api_layer/backend/base/rdsProxy_RenderDevice.h"
#include "rds_render_api_layer/backend/base/rdsProxy_TransferContext.h"

namespace rds
{

RenderThread::CreateDesc 
RenderThread::makeCDesc(RenderDevice* rdDev, JobSystem* jobSystem)
{
	auto cDesc = CreateDesc{};
	cDesc.localId		= Traits::s_kRenderThreadId;
	cDesc.affinityIdx	= cDesc.localId;
	cDesc.name			= "RenderThread";
	cDesc.threadPool	= jobSystem->_internal_threadPool();

	cDesc.renderDevice	= rdDev;
	return cDesc;
}

#if 0
#pragma mark --- rdsRenderThread-Impl ---
#endif // 0
#if 1

RenderThread::RenderThread()
{

}

RenderThread::~RenderThread()
{
	destroy();
}

void 
RenderThread::onCreate(const CreateDesc_Base& cDescBase)
{
	Base::onCreate(cDescBase);
	_rdDev = sCast<const CreateDesc&>(cDescBase).renderDevice;
}

void 
RenderThread::onDestroy()
{
	if (!_rdDev)
		return;

	waitIdle();

	// clean all transferFrames
	for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
	{
		_rdDev->submitRenderJob(_rdDev->newRenderJob(nullptr, i));
	}
	waitIdle();

	quit();
	_rdDev = nullptr;
}

void 
RenderThread::onThreadState_Terminate()
{
	
}

void*
RenderThread::onRoutine()
{
	RDS_PROFILE_SCOPED();

	{
		auto data = _state.scopedULock();
		data->isStarted = true;
	}

	for (;;)
	{
		{
			auto data = _state.scopedULock();
			if (data->isQuit)
			{
				// onThreadState_Terminate();
				return nullptr;
			}
		}

		bool hasRender = tryRender();
		if (!hasRender) tryExecuteStealJob();
	}

	return nullptr;
}

void 
RenderThread::requestRender(UPtr<RenderJob> rdJob)
{
	_pendingRdJobs.push(rds::move(rdJob));
}

void 
RenderThread::quit()
{
	// quit the thread
	// wait quit
	// process the remaining
	{
		auto data = _state.scopedULock();
		data->isQuit = true;
	}
}

bool 
RenderThread::tryRender()
{
	UPtr<RenderJob> rdJob;
	bool hasPending = _pendingRdJobs.try_pop(rdJob);		// seems try_pop is poping front, if no hints could try once more
	if (rdJob)
	{
		render(rds::move(rdJob));
	}
	return hasPending;
}

void
RenderThread::render(UPtr<RenderJob> renderJob)
{
	_rdDev = renderJob->renderDevice;

	RDS_TODO("store a last semaphore and chain each time when new job, it is for multiple RenderContext");

	//auto curFrame = renderJob->frameCount;
	//RDS_CORE_ASSERT(renderJob->frameCount == _curFrameCount + 1, "RenderThread order incorrect");
	//_curFrameCount.store(curFrame);
	//RDS_PROFILE_DYNAMIC_FMT("render() - frame {}", curFrame);
	RDS_PROFILE_SECTION("render()");

	//auto* renderJob = renderJob_.ptr();
	//_processingRdJobs.push(rds::move(renderJob_));

	auto* rdDev			= renderJob->renderDevice;
	//auto& tsfCtx	= rdDev->transferContext();

	auto* pxy_rdDev		= sCast<Proxy_RenderDevice*>(renderJob->renderDevice);
	auto* pxy_tsfCtx	= sCast<Proxy_TransferContext*>(&rdDev->transferContext());

	pxy_rdDev->reset(renderJob, pxy_tsfCtx);

	{
		pxy_tsfCtx->transferBegin();
		pxy_tsfCtx->commit(renderJob, false);

		if (renderJob->renderRequest().renderContext())
		{
			auto*	rdCtx		= renderJob->renderGraph().renderContext();
			rdCtx->Render_reset(renderJob);
			rdCtx->beginRender();
			rdCtx->commit();
			rdCtx->endRender();
		}

		pxy_tsfCtx->transferEnd();
	}

	//_lastFinishedFrameCount.store(curFrame);
	rdDev->_internal_freeRenderJob(rds::move(renderJob));
}

bool 
RenderThread::tryExecuteStealJob()
{
	JobHandle job = nullptr;
	auto* thp = threadPool();
	if (thp->trySteal(job))
	{
		RDS_PROFILE_SECTION("work as worker");
		thp->execute(job);
	}
	return job;
}

bool 
RenderThread::hasPendingRenderJobs()
{
	return _pendingRdJobs.isEmpty();
}

void
RenderThread::waitIdle()
{
	waitCpuIdle();
	waitGpuIdle();
}

void
RenderThread::waitCpuIdle()
{
	_rdDev->waitCpuIdle();
}

void
RenderThread::waitGpuIdle()
{
	RDS_TODO("later should put imageAvaliableSmp to Vk_Swapchain, renderCompletedSmp to RenderJob_Vk, no longer in RenderContext");
	RDS_TODO("all semaphore in RenderContext need to separate FrameAHead and FrameInflight");
	RDS_TODO("check fence in _processingRdJobs");
	
	waitCpuIdle();
	_rdDev->_internal_waitGpuIdle();
}

#if 0
void 
RenderThread::setState(RenderThreadState state)
{
	RDS_TODO("quick fix for close app crash, later change to consumer/producer pattern should fix this");
	if (_state.load() == RenderThreadState::Terminate)
	{
		if (state != RenderThreadState::TerminateEnd)
			return;
	}
	_state.store(state);
}

bool	RenderThread::isState(RenderThreadState state)	const { return _state.load() == state; }
bool	RenderThread::isTerminated()					const { return isState(RenderThreadState::TerminateEnd); }
bool	RenderThread::isReadyToProcess()				const { return isState(RenderThreadState::Idle) || isState(RenderThreadState::Stealing); }
bool	RenderThread::isIdle()							const { return isState(RenderThreadState::Idle); }


bool	RenderThread::isFrameFinished(u64 frame)	const	{ auto n = lastFinishedFrameCount(); return n >= frame; }

u64		RenderThread::currentFrameCount()			const	{ return _curFrameCount.load(); } //{ auto n = lastFinishedFrameCount(); return n <= 1 && isFrameFinished(n) ? n : n - 1; }
u64		RenderThread::lastFinishedFrameCount()		const	{ return _lastFinishedFrameCount.load(); }

#endif // 0

#endif

}
