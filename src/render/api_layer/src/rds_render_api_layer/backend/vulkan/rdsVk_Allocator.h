#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_Allocator-Decl ---
#endif // 0
#if 1

struct Vk_AllocInfo
{
	RenderMemoryUsage		usage			= RenderMemoryUsage::Auto;
	RenderAllocFlags		flags			= RenderAllocFlags::None;
	VkMemoryPropertyFlags	vkMemPropFlags	= {};

	void* outMappedData = nullptr;
};

class Vk_Allocator : public NonCopyable
{
public:
	using Util = Vk_RenderApiUtil;

public:
	Vk_Allocator();
	~Vk_Allocator();

	void create(RenderDevice_Vk* rdDev);
	void create(Vk_Device_T* vkDev, Vk_PhysicalDevice_T* vkPhyDev, Vk_Instance_T* vkInst, const VkAllocationCallbacks* vkAllocCallbacks);
	void destroy();

	VkResult	allocBuf(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, const VkBufferCreateInfo* bufferInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void		freeBuf	(Vk_Buffer_T* vkBuf, Vk_AllocHnd* allocHnd);

	VkResult	allocImage	(Vk_Image_T** outImg, Vk_AllocHnd* allocHnd, const VkImageCreateInfo* imageInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void		freeImage	(Vk_Image_T* vkImg, Vk_AllocHnd* allocHnd);


	void mapMem(void** outData, Vk_AllocHnd* allocHnd);
	void mapMem(u8** outData,	Vk_AllocHnd* allocHnd);
	void unmapMem(Vk_AllocHnd* allocHnd);

	void setAllocationDebugName(const char* name, Vk_AllocHnd* allocHnd);

private:
	VmaMemoryUsage				toMemoryUsage(RenderMemoryUsage v);
	VmaAllocationCreateFlags	toAllocFlags(RenderAllocFlags v);

private:
	VmaAllocator _allocator;
};

#endif
}

#endif