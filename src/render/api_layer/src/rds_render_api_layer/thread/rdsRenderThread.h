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
	void requestRender(UPtr<RenderData>&& renderData);
	void terminate();

public:
	bool	isTerminated()				const;
	u64		lastFinishedFrameCount()	const;

protected:
	virtual void* onRoutine() override;
	void render(RenderData& renderData);

public:		// TODO: remove temp
	void _temp_render();

private:
	AtmQueue<UPtr<RenderData> > _rdDataQueue;
	Atm<bool>					_isTerminated = false;
	Atm<u64>					_lastFinishedFrameCount = 0;
};

#endif

}