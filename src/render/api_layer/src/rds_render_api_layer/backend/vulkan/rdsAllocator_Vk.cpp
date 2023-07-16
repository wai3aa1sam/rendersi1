#include "rds_render_api_layer-pch.h"
#include "rdsAllocator_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

RDS_DISABLE_ALL_WARNINGS();

#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.3
#include <vk_mem_alloc.h>

RDS_RESTORE_ALL_WARNINGS();

#endif

#if RDS_RENDER_HAS_VULKAN


namespace rds
{

#if 0
#pragma mark --- rdsAllocator_Vk-Impl ---
#endif // 0
#if 1

Allocator_Vk::Allocator_Vk()
{

}

Allocator_Vk::~Allocator_Vk()
{
	destroy();
}

void 
Allocator_Vk::create(Vk_Device* vkDev, Vk_PhysicalDevice* vkPhyDev, Vk_Instance* vkInst, const VkAllocationCallbacks* vkAllocCallbacks)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.instance				= vkInst;
	allocatorInfo.physicalDevice		= vkPhyDev;
	allocatorInfo.device				= vkDev;
	allocatorInfo.pAllocationCallbacks	= vkAllocCallbacks;
	vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void 
Allocator_Vk::destroy()
{
	vmaDestroyAllocator(_allocator);
	_allocator = nullptr;
}

VkResult
Allocator_Vk::allocBuf(VkPtr<Vk_Buffer>& outBuf, const VkBufferCreateInfo* bufferInfo, const AllocInfo_Vk* allocInfo)
{
	VmaAllocationCreateInfo vmaAllocCInfo = {};
	vmaAllocCInfo.usage = toMemoryUsage(allocInfo->usage);
	vmaAllocCInfo.flags = toAllocFlags(allocInfo->flags);
	//vmaAllocCInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	
	outBuf.reset(nullptr);
	outBuf._internal_setAlloc(this);
	auto ret = vmaCreateBuffer(_allocator, bufferInfo, &vmaAllocCInfo, outBuf.ptrForInit(), outBuf._internal_allocHnd(), nullptr);
	Util::throwIfError(ret);

	return ret;
}

void 
Allocator_Vk::freeBuf(Vk_Buffer* vkBuf, AllocHnd_Vk* allocHnd)
{
	vmaDestroyBuffer(_allocator, vkBuf, *allocHnd);
}

void 
Allocator_Vk::mapMem(void** outData, AllocHnd_Vk* allocHnd)
{
	vmaMapMemory(_allocator, *allocHnd, outData);
}

void 
Allocator_Vk::unmapMem(AllocHnd_Vk* allocHnd)
{
	vmaUnmapMemory(_allocator, *allocHnd);
}

VmaMemoryUsage 
Allocator_Vk::toMemoryUsage(RenderMemoryUsage v)
{
	using SRC = RenderMemoryUsage;
	switch (v)
	{
		case SRC::CpuToGpu:	{ return VMA_MEMORY_USAGE_CPU_TO_GPU;  }	break;
		case SRC::CpuOnly:	{ return VMA_MEMORY_USAGE_CPU_ONLY;  }		break;

		case SRC::GpuToCpu:	{ return VMA_MEMORY_USAGE_GPU_TO_CPU;  }	break;
		case SRC::GpuOnly:	{ return VMA_MEMORY_USAGE_GPU_ONLY;  }		break;
	}
	RDS_CORE_ASSERT(false, "unsupported memory usage");
	return VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN;
}

VmaAllocationCreateFlags
Allocator_Vk::toAllocFlags(RenderAllocFlags v)
{
	using SRC = RenderAllocFlags;
	using DST = VmaAllocationCreateFlagBits;
	VmaAllocationCreateFlags flags = {};

	if (BitUtil::has(v, SRC::HostWrite))	{ flags |= DST::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; }
	if (BitUtil::has(v, SRC::CreateMapped))	{ flags |= DST::VMA_ALLOCATION_CREATE_MAPPED_BIT; }

	return flags;
}

ScopedMemMap_Vk::ScopedMemMap_Vk(void** outData, VkPtr<Vk_Buffer>* vkp)
{
	_p = vkp;
	vkp->_internal_alloc()->mapMem(outData, _p->_internal_allocHnd());
}

ScopedMemMap_Vk::~ScopedMemMap_Vk()
{
	if (_p)
	{
		unmap();
	}
}

void ScopedMemMap_Vk::unmap()
{
	_p->_internal_alloc()->unmapMem(_p->_internal_allocHnd());
	_p = nullptr;
}

#endif

}

#endif