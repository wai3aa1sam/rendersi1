#include "rds_render_api_layer-pch.h"

#include "rdsRenderContext.h"

#include "rdsRenderer.h"
#include "rdsRenderFrame.h"

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
	return Renderer::rdDev()->createContext(cDesc); 
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

	onDestroy();
	Base::destroy();
}

void
RenderContext::beginRender()
{
	RDS_PROFILE_SCOPED();
	onBeginRender();
}

void
RenderContext::endRender()
{
	RDS_PROFILE_SCOPED();
	onEndRender();
}

void 
RenderContext::commit(RenderCommandBuffer& renderBuf)
{
	onCommit(renderBuf);
}

void 
RenderContext::commit(TransferCommandBuffer& transferBuf)
{
	onCommit(transferBuf);
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

void
RenderContext::onCreate(const CreateDesc& cDesc)
{
	_framebufferSize.x = cDesc.window->clientRect().w;
	_framebufferSize.y = cDesc.window->clientRect().h;

	_nativeUIWindow = cDesc.window;
}

void
RenderContext::onPostCreate(const CreateDesc& cDesc)
{

}

void
RenderContext::onDestroy()
{

}

void 
RenderContext::onCommit(RenderCommandBuffer& renderBuf)
{

}

void
RenderContext::onCommit(TransferCommandBuffer& transferBuf)
{

}

void 
RenderContext::uploadBuffer(RenderFrameUploadBuffer& rdfUploadBuf)
{
	onUploadBuffer(rdfUploadBuf);
}

void 
RenderContext::onUploadBuffer(RenderFrameUploadBuffer& rdfUploadBuf)
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