
#include "rds_render_api_layer-pch.h"
#include "rdsRenderThread.h"
#include "../rdsRenderDevice.h"
#include "../rdsRenderContext.h"
#include "../transfer/rdsTransferContext.h"

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
	UPtr<RenderData> rdData;
	while (_rdDataQueue.try_pop(rdData))
	{
		render(*rdData);
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

		UPtr<RenderData> rdData;
		bool hasRequestRender = _rdDataQueue.try_pop(rdData);		// seems try_pop is poping front, if no hints could try once more
		if (hasRequestRender)
		{
			setState(RenderThreadState::Processing);
			render(*rdData);
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
RenderThread::requestRender(UPtr<RenderData>&& renderData)
{
	//RDS_CORE_LOG_ERROR("requestRender() - renderData.frameCount: {}", renderData->frameCount);

	//RDS_CORE_ASSERT(!isTerminated(), " RenderThread has already terminated");
	_rdDataQueue.push(rds::move(renderData));
}

void 
RenderThread::requestTerminate()
{
	setState(RenderThreadState::Terminate);
}

void 
RenderThread::waitTerminated()
{
	while (!isFrameFinished(currentFrameCount()) || _state != RenderThreadState::TerminateEnd)
	{
		OsUtil::sleep_ms(1);
	}
}

void
RenderThread::render(RenderData& renderData)
{
	auto curFrame = renderData.frameCount;
	RDS_CORE_ASSERT(renderData.frameCount == _curFrameCount + 1, "RenderThread order incorrect");
	_curFrameCount.store(curFrame);

	RDS_PROFILE_DYNAMIC_FMT("render() - frame {}", curFrame);

	auto* rdDev			= renderData.renderDevice;
	auto& rdFrameParam	= rdDev->renderFrameParam();

	rdDev->reset(curFrame);
	// vmaSetCurrentFrameIndex();	// 

	{
		auto& tsfCtx	= rdDev->transferContext();
		//auto& tsfReq	= tsfCtx.transferRequest(Traits::rotateFrame(curFrame));
		tsfCtx.transferBegin();
		tsfCtx.commit(rdFrameParam, rds::move(renderData.transferFrame), false);

		{
			// RenderFrameContext
			// beginFrame();
			//		for ... render()
			// endFrame();		// only submit here
			for (auto& e : renderData.renderJobs)
			{
				//RDS_CORE_LOG_ERROR("render begin - curFrame: {}, e.renderGraphFrameIdx: {}", curFrame, e.renderGraphFrameIdx);

				auto&	rdGraph		= *e.renderGraph;
				auto*	rdCtx		= rdGraph.renderContext();
				//auto	frameIndex	= rdFrameParam.frameIndex();

				rdCtx->beginRender();

				rdGraph.commit(e.renderGraphFrameIdx);
				if (auto* rdReq = e.renderRequest)
				{
					rdCtx->commit(*rdReq);
				}

				rdCtx->endRender();

				//RDS_CORE_LOG_ERROR("render end - curFrame: {}, e.renderGraphFrameIdx: {}", curFrame, e.renderGraphFrameIdx);
			}
		}

		tsfCtx.transferEnd();
	}

	RDS_TODO(
		"curFrame is from rdDev, so if later support multi rdDev, then there maybe a map for each _lastFinishedFrameCount"
		"\n or there is multi RenderThread"
	);
	_lastFinishedFrameCount.store(curFrame);
}

void 
RenderThread::_temp_render()
{
	UPtr<RenderData> rdData;
	bool hasRequestRender = _rdDataQueue.try_pop(rdData);
	if (hasRequestRender)
	{
		render(*rdData);
	}
}

void 
RenderThread::setState(RenderThreadState state)
{
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
