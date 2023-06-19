#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

SPtr<RenderContext> Renderer_Vk::onCreateContext(const RenderContext_CreateDesc& cDesc)
{
	auto p = SPtr<RenderContext>(RDS_NEW(RenderContext_Vk)());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderContext_Vk-Impl ---
#endif // 0
#if 1

RenderContext_Vk::RenderContext_Vk()
	: Base()
{

}

RenderContext_Vk::~RenderContext_Vk()
{
	destroy();
}

void 
RenderContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
RenderContext_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void 
RenderContext_Vk::onDestroy()
{
	Base::onDestroy();
}


#endif
}

#endif // RDS_RENDER_HAS_VULKAN