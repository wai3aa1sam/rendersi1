#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "nmsp_job_system/thread/nmspTypeThread.h"

#include "rdsRenderThreadQueue.h"

namespace rds
{

class RenderDevice;

using TypeThread_CreateDesc = ::nmsp::TypeThread_CreateDesc;
using TypeThread			= ::nmsp::TypeThread_T;

struct RenderThread_CreateDesc : public ::nmsp::TypeThread_CreateDesc
{
	RenderDevice* renderDevice = nullptr;
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

class RenderThread : public TypeThread
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base				= TypeThread;
	using CreateDesc		= RenderThread_CreateDesc;

public:
	static CreateDesc makeCDesc(RenderDevice* rdDev, JobSystem* jobSystem);

public:
	RenderThread();
	~RenderThread();

public:
	UPtr<RenderJob> newRenderJob(RenderDevice* renderDevice, u64 frameCount);

public:
	void requestRender(UPtr<RenderJob> renderJob);
	void quit();

public:
	bool tryRender();
	bool tryExecuteStealJob();

	void render(UPtr<RenderJob> renderJob);

public:
	bool hasPendingRenderJobs();

	void waitIdle();
	void waitCpuIdle();
	void waitGpuIdle();

protected:
	virtual void onCreate(const CreateDesc_Base& cDescBase) override;
	virtual void onDestroy() override;
	virtual void* onRoutine() override;

	virtual void onThreadState_Terminate();

private:
	RenderDevice* _rdDev = nullptr;

	//Atm<RenderThreadState>		_state = RenderThreadState::None;
	struct State
	{
		bool isStarted	= false;
		bool isQuit		= false;
	};
	MutexProtected<State>		_state;		// TODO: CondMutexProtected
	AtmQueue<UPtr<RenderJob> >	_pendingRdJobs;
	//AtmQueue<UPtr<RenderJob> >	_processingRdJobs;// this is for check the gpu side is completed or not

	//RenderThreadQueue			_rdThreadQueue;		// if, use other name, maybe like Dx12 called Engine as an interface for RenderThread
};

#endif

}


