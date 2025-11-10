#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "nmsp_job_system/thread/nmspTypeThread.h"
#include "EASTL/slist.h"

namespace rds
{

class RenderJob;
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

#if 1


template<class T> 
class CondQueue : public AtmQueue<T>
{
public:
	using Base		= AtmQueue<T>;
	using SizeType	= typename Base::SizeType;

public:
	CondQueue() = default;
	~CondQueue() = default;

public:
	void push(const T& data)	{ _size++;  Base::push(data); RDS_TODO("real impl for CondQueue, CondVarProtected"); }
	void push(		T&& data)	{ _size++;  Base::push(rds::move(data)); }

	bool try_pop(T& o) { bool isSuccess = Base::try_pop(o); if (isSuccess) { _size--; }  return isSuccess; }

	void clear() { T o; while(try_pop(o)) {}; }

public:
	SizeType	size()		const { return _size; }
	bool		isEmpty()	const { return _size == 0; }

private:
	Atm<u32>	_size = 0;
};

#endif // 1


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
	void requestRender(UPtr<RenderJob> renderJob);
	void quit();

public:
	bool tryRender();
	bool tryExecuteStealJob();

	void render(UPtr<RenderJob> renderJob);

public:
	bool isQuit();

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
	MutexProtected<State>		_state;		// TODO: CondMutexProtected
	CondQueue<UPtr<RenderJob> >	_pendingRdJobs;
	CondQueue<UPtr<RenderJob> >	_processingRdJobs;// this is for check the gpu side is completed or not

	//RenderThreadQueue			_rdThreadQueue;		// if, use other name, maybe like Dx12 called Engine as an interface for RenderThread
};

#endif

}


