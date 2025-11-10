#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

#include "command/rds_transfer_command.h"

#define RDS_OLD_TSF_FRAME_IMPL 0

namespace rds
{

#if 0
#pragma mark --- rdsTransferContext-Impl ---
#endif // 0
#if 1

TransferContext::CreateDesc				
TransferContext::makeCDesc()
{
	return CreateDesc{};
}

SPtr<TransferContext>	
TransferContext::make(const CreateDesc& cDesc)
{
	_notYetSupported(RDS_SRCLOC);
	return nullptr;
	//Renderer::renderDevice()->cr
}

TransferContext::TransferContext()
{

}

TransferContext::~TransferContext()
{

}

void 
TransferContext::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
TransferContext::destroy()
{
	if (!hasCreated())
		return;

	for (auto& e : _tsfFrames)
	{
		_destroyRenderResources(e);
	}
	_tsfFrames.clear();

	onDestroy();
	Base::destroy();
}

void 
TransferContext::submit(RenderJob* rdJob)
{
	auto curTsfFrameIdx		= _tsfFrameIdx.load();
	auto nextTsfFrameIdx	= sCast<u32>((curTsfFrameIdx + 1) % s_kMaxTransferFrameCount);
	_tsfFrameIdx = nextTsfFrameIdx;

	rdJob->_transferFrame = _tsfFrames[curTsfFrameIdx];
}

void 
TransferContext::waitFrameFinished(RenderFrameParam& rdFrameParam)
{
	RDS_ASSERT(false, "shd not be called");
}

void 
TransferContext::onCreate(const CreateDesc& cDesc)
{
	_tsfFrames.reserve(s_kMaxTransferFrameCount);
	for (size_t i = 0; i < s_kMaxTransferFrameCount; i++)
	{
		auto tsf_cDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
		auto frame = _tsfFrames.emplace_back(renderDevice()->createTransferFrame(tsf_cDesc));
		//frame->setDebugName("TransferFrame");
	}
}

void 
TransferContext::onDestroy()
{
	
}

TransferFrame*	TransferContext::transferFramePtr()		{ return _tsfFrames[_tsfFrameIdx]; }
TransferFrame&	TransferContext::transferFrame()		{ return *_tsfFrames[_tsfFrameIdx]; }

#endif

#if RDS_OLD_TSF_FRAME_IMPL

#if 0
private:
	TransferCommandSafeBuffer	_createRdRscQueue;
	TransferCommandSafeBuffer	_destroyRdRscQueue;

	using TransferFramePool = MutexProtected<Vector<UPtr<TransferFrame>, s_kFrameInFlightCount> >;
	TransferFramePool									_tsfFramePool;
	Vector<UPtr<TransferFrame>, s_kFrameInFlightCount>	_prevTsfFrames;
	UPtr<TransferFrame>									_curTsfFrame = nullptr;

protected:
	void releasePreviousTransferFrame();

#endif // 0

#if 0

SPtr<TransferFrame> 
TransferContext::newTransferFrame()
{
	SPtr<TransferFrame> o;
	_freeTsfFrames.try_pop(o);
	RDS_CORE_ASSERT(o, "must exist, since it call after newRenderJob()");
	o->reset();

	_tsfFrame = o;
	return _tsfFrame;
}

void 
TransferContext::_internal_freeTransferFrame(SPtr<TransferFrame>&& tsfFrame)
{
	_freeTsfFrames.push(rds::move(tsfFrame));
}
#endif // 0

void 
TransferContext::releasePreviousTransferFrame()
{
	// here is to reset the tsf frame of prev same index
	auto frameIdx = frameIndex();

	bool isFristFrame = _prevTsfFrames.is_empty(); // emplace_back to detect first cycle
	if (isFristFrame)
	{
		_prevTsfFrames.resize(s_kFrameInFlightCount);		
	}

	auto& prevTsfFrame = _prevTsfFrames[frameIdx];
	// auto& toBeRecycle = isFristFrame ? _curTsfFrame : prevTsfFrame; // useless
	auto& toBeRecycle = prevTsfFrame;

	if (toBeRecycle)
	{
		{
			auto lock = _tsfFramePool.scopedULock();
			lock->emplace_back(rds::move(toBeRecycle));
		}
	}
	else
	{
		RDS_LOG_DEBUG("_prevTsfFrames[{}] == nullptr", frameIdx);
	}

	prevTsfFrame = rds::move(_curTsfFrame);
}

UPtr<TransferFrame> 
TransferContext::allocTransferFrame()
{
	UPtr<TransferFrame> p;
	{
		auto lock = _tsfFramePool.scopedULock();

		if (lock->is_empty())		// maybe a spin lock to wait, when it is empty, better then create a new one
		{
			auto tsfFrameCDesc = TransferFrame::makeCDesc(RDS_SRCLOC);
			lock->emplace_back(renderDevice()->createTransferFrame(tsfFrameCDesc));
		}
		RDS_ASSERT(lock->size()		<= s_kFrameInFlightCount, "lock->size():	 {}, s_kFrameInFlightCount assumption is wrong, need modify", lock->size());
		RDS_ASSERT(lock->capacity() == s_kFrameInFlightCount, "lock->capacity(): {}, s_kFrameInFlightCount assumption is wrong, need modify", lock->capacity());

		auto tmp = lock->moveBack();
		tmp->reset();
		RDS_ASSERT(tmp, "_tsfFramePool has no TransferFrame");
		swap(p, tmp);
	}
	return p;
}

#endif // RDS_OLD_TSF_FRAME_IMPL

}