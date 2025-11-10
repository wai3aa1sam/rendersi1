#include "rds_render_api_layer-pch.h"
#include "rdsRenderThread.h"
#include "../rdsRenderDevice.h"
#include "../rdsRenderContext.h"
#include "../transfer/rdsTransferContext.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

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

	#if 0
	// clean all transferFrames
	// some destroy command may miss if the Render_destroy will spwan other destroy command, 
	// maybe check all the destroyBuf only quit when no destroy commands
	constexpr size_t tryCount = 10;
	for (size_t j = 0; j < tryCount; j++)
	{
		for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
		{
			_rdDev->submitRenderJob(_rdDev->newRenderJob(nullptr, i));
		}
		waitIdle();
	}
	#endif // 0
	
	quit();
	_rdDev = nullptr;
}

void 
RenderThread::onThreadState_Terminate()
{
	
}

bool 
RenderThread::_checkUploadCompletedJob()
{
	UPtr<RenderJob> o = nullptr;
	while (_processingRdJobs.try_pop(o))
	{
		if (!o->isDoneUploading())		// we can start new job when Upload is completed, we will check again when render
		{
			_processingRdJobs.push(rds::move(o));
			return false;
		}
		_rdDev->_internal_freeRenderJob(rds::move(o));
	}
	return true;
}

bool 
RenderThread::_checkRenderCompletedJob()
{
	UPtr<RenderJob> o = nullptr;
	while (_processingRdJobs.try_pop(o))
	{
		if (!o->isDoneRendering())		// we can start new job when Upload is completed, we will check again when render
		{
			_processingRdJobs.push(rds::move(o));
			return false;
		}
		_rdDev->_internal_freeRenderJob(rds::move(o));
	}
	return true;
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
		_checkUploadCompletedJob();

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
	waitIdle();

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
	_processingRdJobs.push(rds::move(renderJob));	// ensure only 1 thread is accessing
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
RenderThread::isQuit()
{
	{
		auto data = _state.scopedULock();
		return data->isQuit;
	}
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
	{
		auto data = _state.scopedULock();
		if (data->isQuit)
			return;
	}
	if (_rdDev)
		_rdDev->waitCpuIdle();
}

void
RenderThread::waitGpuIdle()
{
	//RDS_TODO("later should put imageAvaliableSmp to Vk_Swapchain, renderCompletedSmp to RenderJob_Vk, no longer in RenderContext");
	//RDS_TODO("all semaphore in RenderContext need to separate FrameAHead and FrameInflight");
	//RDS_TODO("check fence in _processingRdJobs");
	
	while (!_checkRenderCompletedJob() || !_processingRdJobs.isEmpty());
	
	//_rdDev->_internal_waitGpuIdle();
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

#if 0


static void s()
{
	eastl::slist<int> q;
	q.pop_front();

	q.pop();
	q.push()
}

class SList : public eastl::slist<int>
{
public:
	using T = int;
public:
	void insert(T* v) { push_front(v); }
};

template<class T>
class CondQueue : public NonCopyable
{
public:
	struct MData 
	{
		u32				maxSize = NumLimit<u32>::max();
		eastl::queue<T> queue; // cannot use DList, because DListNode::removeFromList() may cause race condition
	};

public:
	void	clear() {
		auto md = _mdata.scopedULock();
		md->list.clear();
	}

	void	insert	(T* p)	{
		auto md = _mdata.scopedULock();
		while (md->list.size() >= md->maxSize) {
			md.wait();
		}
		md->list.insert(p); 
	}

	void	append	(T* p)	{
		auto md = _mdata.scopedULock();
		while (md->list.size() >= md->maxSize) {
			md.wait();
		}
		md->list.append(p);
	}

	void	insert	(UPtr<T> p)	{ insert(p.ptr()); p.detach(); }
	void	append	(UPtr<T> p)	{ append(p.ptr()); p.detach(); }

	void	setMaxSize(u32 n) { _mdata.scopedULock()->maxSize = n; }

	UPtr<T>	popHead	()	{ return _mdata.scopedULock()->list.pop_front(); }
	UPtr<T>	popTail	()	{ return _mdata.scopedULock()->list.popTail(); }

	UPtr<T>	waitHead() {
		auto md = _mdata.scopedULock();
		for(;;) {
			auto p = md->list.popHead();
			if (p) return p;
			//md.wait();
		}
	}

	UPtr<T>	timedWaitHead(int milliseconds) {
		auto md = _mdata.scopedULock();
		for(;;) {
			auto p = md->list.popHead();
			if (p) return p;
			/*if (!md.timedWait(milliseconds))
			return nullptr;*/
		}
	}

	u32 size() {
		auto md = _mdata.scopedULock();
		return md->list.size();
	}

private:
	MutexProtected<MData> _mdata;
};


#endif // 0


}

