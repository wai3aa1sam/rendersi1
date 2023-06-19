#include "rds_render_api_layer-pch.h"
#include "rdsMemoryContext_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsMemoryContext_Vk-Impl ---
#endif // 0
#if 1

MemoryContext_Vk* 
MemoryContext_Vk::instance()
{
	return Renderer_Vk::instance()->memoryContext();
}

MemoryContext_Vk::MemoryContext_Vk()
{
	//_allocationCallbacks.pfnAllocation
}



#endif

}

#endif