#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class RenderJob;
class RenderDevice;


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

class RenderThread : public TypeThread, public RenderApiLayerCommon_Base
{
public:
	using Base				= TypeThread;
	using CreateDesc		= RenderThread_CreateDesc;

public:
	static CreateDesc makeCDesc(RenderDevice* rdDev, JobSystem* jobSystem);

public:
	RenderThread();
	~RenderThread();

public:
	void requestRender(UPtr<RenderJob> renderJob);
	void quit();

public:
	bool tryRender();
	bool tryExecuteStealJob();

	void render(UPtr<RenderJob> renderJob);

public:
	bool isQuit();
	bool isStarted();

	void waitIdle();
	void waitCpuIdle();
	void waitGpuIdle();

protected:
	virtual void onCreate(const CreateDesc_Base& cDescBase) override;
	virtual void onDestroy() override;
	virtual void* onRoutine() override;

	virtual void onThreadState_Terminate();

public:
	bool _checkUploadCompletedJob();
	bool _checkRenderCompletedJob();

private:
	RenderDevice* _rdDev = nullptr;

	//Atm<RenderThreadState>		_state = RenderThreadState::None;
	struct State
	{
		bool isStarted	= false;
		bool isQuit		= false;
	};
	using ProctectedState = CondMutexProtected<State, true>;
	ProctectedState			_state;		// TODO: CondMutexProtected
	CondQueue<RenderJob>	_pendingRdJobs;
	CondQueue<RenderJob>	_processingRdJobs;// this is for check the gpu side is completed or not

	//RenderThreadQueue			_rdThreadQueue;		// if, use other name, maybe like Dx12 called Engine as an interface for RenderThread
};

#endif

}


