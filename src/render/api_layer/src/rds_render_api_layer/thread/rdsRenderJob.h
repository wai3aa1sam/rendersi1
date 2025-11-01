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

struct RenderJob_CreateDesc
{
	RenderDevice* renderDevice	= nullptr;
};

#if 0
#pragma mark --- rdsRenderJob-Impl ---
#endif // 0
#if 1

// maybe use RenderRequest as a name is better, and original class RenderRequest -> class RenderCommandList
class RenderJob : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using CreateDesc = RenderJob_CreateDesc;

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

	bool isDoneUploading();
	bool isDoneRendering();

public:
	// Transfer stuff should not in here, please put to RenderUploadSystem
	//TransferFrame		transferFrame;
	//UPtr<TransferFrame>	transferFrame = nullptr;
	SPtr<TransferFrame>	_transferFrame = nullptr;

public:
	RenderJob();
	virtual ~RenderJob();

public:
	void create(CreateDesc& cDesc);
	void destroy();

protected:
	virtual void onCreate(CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onReset(RenderContext* rdCtx);
	virtual bool onCheckUploadCompleted() = 0;
	virtual bool onCheckRenderCompleted() = 0;
};

#endif

}