#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderApi_Include_Vk.h"


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

	void create(Vk_PhysicalDevice* vkPhyDev);

	const VkAllocationCallbacks*			allocCallbacks();
	const VkPhysicalDeviceMemoryProperties& vkMemoryProperties();


private:
	VkAllocationCallbacks				_allocCallbacks	 = {};
	VkPhysicalDeviceMemoryProperties	_vkMemProperties = {};

};


#endif


#if 0
#pragma mark --- rdsMemoryContext_Vk-Impl ---
#endif // 0
#if 1

inline const VkAllocationCallbacks* MemoryContext_Vk::allocCallbacks() { return /*&_allocationCallbacks*/ nullptr; }

inline const VkPhysicalDeviceMemoryProperties& MemoryContext_Vk::vkMemoryProperties() { return _vkMemProperties; }


#endif

}

#endif