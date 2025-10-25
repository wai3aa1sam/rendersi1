
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
RenderThread::makeCDesc(JobSystem* jobSystem)
{
	auto cDesc = CreateDesc{};
	cDesc.localId		= Traits::s_kRenderThreadId;
	cDesc.affinityIdx	= cDesc.localId;
	cDesc.name			= "RenderThread";
	cDesc.threadPool	= jobSystem->_internal_threadPool();
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
	RDS_TODO("currently have bug when quit program, the timing in setStaete , it is a bad design, later change to "
				"cosumer-producer pattern should fix that, State design is bad");
	// temp fix, only wait if created
	if (bool hasNotCreated = localId() != Traits::s_kRenderThreadId)
	{
		destroy();
		return;
	}

	waitTerminated();
	destroy();
}

void 
RenderThread::onDestroy()
{
	
}

void 
RenderThread::onThreadState_Terminate()
{
	UPtr<RenderJob> rdJob;
	while (_rdJobConsumerQueue.try_pop(rdJob))
	{
		render(rds::move(rdJob));
	}
	Renderer::instance()->destroy();

	setState(RenderThreadState::TerminateEnd);
}

void*
RenderThread::onRoutine()
{
	RDS_PROFILE_SCOPED();

	setState(RenderThreadState::None);
	_curFrameCount.store(0);
	_lastFinishedFrameCount.store(0);

	using State = RenderThreadState;
	#if 0
	while (_state != State::Terminated)
	{
		switch (_state)
		{
			case State::onTerminate: { ThreadState_onTerminate(); } break;
			default: { RDS_THROW("invalid thread state"); } break;
		}
	}
	#endif // 0

	for (;;)
	{
		if (_state == State::Terminate) { onThreadState_Terminate(); break; }

		UPtr<RenderJob> rdJob;
		bool hasRequestRender = _rdJobConsumerQueue.try_pop(rdJob);		// seems try_pop is poping front, if no hints could try once more
		if (hasRequestRender)
		{
			setState(RenderThreadState::Processing);
			render(rds::move(rdJob));
		}
		else
		{
			setState(RenderThreadState::Stealing);
			// as a worker thread
			JobHandle job = nullptr;
			auto* thp = threadPool();
			if (thp->trySteal(job))
			{
				RDS_PROFILE_SECTION("work as worker");
				thp->execute(job);
			}
		}
	}

	return nullptr;
}

void 
RenderThread::requestRender(UPtr<RenderJob> rdJob)
{
	//RDS_CORE_LOG_ERROR("requestRender() - renderData.frameCount: {}", renderData->frameCount);
	//RDS_CORE_ASSERT(!isTerminated(), " RenderThread has already terminated");
	_rdJobConsumerQueue.push(rds::move(rdJob));
}

void 
RenderThread::terminate()
{
	setState(RenderThreadState::Terminate);
	// quit the thread
	// wait quit
	// process the remaining
}

void 
RenderThread::waitTerminated()
{
	UPtr<RenderJob> rdJob;
	bool hasRequestRender = _rdJobConsumerQueue.try_pop(rdJob);		// seems try_pop is poping front, if no hints could try once more
	if (hasRequestRender)
	{
		render(rds::move(rdJob));
	}

	/*while (!isFrameFinished(currentFrameCount()) || _state != RenderThreadState::TerminateEnd)
	{
		OsUtil::sleep_ms(1);
	}*/
}

void
RenderThread::render(UPtr<RenderJob> renderJob)
{
	auto curFrame = renderJob->frameCount;
	//RDS_CORE_ASSERT(renderJob->frameCount == _curFrameCount + 1, "RenderThread order incorrect");
	_curFrameCount.store(curFrame);

	RDS_PROFILE_DYNAMIC_FMT("render() - frame {}", curFrame);

	auto* rdDev			= renderJob->renderDevice;
	//auto& tsfCtx	= rdDev->transferContext();

	auto* pxy_rdDev		= sCast<Proxy_RenderDevice*>(renderJob->renderDevice);
	auto* pxy_tsfCtx	= sCast<Proxy_TransferContext*>(&rdDev->transferContext());

	pxy_rdDev->reset(renderJob, pxy_tsfCtx);

	{
		pxy_tsfCtx->transferBegin();
		pxy_tsfCtx->commit(renderJob, false);

		{
			auto&	rdGraph		= renderJob->renderGraph();
			auto*	rdCtx		= rdGraph.renderContext();
			//auto	frameIndex	= rdFrameParam.frameIndex();

			rdCtx->beginRender();

			rdGraph.commit(renderJob->_renderGraphFrameIdx);
			rdCtx->commit(renderJob->renderRequest());

			rdCtx->endRender();
		}

		pxy_tsfCtx->transferEnd();
	}

	RDS_TODO(
		"curFrame is from rdDev, so if later support multi rdDev, then there maybe a map for each _lastFinishedFrameCount"
		"\n or there is multi RenderThread"
	);
	_lastFinishedFrameCount.store(curFrame);

	rdDev->_internal_freeRenderJob(rds::move(renderJob));
}

void 
RenderThread::_temp_render()
{
	UPtr<RenderJob> o;
	bool hasRequestRender = _rdJobConsumerQueue.try_pop(o);
	if (hasRequestRender)
	{
		render(rds::move(o));
	}
}

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

#endif

}
