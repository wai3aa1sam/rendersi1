#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"
#include "rdsAllocator_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsMemoryContext_Vk-Decl ---
#endif // 0
#if 1

class MemoryContext_Vk /*: public Singleton<MemoryContext_Vk>*/
{
public:
	static MemoryContext_Vk* instance();

public:
	MemoryContext_Vk();
	~MemoryContext_Vk();

	void create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance* vkInst);
	void destroy();

	const VkAllocationCallbacks*			allocCallbacks();
	const VkPhysicalDeviceMemoryProperties& vkMemoryProperties();

	Allocator_Vk*							allocVk();

private:
	VkAllocationCallbacks				_vkAllocCallbacks = {};
	VkPhysicalDeviceMemoryProperties	_vkMemProperties = {};
	Allocator_Vk						_allocVk;
};

inline Allocator_Vk* MemoryContext_Vk::allocVk() { return &_allocVk; }

inline const VkAllocationCallbacks* MemoryContext_Vk::allocCallbacks() { return /*&_allocationCallbacks*/ nullptr; }

inline const VkPhysicalDeviceMemoryProperties& MemoryContext_Vk::vkMemoryProperties() { return _vkMemProperties; }

#endif


#if 0
#pragma mark --- rdsMemoryContext_Vk-Impl ---
#endif // 0
#if 1

#endif

}

#endif