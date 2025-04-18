#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"

namespace rds
{

class RenderThread;
class RenderDevice;

struct RenderData_RenderJob
{
public:
	RenderGraph*	renderGraph				= nullptr; // need a share pointer
	RenderRequest*	renderRequest			= nullptr; // TODO: temp, need a ownership is better
	u32				renderGraphFrameIdx		= 0;

public:
	void create(RenderGraph* renderGraph_, RenderRequest* renderReq);
};

struct RenderData
{
	using RenderJob		= RenderData_RenderJob;
	using RenderJobs	= Vector<RenderJob, 6>; 

public:
	u64				frameCount		= 0;
	RenderDevice*	renderDevice	= nullptr;
	RenderJobs		renderJobs;
};

#if 0
#pragma mark --- rdsRenderThreadQueue-Decl ---
#endif // 0
#if 1

class RenderThreadQueue : public NonCopyable
{
public:
	using RenderJob		= RenderData_RenderJob;

public:
	RenderThreadQueue();
	~RenderThreadQueue();

	void create(RenderThread* renderThread);
	void destroy();

public:
	void submit(RenderDevice* renderDevice, u64 frameCount, RenderJob&& renderJob);

	void waitFrame(u64 frameCount);				// please reference to unreal similar fn, eg, a timeout

public:
	u64		currentFrameCount()					const;
	//u64		lastFinishedFrameCount()			const;
	bool	isSignaled(u64 engineFrameCount)	const;

private:
	RenderThread* _rdThread = nullptr;
};


#endif

}