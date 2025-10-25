#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "rds_render_api_layer/command/rdsRenderRequest.h"

namespace rds
{

class RenderThread;
class RenderDevice;
class RenderContext;
class TransferFrame;
class RenderGraph;

class RenderJob
{
public:
	u64					frameCount		= 0;
	RenderDevice*		renderDevice	= nullptr;

public:
	// maybe support multiple? so use a function first. but seems no need anyways
	RenderRequest&		renderRequest() { return _renderRequest; };
	RenderGraph&		renderGraph()	{ return *_renderGraph; };

public:
	// TODO: change to private, and to RenderGraph no need RenderGraphFrame
	RenderRequest		_renderRequest;
	RenderGraph*		_renderGraph				= nullptr; // need a share pointer
	u32					_renderGraphFrameIdx		= 0;

public:
	void reset(RenderDevice* renderDevice_, RenderContext* rdCtx, u64 frameCount_);

public:
	// Transfer stuff should not in here, please put to RenderUploadSystem
	//TransferFrame		transferFrame;
	//UPtr<TransferFrame>	transferFrame = nullptr;
	SPtr<TransferFrame>	_transferFrame = nullptr;
};

#if 0
#pragma mark --- rdsRenderThreadQueue-Decl ---
#endif // 0
#if 1

class RenderThreadQueue : public NonCopyable
{
	friend class RenderThread;
public:
	RenderThreadQueue();
	~RenderThreadQueue();

	void create(RenderThread* renderThread);
	void destroy();

public:
	UPtr<RenderJob> newRenderJob(u64 frameCount);
	void submit(UPtr<RenderJob> renderJob);

	void submit(RenderDevice* renderDevice, u64 frameCount, RenderJob&& renderJob);

	void waitFrame(u64 frameCount, int sleepMs = 0);				// please reference to unreal similar fn, eg, a timeout

public:
	u64		currentFrameCount()					const;
	//u64		lastFinishedFrameCount()			const;

	bool	isSignaled()						const;		// isEngineReadyToProcess

	// old impl
	bool	isSignaled(u64 engineFrameCount)	const;		// isEngineReadyToProcess
	bool	isFinished(u64 engineFrameCount)	const;		// isEngineReadyToProcess

protected:
	UPtr<RenderJob> consumeRenderJob();
	void freeRenderJob(UPtr<RenderJob> renderJob);

private:
	RenderThread* _rdThread = nullptr;

	// sth maybe still running in gpu eg. TransferFrame, having safe frame is good, no need to wait
	//using RenderJobPool = MutexProtected<Vector<UPtr<RenderJob>, RenderApiLayerTraits::s_kFrameSafeInFlightCount> >;
	//RenderJobPool				_rdFramePool;
	AtmQueue<UPtr<RenderJob> >	_rdJobProducerQueue;
	AtmQueue<UPtr<RenderJob> >	_rdJobConsumerQueue;
};

#endif

}