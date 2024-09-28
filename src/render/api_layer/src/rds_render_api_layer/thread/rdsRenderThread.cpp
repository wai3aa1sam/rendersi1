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
	terminate();
	destroy();
}

void* 
RenderThread::onRoutine()
{
	RDS_PROFILE_SCOPED();

	_isTerminated.store(false);
	_lastFinishedFrameCount.store(0);

	for (;;)
	{
		if (isTerminated())
		{
			break;
		}

		UPtr<RenderData> rdData;
		bool hasRequestRender = _rdDataQueue.try_pop(rdData);
		if (hasRequestRender)
		{
			render(*rdData);
		}
		else
		{
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
RenderThread::render(RenderData& renderData)
{
	RDS_PROFILE_DYNAMIC_FMT("render() - frame {}", renderData.frameCount);

	auto* rdDev			= renderData.renderDevice;
	auto& rdFrameParam	= rdDev->renderFrameParam();
	rdFrameParam.reset(renderData.frameCount);

	auto& tsfCtx	= rdDev->transferContext();
	auto& tsfReq	= tsfCtx.transferRequest(Traits::rotateFrame(renderData.frameCount));
	tsfReq.commit(rdFrameParam);

	// RenderFrameContext
	// beginFrame();
	//		for ... render()
	// endFrame();		// only submit here
	for (auto& e : renderData.renderJobs)
	{
		//RDS_CORE_LOG_ERROR("render begin - renderData.frameCount: {}, e.renderGraphFrameIdx: {}", renderData.frameCount, e.renderGraphFrameIdx);

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

		//RDS_CORE_LOG_ERROR("render end - renderData.frameCount: {}, e.renderGraphFrameIdx: {}", renderData.frameCount, e.renderGraphFrameIdx);
	}

	_lastFinishedFrameCount.store(renderData.frameCount);
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
RenderThread::terminate()
{
	_isTerminated.store(true);
}

bool	RenderThread::isTerminated()			const	{ return _isTerminated; }

u64		RenderThread::lastFinishedFrameCount()	const	{ return _lastFinishedFrameCount.load(); }

#endif

}