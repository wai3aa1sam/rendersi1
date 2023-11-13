#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
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

	Vk_Allocator*							vkAlloc();

private:
	VkAllocationCallbacks				_vkAllocCallbacks = {};
	VkPhysicalDeviceMemoryProperties	_vkMemProperties = {};
	Vk_Allocator						_vkAlloc;
};

inline Vk_Allocator* Vk_MemoryContext::vkAlloc() { return &_vkAlloc; }

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