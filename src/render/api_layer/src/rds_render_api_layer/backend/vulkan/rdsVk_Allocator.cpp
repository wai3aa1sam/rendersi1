#include "rds_render_api_layer-pch.h"
#include "rdsVk_Allocator.h"

#include "rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

RDS_DISABLE_ALL_WARNINGS();

	#define VMA_IMPLEMENTATION
	#include <vk_mem_alloc.h>

RDS_RESTORE_ALL_WARNINGS();

#endif

#if RDS_RENDER_HAS_VULKAN


namespace rds
{

#if 0
#pragma mark --- rdsVk_Allocator-Impl ---
#endif // 0
#if 1

Vk_Allocator::Vk_Allocator()
{

}

Vk_Allocator::~Vk_Allocator()
{
	destroy();
}

void 
Vk_Allocator::create(RenderDevice_Vk* rdDev)
{
	create(rdDev->vkDevice(), rdDev->vkPhysicalDevice(), rdDev->vkInstance(), rdDev->allocCallbacks());
}

void 
Vk_Allocator::create(Vk_Device_T* vkDev, Vk_PhysicalDevice_T* vkPhyDev, Vk_Instance_T* vkInst, const VkAllocationCallbacks* vkAllocCallbacks)
{
	VmaAllocatorCreateInfo allocatorCInfo = {};
	allocatorCInfo.instance				= vkInst;
	allocatorCInfo.physicalDevice		= vkPhyDev;
	allocatorCInfo.device				= vkDev;
	allocatorCInfo.pAllocationCallbacks	= vkAllocCallbacks;
	allocatorCInfo.flags				= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorCInfo, &_allocator);
}

void 
Vk_Allocator::destroy()
{
	if (!_allocator)
		return;

	vmaDestroyAllocator(_allocator);
	_allocator = nullptr;
}

void 
Vk_Allocator::resetFrame(u64 frameCount)
{
	vmaSetCurrentFrameIndex(_allocator, sCast<u32>(frameCount));
}

VkResult
Vk_Allocator::allocBuf(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, const VkBufferCreateInfo* bufferInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	RDS_CORE_ASSERT(outBuf || allocHnd, "");

	VmaAllocationCreateInfo vmaAllocCInfo = {};
	vmaAllocCInfo.usage			= toMemoryUsage(allocInfo->usage);
	vmaAllocCInfo.flags			= toAllocFlags(allocInfo->flags);
	vmaAllocCInfo.requiredFlags = vkMemPropFlags;
	
	//vmaAllocCInfo.pUserData = nullptr;
	/*
	VkBuffer buffer;
	VmaAllocation allocation;
	vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
	*/
	
	VmaAllocationInfo outInfo;
	auto ret = vmaCreateBuffer(_allocator, bufferInfo, &vmaAllocCInfo, outBuf, allocHnd, &outInfo);
	allocInfo->outMappedData = outInfo.pMappedData;
	
	Util::throwIfError(ret);
	
	return ret;
}

void 
Vk_Allocator::freeBuf(Vk_Buffer_T* vkBuf, Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(vkBuf, "");
	vmaDestroyBuffer(_allocator, vkBuf, *allocHnd);
}

VkResult	
Vk_Allocator::allocImage(Vk_Image_T** outImg, Vk_AllocHnd* allocHnd, const VkImageCreateInfo* imageInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	RDS_CORE_ASSERT(outImg || allocHnd, "");

	VmaAllocationCreateInfo vmaAllocCInfo = {};
	vmaAllocCInfo.usage			= toMemoryUsage(allocInfo->usage);
	vmaAllocCInfo.flags			= toAllocFlags(allocInfo->flags);
	//vmaAllocCInfo.requiredFlags = vkMemPropFlags;

	auto ret = vmaCreateImage(_allocator, imageInfo, &vmaAllocCInfo, outImg, allocHnd, nullptr);
	Util::throwIfError(ret);

	return ret;
}

void		
Vk_Allocator::freeImage	(Vk_Image_T* vkImg, Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(vkImg, "");
	vmaDestroyImage(_allocator, vkImg, *allocHnd);
}

void 
Vk_Allocator::mapMem(void** outData, Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(outData || allocHnd, "");
	vmaMapMemory(_allocator, *allocHnd, outData);
}

void 
Vk_Allocator::mapMem(u8** outData, Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(allocHnd, "");
	mapMem(reinCast<void**>(outData), allocHnd);
}

void 
Vk_Allocator::unmapMem(Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(allocHnd, "");
	vmaUnmapMemory(_allocator, *allocHnd);
}

void 
Vk_Allocator::setAllocationDebugName(const char* name, Vk_AllocHnd* allocHnd)
{
	RDS_CORE_ASSERT(allocHnd, "");
	vmaSetAllocationName(_allocator, *allocHnd, name);
}

VmaMemoryUsage 
Vk_Allocator::toMemoryUsage(RenderMemoryUsage v)
{
	using SRC = RenderMemoryUsage;
	switch (v)
	{
		case SRC::Auto:				{ return VMA_MEMORY_USAGE_AUTO;  }					break;
		case SRC::AutoPreferCpu:	{ return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;  }		break;
		case SRC::AutoPreferGpu:	{ return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;  }	break;

		case SRC::CpuToGpu:			{ return VMA_MEMORY_USAGE_CPU_TO_GPU;  }	break;
		case SRC::CpuOnly:			{ return VMA_MEMORY_USAGE_CPU_ONLY;  }		break;

		case SRC::GpuToCpu:			{ return VMA_MEMORY_USAGE_GPU_TO_CPU;  }	break;
		case SRC::GpuOnly:			{ return VMA_MEMORY_USAGE_GPU_ONLY;  }		break;
	}
	RDS_CORE_ASSERT(false, "unsupported memory usage");
	return VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN;
}

VmaAllocationCreateFlags
Vk_Allocator::toAllocFlags(RenderAllocFlags v)
{
	using SRC = RenderAllocFlags;
	using DST = VmaAllocationCreateFlagBits;
	VmaAllocationCreateFlags flags = {};

	if (BitUtil::has(v, SRC::HostWrite))		{ flags |= DST::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; }
	if (BitUtil::has(v, SRC::PersistentMapped))	{ flags |= DST::VMA_ALLOCATION_CREATE_MAPPED_BIT; }

	return flags;
}


#endif

}

#endif