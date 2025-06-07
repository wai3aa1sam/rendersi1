#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "nmsp_job_system/thread/nmspTypeThread.h"

#include "rdsRenderThreadQueue.h"

namespace rds
{

class RenderDevice;

struct RenderThread_CreateDesc : public ::nmsp::TypeThread_CreateDesc
{

};

#define RenderThreadState_ENUM_LIST(E) \
	E(None, = 0) \
	E(Terminate, ) \
	E(TerminateEnd, ) \
	E(Idle, ) \
	E(Processing, ) \
	E(Stealing, ) \
	E(_kCount, ) \
//---
RDS_ENUM_CLASS(RenderThreadState, u8);

#if 0
#pragma mark --- rdsRenderThread-Decl ---
#endif // 0
#if 1

class RenderThread : public nmsp::TypeThread_T
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using CreateDesc		= RenderThread_CreateDesc;
	using CreateDesc_Base	= ::nmsp::TypeThread_CreateDesc;

public:
	static CreateDesc makeCDesc(JobSystem* jobSystem);

public:
	RenderThread();
	~RenderThread();

public:
	UPtr<RenderJob> newRenderJob(RenderDevice* renderDevice, u64 frameCount);
	void requestRender(UPtr<RenderJob> renderJob);

	//void requestRender(UPtr<RenderData> renderData);
	void requestTerminate();

public:
	void waitSignaled();
	void waitTerminated();

public:
	bool	isTerminated()				const;
	bool	isReadyToProcess()			const;
	bool	isIdle()					const;
	bool	isSignaled()				const;
	bool	isFrameFinished(u64 frame)	const;
	u64		currentFrameCount()			const;
	u64		lastFinishedFrameCount()	const;

protected:
	virtual void onDestroy();
	virtual void onThreadState_Terminate();


	virtual void* onRoutine() override;
	//void render(RenderData& renderData);
	void render(UPtr<RenderJob> renderJob);

public:		// TODO: remove temp
	void _temp_render();

protected:
	void setState(RenderThreadState state);
	bool isState(RenderThreadState state) const;

private:
	Atm<RenderThreadState>		_state = RenderThreadState::None;
	Atm<u64>					_curFrameCount = 0;
	Atm<u64>					_lastFinishedFrameCount = 0;

	RenderThreadQueue			_rdThreadQueue;
};

#endif

}


