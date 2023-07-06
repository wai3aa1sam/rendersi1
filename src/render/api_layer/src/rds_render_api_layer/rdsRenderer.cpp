#include "rds_render_api_layer-pch.h"

#include "rdsRenderer.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

namespace rds
{
#if 0
#pragma mark --- rdsRenderer-Impl ---
#endif // 0
#if 1

Renderer* Renderer::s_instance = nullptr;

Renderer_CreateDesc::Renderer_CreateDesc()
{
	apiType		= RenderApiType::Vulkan;
	isPresent	= true;

	#if RDS_DEBUG
	isDebug = true;
	#else
	isDebug = false;
	#endif // RDS_DEBUG

}

Renderer::CreateDesc Renderer::makeCDesc() { return CreateDesc{}; }

Renderer::Renderer()
	: Base()
{
}

Renderer::~Renderer()
{

}

void 
Renderer::create(const CreateDesc& cDesc)
{
	auto* rdr = _init(cDesc); RDS_UNUSED(rdr);
	rdr->onCreate(cDesc);
}


void 
Renderer::destroy()
{
	onDestroy();
}

void 
Renderer::onCreate(const CreateDesc& cDesc)
{

}

void 
Renderer::onDestroy()
{

}

Renderer*
Renderer::_init(const CreateDesc& cDesc)
{
	Renderer* rdr = nullptr;
	switch (cDesc.apiType)
	{
		using EM = RenderApiType;
		case EM::Vulkan: { rdr = RDS_NEW(Renderer_Vk)(); } break;

		default: { throwIf(false, "unsupported render api"); } break;
	}
	s_instance = rdr;
	return rdr;
}



#endif
}
