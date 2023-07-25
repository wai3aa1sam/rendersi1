#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"
#include "rdsVk_Allocator.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_MemoryContext-Decl ---
#endif // 0
#if 1

class Vk_MemoryContext /*: public Singleton<Vk_MemoryContext>*/
{
public:
	static Vk_MemoryContext* instance();

public:
	Vk_MemoryContext();
	~Vk_MemoryContext();

	void create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance_T* vkInst);
	void destroy();

	const VkAllocationCallbacks*			allocCallbacks();
	const VkPhysicalDeviceMemoryProperties& vkMemoryProperties();

	Vk_Allocator*							allocVk();

private:
	VkAllocationCallbacks				_vkAllocCallbacks = {};
	VkPhysicalDeviceMemoryProperties	_vkMemProperties = {};
	Vk_Allocator						_allocVk;
};

inline Vk_Allocator* Vk_MemoryContext::allocVk() { return &_allocVk; }

inline const VkAllocationCallbacks* Vk_MemoryContext::allocCallbacks() { return /*&_allocationCallbacks*/ nullptr; }

inline const VkPhysicalDeviceMemoryProperties& Vk_MemoryContext::vkMemoryProperties() { return _vkMemProperties; }

#endif


#if 0
#pragma mark --- rdsVk_MemoryContext-Impl ---
#endif // 0
#if 1

#endif

}

#endif