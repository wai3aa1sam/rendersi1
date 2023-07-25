#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_Allocator-Decl ---
#endif // 0
#if 1

struct Vk_AllocInfo
{
	RenderMemoryUsage		usage			= RenderMemoryUsage::None;
	RenderAllocFlags		flags			= RenderAllocFlags::None;
	VkMemoryPropertyFlags	vkMemPropFlags	= {};
};

class Vk_Allocator : public NonCopyable
{
public:
	using Util = RenderApiUtil_Vk;

public:
	Vk_Allocator();
	~Vk_Allocator();

	void create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance_T* vkInst, const VkAllocationCallbacks* vkAllocCallbacks);
	void destroy();

	VkResult allocBuf(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, const VkBufferCreateInfo* bufferInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void freeBuf(Vk_Buffer_T* vkBuf, Vk_AllocHnd* allocHnd);

	void mapMem(void** outData, Vk_AllocHnd* allocHnd);
	void mapMem(u8** outData,	Vk_AllocHnd* allocHnd);
	void unmapMem(Vk_AllocHnd* allocHnd);

private:
	VmaMemoryUsage				toMemoryUsage(RenderMemoryUsage v);
	VmaAllocationCreateFlags	toAllocFlags(RenderAllocFlags v);

private:
	VmaAllocator _allocator;
};

class Vk_MemMap	// for presistent mapping
{
public:
	Vk_MemMap();
	~Vk_MemMap();

	void create(VkPtr<Vk_Buffer>& vkp);

	void map(void* outData);
	void unmap();

	// if no VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkFlushMappedMemoryRanges, vkInvalidateMappedMemoryRanges

protected:
	VkPtr<Vk_Buffer>& vkp;
};

class Vk_ScopedMemMap
{
public:
	Vk_ScopedMemMap(u8** outData, Vk_Buffer* vkBuf);
	~Vk_ScopedMemMap();

	void unmap();

protected:
	Vk_Buffer* _p = nullptr;
};

#endif
}

#endif