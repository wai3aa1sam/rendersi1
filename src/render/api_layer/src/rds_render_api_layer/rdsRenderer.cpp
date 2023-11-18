#include "rds_render_api_layer-pch.h"

#include "rdsRenderer.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{
#if 0
#pragma mark --- rdsRenderer-Impl ---
#endif // 0
#if 1

Renderer* Renderer::s_instance = nullptr;

Renderer::CreateDesc Renderer::makeCDesc() { return CreateDesc{}; }

RenderDevice*	
Renderer::rdDev()
{
	return Renderer::instance()->_rdDevs[0];
}

RenderDevice* 
Renderer::rdDev(u32 i)
{
	return Renderer::instance()->_rdDevs[i];
}


Renderer::Renderer()
	: Base()
{
}

Renderer::~Renderer()
{
	destroy();
	RDS_LOG_DEBUG("~Renderer()");
}

void 
Renderer::create(const CreateDesc& cDesc)
{
	_rdDevs.resize(cDesc.gpuCount);
	for (auto& e : _rdDevs)
	{
		switch (cDesc.apiType)
		{
			using SRC = RenderApiType;
			case SRC::Vulkan: { e.reset(RDS_NEW(RenderDevice_Vk)()); }	break;
			default: { RDS_THROW("unsupported render api"); }			break;
		}
		e->create(cDesc);
	}
}

void 
Renderer::destroy()
{
	_rdDevs.clear();
}

#endif
}
