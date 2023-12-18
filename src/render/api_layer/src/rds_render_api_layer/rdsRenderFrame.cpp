#include "rds_render_api_layer-pch.h"

#include "rdsRenderFrame.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderFrame-Impl ---
#endif // 0
#if 1

RenderFrame::RenderFrame()
{
	
}

RenderFrame::~RenderFrame()
{
	destroy();
}

void RenderFrame::create()
{
	destroy();

	RDS_TODO("remove");
	_renderCommandAllocators.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		_renderCommandAllocators.emplace_back(makeUPtr<LinearAllocator>());
	}
	/*_renderCommandPools.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		_renderCommandPools[i].emplace_back(makeUPtr<RenderCommandBuffer>());
	}*/
	_renderCommandPool.emplace_back(makeUPtr<RenderCommandBuffer>());

}

void RenderFrame::destroy()
{
	reset();
}

void 
RenderFrame::reset()
{
	renderQueue().clear();

	// must be last
	for (auto& e : _renderCommandAllocators)
	{
		e->clear();
	}
	for (auto& e : _renderCommandPool)
	{
		e->reset();
		//e.clear();
	}
}

RenderCommandBuffer* 
RenderFrame::requestCommandBuffer()
{
	RDS_TODO("better handle");
	return _renderCommandPool[0];
}


#endif

#if 0
#pragma mark --- rdsRenderFrameContext-Impl ---
#endif // 0
#if 1

//RenderFrameContext* RenderFrameContext::s_instance = nullptr;

RenderFrameContext::RenderFrameContext()
{
	_renderFrames.resize(s_kFrameInFlightCount);
}

RenderFrameContext::~RenderFrameContext()
{
}

void 
RenderFrameContext::destroy()
{
	clear();
}

void 
RenderFrameContext::clear()
{
	_renderFrames.clear();
}

void 
RenderFrameContext::rotate()
{
	RDS_TODO("RenderFrameContext should belong to Renderer");

	iFrame = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
	renderFrame().reset();
}

#endif

}