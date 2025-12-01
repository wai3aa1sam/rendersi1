#include "rds_render_api_layer-pch.h"

#include "rdsRenderContext.h"

#include "rdsRenderer.h"
#include "thread/rdsRenderJob.h"

#include "command/rdsRenderRequest.h"
#include "graph/rdsRenderGraph.h"

#include "transfer/rdsTransferContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsBackbuffers-Impl ---
#endif // 0
#if 1

void 
Backbuffers::create(RenderContext* rdCtx, SizeType imageCount)
{
	destroy();
	_rdCtx = rdCtx;

	auto texCDesc = Texture2D::makeCDesc();
	const auto& framebufferSize = rdCtx->swapchainSize();
	texCDesc.size.set(sCast<u32>(framebufferSize.x), sCast<u32>(framebufferSize.y), sCast<u32>(1));
	texCDesc.usageFlags = TextureUsageFlags::BackBuffer;

	_images.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		auto& image = _images[i];
		auto dblbl = RDS_DebugLabel_COPY(rdCtx->DebugLabel_get(), "{}.backbuffers[{}]", rdCtx->DebugLabel_getName(), i);
		image = rdCtx->renderDevice()->createTexture2D(dblbl, texCDesc);
	}
}

void 
Backbuffers::destroy()
{
	for (auto& e : _images)
	{
		e->setNull();
	}
	_images.clear();
	_rdCtx = nullptr;
}

//Texture2D*	Backbuffers::backbuffer()			{ return _images[_rdCtx->_curImageIdx]; }


#endif

#if 0
#pragma mark --- rdsRenderContext-Impl ---
#endif // 0
#if 1

RenderContext::CreateDesc 
RenderContext::makeCDesc() 
{ 
	return CreateDesc{}; 
}

SPtr<RenderContext> 
RenderContext::make(RDS_DebugLabel_PARAM, const CreateDesc& cDesc) 
{ 
	return Renderer::renderDevice()->createContext(RDS_DebugLabel_ARG, cDesc); 
}

RenderContext::RenderContext()
{

}

RenderContext::~RenderContext()
{
	RDS_LOG_DEBUG("~RenderContext()");
}

void
RenderContext::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	transferContext().transferFrame().createRenderContext(this);
}

void
RenderContext::onDestroy()
{
	//RDS_CORE_ASSERT(_backbuffers.isEmpty(), "must clear in backend");
	// prevent spwan more commands when destroy in RenderThread
	_rdUiCtx.destroy();
	_dummyVtxBuf.reset(nullptr);

	transferContext().transferFrame().destroyRenderContext(this);

	Base::onDestroy();
}

void 
RenderContext::Render_reset(RenderJob* rdJob)
{
	_rdJob = rdJob;
}

void
RenderContext::beginRender()
{
	RDS_PROFILE_SCOPED();

	//_rdUiCtx.onBeginRender(this);
	onBeginRender();
}

void
RenderContext::endRender()
{
	RDS_PROFILE_SCOPED();
	
	onEndRender();
	//_rdUiCtx.onEndRender(this);
	_rdJob = nullptr;
}

void 
RenderContext::commit(RenderCommandBuffer& rdCmdBuf)
{
	onCommit(rdCmdBuf);
}

void 
RenderContext::commit(RenderRequest& rdReq)
{
	rdReq._internal_commit();
}

void 
RenderContext::commit(RenderGraph& rdGraph)
{
	onCommit(rdGraph);
}

void 
RenderContext::commit()
{
	RDS_CORE_ASSERT(_rdJob, "Proxy_RenderContext::reset(RenderJob*)");
	_rdJob->renderGraph().commit();
	commit(_rdJob->renderRequest());
}

void 
RenderContext::drawUI(RenderRequest& req)
{
	_rdUiCtx.onDrawUI(req);
}

bool 
RenderContext::onUiMouseEvent(UiMouseEvent& ev)
{
	return _rdUiCtx.onUiMouseEvent(ev);
}

bool 
RenderContext::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	return _rdUiCtx.onUiKeyboardEvent(ev);
}

void
RenderContext::setSwapchainSize(const Vec2f& newSize)
{
	if (_swapchainSize == newSize)
		return;

	RDS_PROFILE_SCOPED();

	_swapchainSize = newSize;
	transferRequest().setSwapchainSize(this, newSize);
}

void
RenderContext::onCreate(const CreateDesc& cDesc)
{
	_swapchainSize.x = cDesc.window->clientRect().w;
	_swapchainSize.y = cDesc.window->clientRect().h;

	_nativeUIWindow = cDesc.window;
	
	#if 0
	_backbuffers.create(this, s_kMaxFrameAheadCountHardLimit);
	#endif // 0

	{
		auto bufCDesc = RenderGpuBuffer::makeCDesc();
		bufCDesc.bufSize	= 16;
		bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;
		_dummyVtxBuf = renderDevice()->createRenderGpuBuffer(RDS_DebugLabel("dummyVtxBuf"), bufCDesc);
	}

	_rdUiCtx.create(this);
}

void
RenderContext::onPostCreate(const CreateDesc& cDesc)
{
	
}

void 
RenderContext::onCommit(RenderCommandBuffer& renderBuf)
{

}

void 
RenderContext::onCommit(RenderGraph& rdGraph)
{

}

void 
RenderContext::onCommit()
{

}


const SrcLocData*
RenderContext::addGpuProfileSection(const SrcLocData& srcLocData)
{
	#if RDS_USE_GPU_PROFILER
	return _gpuProfiler.addUniqueProfileSection(srcLocData);
	#endif
}

float 
RenderContext::aspectRatio() const
{
	auto y = swapchainSize().y != 0 ? swapchainSize().y : 1;
	return swapchainSize().x / y;
}

void 
RenderContext::onSetSwapchainSize(const Vec2f& newSize)
{
	checkRenderThreadExclusive(RDS_SRCLOC);
}

#endif
}