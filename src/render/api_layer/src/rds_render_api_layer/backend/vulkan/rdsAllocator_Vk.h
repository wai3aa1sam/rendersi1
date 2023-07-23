#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsAllocator_Vk-Decl ---
#endif // 0
#if 1

struct AllocInfo_Vk
{
	RenderMemoryUsage		usage			= RenderMemoryUsage::None;
	RenderAllocFlags		flags			= RenderAllocFlags::None;
	VkMemoryPropertyFlags	vkMemPropFlags	= {};
};

class Allocator_Vk : public NonCopyable
{
public:
	using Util = RenderApiUtil_Vk;

public:
	Allocator_Vk();
	~Allocator_Vk();

	void create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance_T* vkInst, const VkAllocationCallbacks* vkAllocCallbacks);
	void destroy();

	VkResult allocBuf(VkPtr<Vk_Buffer>& outBuf, const VkBufferCreateInfo* bufferInfo, const AllocInfo_Vk* allocInfo);
	void freeBuf(Vk_Buffer* vkBuf, AllocHnd_Vk* allocHnd);

	void mapMem(void** outData, AllocHnd_Vk* allocHnd);
	void unmapMem(AllocHnd_Vk* allocHnd);

private:
	VmaMemoryUsage				toMemoryUsage(RenderMemoryUsage v);
	VmaAllocationCreateFlags	toAllocFlags(RenderAllocFlags v);

private:
	VmaAllocator _allocator;
};

class MemMap_Vk	// for presistent mapping
{
public:
	MemMap_Vk();
	~MemMap_Vk();

	void create(VkPtr<Vk_Buffer>& vkp);

	void map(void* outData);
	void unmap();

	// if no VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkFlushMappedMemoryRanges, vkInvalidateMappedMemoryRanges

protected:
	VkPtr<Vk_Buffer>& vkp;
};

class ScopedMemMap_Vk
{
public:
	ScopedMemMap_Vk(u8** outData, VkPtr<Vk_Buffer>* vkp);
	~ScopedMemMap_Vk();

	void unmap();

protected:
	VkPtr<Vk_Buffer>* _p = nullptr;
};

#endif
}

#endif