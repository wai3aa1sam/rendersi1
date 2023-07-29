#include "rds_render_api_layer-pch.h"
#include "rdsVk_MemoryContext.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_MemoryContext-Impl ---
#endif // 0
#if 1

Vk_MemoryContext* 
Vk_MemoryContext::instance()
{
	return Renderer_Vk::instance()->memoryContext();
}

Vk_MemoryContext::Vk_MemoryContext()
{
	//_allocationCallbacks.pfnAllocation
}
Vk_MemoryContext::~Vk_MemoryContext()
{
	destroy();
}

void 
Vk_MemoryContext::create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance_T* vkInst)
{
	vkGetPhysicalDeviceMemoryProperties(vkPhyDev, &_vkMemProperties);

	_vkAlloc.create(vkDev, vkPhyDev, vkInst, allocCallbacks());
}

void 
Vk_MemoryContext::destroy()
{
	_vkAlloc.destroy();
}

#endif

}

#endif