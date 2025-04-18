#include "rds_render_api_layer-pch.h"

#include "rdsRenderContext.h"

#include "rdsRenderer.h"

#include "command/rdsRenderRequest.h"

#include "graph/rdsRenderGraph.h"

namespace rds
{

SPtr<RenderContext> 
RenderDevice::createContext(const RenderContext_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateContext(cDesc);
	p->onPostCreate(cDesc);
	return p;
}


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
	const auto& framebufferSize = rdCtx->framebufferSize();
	texCDesc.size.set(sCast<u32>(framebufferSize.x), sCast<u32>(framebufferSize.y), sCast<u32>(1));
	texCDesc.usageFlags = TextureUsageFlags::BackBuffer;

	_images.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		auto& image = _images[i];
		image = rdCtx->renderDevice()->createTexture2D(texCDesc);
		RDS_RenderResouce_SET_DEBUG_NAME(image, fmtAs_T<TempString>("Backbuffer-{}", i));
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

Texture2D*	Backbuffers::backbuffer()			{ return _images[_rdCtx->_curImageIdx]; }


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
RenderContext::make(const CreateDesc& cDesc) 
{ 
	return Renderer::renderDevice()->createContext(cDesc); 
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
	destroy();

	Base::create(cDesc);
	onCreate(cDesc);
}

void
RenderContext::destroy()
{
	if (!Base::hasCreated())
		return;

	_rdUiCtx.destroy();

	onDestroy();
	Base::destroy();
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
RenderContext::commit(RenderGraph& rdGraph, u32 rdGraphFrameIdx)
{
	onCommit(rdGraph, rdGraph.renderGraphFrame(rdGraphFrameIdx), rdGraphFrameIdx);
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
RenderContext::setFramebufferSize(const Vec2f& newSize)
{
	if (_framebufferSize == newSize)
		return;

	RDS_PROFILE_SCOPED();

	_framebufferSize = newSize;
	onSetFramebufferSize(newSize);
}

bool 
RenderContext::isFrameFinished(u64 frameCount) 
{ 
	throwIf(true, "should not call this");
	return false;
}

void 
RenderContext::waitFrameFinished(u64 frameCount) 
{ 
	throwIf(true, "should not call this");
}

void
RenderContext::onCreate(const CreateDesc& cDesc)
{
	_framebufferSize.x = cDesc.window->clientRect().w;
	_framebufferSize.y = cDesc.window->clientRect().h;

	_nativeUIWindow = cDesc.window;
	_backbuffers.create(this, s_kFrameInFlightCount);

	{
		auto bufCDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
		bufCDesc.bufSize	= 16;
		bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;
		_dummyVtxBuf = renderDevice()->createRenderGpuBuffer(bufCDesc);
		_dummyVtxBuf->setDebugName("dummyVtxBuf");
	}
}

void
RenderContext::onPostCreate(const CreateDesc& cDesc)
{
	_rdUiCtx.create(this);
}

void
RenderContext::onDestroy()
{
	RDS_CORE_ASSERT(_backbuffers.isEmpty(), "must clear in backend");
}

void 
RenderContext::onCommit(RenderCommandBuffer& renderBuf)
{

}

void 
RenderContext::onCommit(const RenderGraph& rdGraph, RenderGraphFrame& rdGraphFrame, u32 rdGraphFrameIdx)
{

}

float 
RenderContext::aspectRatio() const
{
	auto y = framebufferSize().y != 0 ? framebufferSize().y : 1;
	return framebufferSize().x / y;
}

#endif
}