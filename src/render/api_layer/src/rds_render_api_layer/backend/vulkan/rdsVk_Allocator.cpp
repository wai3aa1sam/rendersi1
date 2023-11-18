#include "rds_render_api_layer-pch.h"
#include "rdsVk_Allocator.h"

#include "rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

RDS_DISABLE_ALL_WARNINGS();

#define VMA_IMPLEMENTATION

	#if RDS_DEBUG
		#define VMA_DEBUG_MARGIN 16
		#define VMA_DEBUG_DETECT_CORRUPTION 1
		//#define VMA_DEBUG_LOG
		//#define VMA_DEBUG_LOG_FORMAT
	#endif // RDS_DEBUG

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
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.instance				= vkInst;
	allocatorInfo.physicalDevice		= vkPhyDev;
	allocatorInfo.device				= vkDev;
	allocatorInfo.pAllocationCallbacks	= vkAllocCallbacks;
	vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void 
Vk_Allocator::destroy()
{
	vmaDestroyAllocator(_allocator);
	_allocator = nullptr;
}

VkResult
Vk_Allocator::allocBuf(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, const VkBufferCreateInfo* bufferInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	VmaAllocationCreateInfo vmaAllocCInfo = {};
	vmaAllocCInfo.usage			= toMemoryUsage(allocInfo->usage);
	vmaAllocCInfo.flags			= toAllocFlags(allocInfo->flags);
	vmaAllocCInfo.requiredFlags = vkMemPropFlags;
	
	auto ret = vmaCreateBuffer(_allocator, bufferInfo, &vmaAllocCInfo, outBuf, allocHnd, nullptr);
	Util::throwIfError(ret);
	
	return ret;
}

void 
Vk_Allocator::freeBuf(Vk_Buffer_T* vkBuf, Vk_AllocHnd* allocHnd)
{
	vmaDestroyBuffer(_allocator, vkBuf, *allocHnd);
}

VkResult	
Vk_Allocator::allocImage(Vk_Image_T** outImg, Vk_AllocHnd* allocHnd, const VkImageCreateInfo* imageInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
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
	vmaMapMemory(_allocator, *allocHnd, outData);
}

void 
Vk_Allocator::mapMem(u8** outData, Vk_AllocHnd* allocHnd)
{
	mapMem(reinCast<void**>(outData), allocHnd);
}

void 
Vk_Allocator::unmapMem(Vk_AllocHnd* allocHnd)
{
	vmaUnmapMemory(_allocator, *allocHnd);
}

VmaMemoryUsage 
Vk_Allocator::toMemoryUsage(RenderMemoryUsage v)
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
Vk_Allocator::toAllocFlags(RenderAllocFlags v)
{
	using SRC = RenderAllocFlags;
	using DST = VmaAllocationCreateFlagBits;
	VmaAllocationCreateFlags flags = {};

	if (BitUtil::has(v, SRC::HostWrite))	{ flags |= DST::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; }
	if (BitUtil::has(v, SRC::CreateMapped))	{ flags |= DST::VMA_ALLOCATION_CREATE_MAPPED_BIT; }

	return flags;
}

Vk_ScopedMemMapBuf::Vk_ScopedMemMapBuf(Vk_Buffer* vkBuf)
{
	_p = vkBuf;
	_p->_internal_alloc()->mapMem(&_data, _p->_internal_allocHnd());
}

Vk_ScopedMemMapBuf::~Vk_ScopedMemMapBuf()
{
	if (_p)
	{
		unmap();
	}
}

void Vk_ScopedMemMapBuf::unmap()
{
	_p->_internal_alloc()->unmapMem(_p->_internal_allocHnd());
	_p = nullptr;
}

#endif

}

#endif