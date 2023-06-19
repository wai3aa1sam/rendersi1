#include "rds_render_api_layer-pch.h"

#include "rdsRenderContext.h"

#include "rdsRenderer.h"


namespace rds
{

SPtr<RenderContext> Renderer::createContext(const RenderContext_CreateDesc& cDesc)
{
	auto p = onCreateContext(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderContext-Impl ---
#endif // 0
#if 1

SPtr<RenderContext> RenderContext::make(const CreateDesc& cDesc) { return Renderer::instance()->createContext(cDesc); }

RenderContext::RenderContext()
{

}

RenderContext::~RenderContext()
{

}

void
RenderContext::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void
RenderContext::destroy()
{
	onDestroy();
}

void RenderContext::onCreate(const CreateDesc& cDesc)
{

}

void RenderContext::onPostCreate(const CreateDesc& cDesc)
{

}

void RenderContext::onDestroy()
{

}


#endif
}