#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "rds_render_api_layer/command/rdsTransferRequest.h"
#include "rds_render_api_layer/command/rdsRenderCommand.h"
#include "rds_render_api_layer/command/rdsRenderQueue.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderFrame-Decl ---
#endif // 0
#if 1

class RenderFrame : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using RenderCommandPool = Vector<UPtr<RenderCommandBuffer>, 12>;

public:
	RenderFrame();
	~RenderFrame();

	RenderFrame(RenderFrame&&)		{ throwIf(true, ""); };
	void operator=(RenderFrame&&)	{ throwIf(true, ""); };

	void create();
	void destroy();

	void reset();

	RenderCommandBuffer* requestCommandBuffer();

	RenderQueue&		renderQueue();
	LinearAllocator&	renderCommandAllocator();

protected:
	Vector<UPtr<LinearAllocator>,	Traits::s_kThreadCount>	_renderCommandAllocators;
	//Vector<RenderCommandPool,		Traits::s_kThreadCount>	_renderCommandPools;
	RenderCommandPool _renderCommandPool;

	RenderQueue _renderQueue;
};

inline LinearAllocator&		RenderFrame::renderCommandAllocator()	{ auto tlid = OsTraits::threadLocalId(); return *_renderCommandAllocators[tlid]; }
inline RenderQueue&			RenderFrame::renderQueue()				{ return _renderQueue; }

#endif

#if 0
#pragma mark --- rdsRenderFrameContext-Decl ---
#endif // 0
#if 1


class RenderFrameContext : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderFrameContext();
	~RenderFrameContext();
	
	void destroy();

	void clear();
	void rotate();

	RenderFrame& renderFrame();

protected:
	Atm<u32> iFrame = 0;
	Vector<RenderFrame,				s_kFrameInFlightCount> _renderFrames;
};

inline RenderFrame& RenderFrameContext::renderFrame() { return _renderFrames[iFrame]; }

#endif

}


