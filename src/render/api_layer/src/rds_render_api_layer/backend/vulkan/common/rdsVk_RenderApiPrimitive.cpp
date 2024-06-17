#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderApiPrimitive.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- Vk_Instance-Impl ---
#endif // 0
#if 1

void 
Vk_Instance::create(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, RenderDevice_Vk* rdDevVk)
{
	VkResult ret = vkCreateInstance(pCreateInfo, pAllocator, hndForInit());

	/*
	Success
	VK_SUCCESS
	Failure
	VK_ERROR_OUT_OF_HOST_MEMORY
	VK_ERROR_OUT_OF_DEVICE_MEMORY
	VK_ERROR_INITIALIZATION_FAILED
	VK_ERROR_LAYER_NOT_PRESENT
	VK_ERROR_EXTENSION_NOT_PRESENT
	VK_ERROR_INCOMPATIBLE_DRIVER
	*/
	Util::throwIfError(ret);
}

void
Vk_Instance::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkAllocCbs = rdDevVk->allocCallbacks();

	vkDestroyInstance(hnd(), vkAllocCbs);
	Base::destroy();
}

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

void
Vk_DebugUtilsMessenger::create(RenderDevice_Vk* rdDevVk)
{
	const auto& vkExtInfo	= rdDevVk->extInfo();
	auto*		vkInst		= rdDevVk->vkInstance();
	auto*		vkAllocCbs	= rdDevVk->allocCallbacks();

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	Util::createDebugMessengerInfo(createInfo);

	auto fn		= vkExtInfo.getInstanceExtFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
	auto ret	= fn(vkInst, &createInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_DebugUtilsMessenger::destroy(RenderDevice_Vk* rdDevVk)
{
	if (!hnd())
		return;

	auto* vkInst		= rdDevVk->vkInstance();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto fn = rdDevVk->extInfo().getInstanceExtFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
	fn(vkInst, hnd(), vkAllocCbs);

	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

void 
Vk_PhysicalDevice::create(Vk_PhysicalDevice_T* vkPhyDevHnd)
{
	_hnd = vkPhyDevHnd;
}

void 
Vk_PhysicalDevice::destroy()
{
	RDS_CORE_ASSERT(hnd(), "");

	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Device>-Impl ---
#endif // 0
#if 1

void 
Vk_Device::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyDevice(hnd(), vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_Surface-Impl ---
#endif // 0
#if 1

void 
Vk_Surface::create(NativeUIWindow* wnd, RenderDevice_Vk* rdDevVk)
{
	if (!wnd)
		return;

	auto* vkInst		= rdDevVk->vkInstance();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

#if RDS_OS_WINDOWS

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd			= wnd->wndHnd();
	createInfo.hinstance	= ::GetModuleHandle(nullptr);		// get handle of current process

#else
	#error("createSurface() not support int this platform")
#endif // RDS_OS_WINDOWS

	create(wnd, vkInst, &createInfo, vkAllocCbs, rdDevVk);
}

void 
Vk_Surface::create(NativeUIWindow* wnd, Vk_Instance_T* instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, RenderDevice_Vk* rdDevVk)
{
	if (wnd == _wnd || !wnd)
		return;

	destroy(wnd, rdDevVk);

	_wnd = wnd;

	auto ret = vkCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Surface::destroy(NativeUIWindow* wnd, RenderDevice_Vk* rdDevVk)
{
	if (!hnd() || _wnd == wnd)
		return;

	vkDestroySurfaceKHR(rdDevVk->vkInstance(), hnd(), rdDevVk->allocCallbacks());

	_wnd = nullptr;
	Base::destroy();
}

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Queue>-Impl ---
#endif // 0
#if 1

void 
Vk_Queue::create(QueueTypeFlags type, RenderDevice_Vk* rdDevVk)
{
	create(rdDevVk->queueFamilyIndices().getFamilyIdx(type), rdDevVk->vkDevice());
}

void 
Vk_Queue::create(u32 familyIdx, Vk_Device_T* vkDevice)
{
	vkGetDeviceQueue(vkDevice, familyIdx, _queueIdx, hndForInit());
	_familyIdx = familyIdx;
}

void 
Vk_Queue::destroy()
{
	if (!hnd())
		return;

	_familyIdx	= ~u32(0);
	_queueIdx	= 0;
	Base::destroy();
}

void 
Vk_Queue::submit(const VkSubmitInfo2& submitInfo, Vk_Fence_T* signalFenceHnd, const RenderDebugLabel& debugLabel)
{
	beginDebugLabel(debugLabel.name, debugLabel.color);
	auto ret = vkQueueSubmit2(hnd(), 1, &submitInfo, signalFenceHnd);
	Util::throwIfError(ret);
	endDebugLabel();
}

void 
Vk_Queue::insertDebugLabel(const char* name, const Color4f& color)
{
	#if RDS_DEVELOPMENT
	VkDebugUtilsLabelEXT debugLabel = Util::toVkDebugUtilsLabel(name, color);
	vkQueueInsertDebugUtilsLabel(hnd(), &debugLabel);
	#endif
}

void 
Vk_Queue::beginDebugLabel(const char* name, const Color4f& color)
{
	#if RDS_DEVELOPMENT
	VkDebugUtilsLabelEXT debugLabel = Util::toVkDebugUtilsLabel(name, color);
	vkQueueBeginDebugUtilsLabel(hnd(), &debugLabel);
	#endif
}

void 
Vk_Queue::endDebugLabel()
{
	#if RDS_DEVELOPMENT
	vkQueueEndDebugUtilsLabel(hnd());
	#endif
}

#endif

#if 0
#pragma mark --- rdsVk_Image-Impl ---
#endif // 0
#if 1

void 
Vk_Image::create(Vk_Allocator* vkAlloc, const VkImageCreateInfo* imageInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	_internal_setAlloc(vkAlloc);
	auto ret = vkAlloc->allocImage(hndForInit(), &_allocHnd, imageInfo, allocInfo, vkMemPropFlags);
	Util::throwIfError(ret);
}


void 
Vk_Image::create(RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, u32 width, u32 height, VkFormat vkFormat, VkImageTiling vkTiling
				, u32 mipLevels, u32 layers
				, VkImageUsageFlags usage, QueueTypeFlags queueTypeFlags, VkImageCreateFlags createFlags, VkMemoryPropertyFlags vkMemPropFlags)
{
	RDS_CORE_ASSERT(width > 0 && height > 0, "");

	auto& vkQueueIndices = rdDevVk->queueFamilyIndices();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageInfo.extent.width	= width;
	imageInfo.extent.height	= height;
	imageInfo.extent.depth	= 1;
	imageInfo.mipLevels		= mipLevels;
	imageInfo.arrayLayers	= layers;
	imageInfo.format		= vkFormat;
	imageInfo.tiling		= vkTiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage			= usage;
	imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples		= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags			= createFlags; // Optional

	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
	if (queueCount > 1)
	{
		imageInfo.sharingMode			= VK_SHARING_MODE_CONCURRENT;
		imageInfo.queueFamilyIndexCount	= queueCount;
		imageInfo.pQueueFamilyIndices	= queueIdices.data();
	}

	create(vkAlloc, &imageInfo, allocInfo, vkMemPropFlags);
}

void 
Vk_Image::create(Vk_Image_T* vkImage)
{
	_hnd = vkImage;
}

void 
Vk_Image::destroy()
{
	if (_hnd && _alloc)
	{
		_alloc->freeImage(_hnd, &_allocHnd);
	}
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Impl ---
#endif // 0
#if 1

void 
Vk_ImageView::create(VkImageViewCreateInfo* viewInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev				= rdDevVk->vkDevice();
	auto* vkAllocCallBacks	= rdDevVk->allocCallbacks();

	auto ret = vkCreateImageView(vkDev, viewInfo, vkAllocCallBacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_ImageView::create(Vk_Image* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount, RenderDevice_Vk* rdDevVk)
{
	create(vkImage->hnd(), vkFormat, aspectFlags, mipCount, rdDevVk);
}

void 
Vk_ImageView::create(Vk_Image_T* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount, RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(mipCount >= 1, "");

	if (Util::hasStencilComponent(vkFormat))
	{
		aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image								= vkImage;
	viewInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format								= vkFormat;

	viewInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;

	viewInfo.subresourceRange.aspectMask		= aspectFlags;
	viewInfo.subresourceRange.baseMipLevel		= 0;
	viewInfo.subresourceRange.baseArrayLayer	= 0;
	viewInfo.subresourceRange.levelCount		= mipCount;
	viewInfo.subresourceRange.layerCount		= 1;

	create(&viewInfo, rdDevVk);
}

void 
Vk_ImageView::destroy(RenderDevice_Vk* rdDev)
{
	if (_hnd && rdDev)
	{
		auto* vkDev				= rdDev->vkDevice();
		auto* vkAllocCallBacks	= rdDev->allocCallbacks();

		vkDestroyImageView(vkDev, _hnd, vkAllocCallBacks);
	}

	Base::destroy();
}

#endif


#if 0
#pragma mark --- rdsVk_Sampler-Impl ---
#endif // 0
#if 1

void 
Vk_Sampler::create(VkSamplerCreateInfo* samplerInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev				= rdDevVk->vkDevice();
	auto* vkAllocCallBacks	= rdDevVk->allocCallbacks();

	auto ret = vkCreateSampler(vkDev, samplerInfo, vkAllocCallBacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Sampler::destroy(RenderDevice_Vk* rdDevVk)
{
	if (_hnd && rdDevVk)
	{
		auto* vkDev				= rdDevVk->vkDevice();
		auto* vkAllocCallBacks	= rdDevVk->allocCallbacks();

		vkDestroySampler(vkDev, _hnd, vkAllocCallBacks);
	}
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_ShaderModule-Impl ---
#endif // 0
#if 1

Vk_ShaderModule::~Vk_ShaderModule()
{

}

void 
Vk_ShaderModule::create(StrView filename, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	destroy(rdDevVk);

	Vector<u8> bin;
	File::readFile(filename, bin);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize	= bin.size();
	createInfo.pCode	= reinCast<const u32*>(bin.data());

	auto ret = vkCreateShaderModule(vkDev, &createInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_ShaderModule::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(rdDevVk, "rdDevVk == nullptr");
	//RDS_CORE_ASSERT(hnd(), "");
	if (!hnd())
	{
		return;
	}

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyShaderModule(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_PipelineLayout-Impl ---
#endif // 0
#if 1

void 
Vk_PipelineLayout::create(const VkPipelineLayoutCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreatePipelineLayout(vkDev, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_PipelineLayout::destroy(RenderDevice_Vk* rdDevVk)
{
	//RDS_CORE_ASSERT(hnd(), "");
	if (!hnd()) return;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyPipelineLayout(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_PipelineCache-Impl ---
#endif // 0
#if 1

void 
Vk_PipelineCache::destroy()
{
	RDS_NOT_YET_SUPPORT();
}

#endif

#if 0
#pragma mark --- rdsVk_Pipeline-Impl ---
#endif // 0
#if 1

void 
Vk_Pipeline::create(const VkGraphicsPipelineCreateInfo* pCreateInfo, Vk_PipelineCache* vkPipelineCache, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto cacheHnd = vkPipelineCache ? vkPipelineCache->hnd() : VK_NULL_HANDLE;

	auto ret = vkCreateGraphicsPipelines(vkDev, cacheHnd, 1, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Pipeline::create(const VkComputePipelineCreateInfo* pCreateInfo, Vk_PipelineCache* vkPipelineCache, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto cacheHnd = vkPipelineCache ? vkPipelineCache->hnd() : VK_NULL_HANDLE;

	auto ret = vkCreateComputePipelines(vkDev, cacheHnd, 1, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Pipeline::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyPipeline(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_Semaphore-Impl ---
#endif // 0
#if 1

void 
Vk_Semaphore::create(RenderDevice_Vk* rdDevVk)
{
	VkSemaphoreCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	create(&cInfo, rdDevVk);
}

void 
Vk_Semaphore::create(const VkSemaphoreCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateSemaphore(vkDev, pCreateInfo, vkAllocCbs, hndForInit());	
	Util::throwIfError(ret);
}

void 
Vk_Semaphore::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroySemaphore(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}


#endif

#if 0
#pragma mark --- rdsVk_Fence-Impl ---
#endif // 0
#if 1

void 
Vk_Fence::create(RenderDevice_Vk* rdDevVk)
{
	VkFenceCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	cInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;			// create with signaled state

	create(&cInfo, rdDevVk);
}

void 
Vk_Fence::create(const VkFenceCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateFence(vkDev, pCreateInfo, vkAllocCbs, hndForInit());			
	Util::throwIfError(ret);
}

void 
Vk_Fence::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyFence(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}

VkResult 
Vk_Fence::wait(RenderDevice_Vk* rdDev, u64 timeout)
{
	Vk_Fence_T* vkFences[] = { hnd() };
	u32 vkFenceCount = ArraySize<decltype(vkFences)>;

	auto ret = vkWaitForFences(rdDev->vkDevice(), vkFenceCount, vkFences, VK_TRUE, timeout);
	Util::throwIfError(ret);
	return ret;
}

VkResult
Vk_Fence::reset(RenderDevice_Vk* rdDev)
{
	Vk_Fence_T* vkFences[]		= { hnd() };
	u32			vkFenceCount	= ArraySize<decltype(vkFences)>;

	auto ret = vkResetFences(rdDev->vkDevice(), vkFenceCount, vkFences);		// should handle it to signaled if the function throw?
	Util::throwIfError(ret);
	return ret;
}

VkResult 
Vk_Fence::status(RenderDevice_Vk* rdDev) const
{
	RDS_CORE_ASSERT(rdDev && hnd(), "rdDev && hnd()");
	return vkGetFenceStatus(rdDev->vkDevice(), hnd());
}

bool 
Vk_Fence::isSignaled(RenderDevice_Vk* rdDev) const
{
	auto ret = status(rdDev);
	
	if (ret == VkResult::VK_SUCCESS || ret == VkResult::VK_NOT_READY)
	{
		return ret == VkResult::VK_SUCCESS;
	}

	Util::throwIfError(ret);
	return false;
}


#endif

#if 0
#pragma mark --- rdsVk_DeviceMemory-Impl ---
#endif // 0
#if 1

void 
Vk_DeviceMemory::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkFreeMemory(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- rdsVk_ScopedMemMapBuf-Impl ---
#endif // 0
#if 1

Vk_ScopedMemMapBuf::Vk_ScopedMemMapBuf(Vk_Buffer* vkBuf)
{
	RDS_CORE_ASSERT(vkBuf, "");
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

#if 0
#pragma mark --- rdsVk_Buffer-Impl ---
#endif // 0
#if 1

void 
Vk_Buffer::create(Vk_Allocator* alloc, const VkBufferCreateInfo* bufferInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	_internal_setAlloc(alloc);
	auto ret = alloc->allocBuf(hndForInit(), &_allocHnd, bufferInfo, allocInfo, vkMemPropFlags);
	Util::throwIfError(ret);
}

void 
Vk_Buffer::create(RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags)
{
	auto& vkQueueIndices	= rdDevVk->queueFamilyIndices();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;

	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
	if (queueCount > 1)
	{
		bufferInfo.sharingMode				= VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount	= queueCount;
		bufferInfo.pQueueFamilyIndices		= queueIdices.data();
	}
	create(vkAlloc, &bufferInfo, allocInfo, vkMemPropFlags);
}

void 
Vk_Buffer::destroy()
{
	if (isInvalid())
	{
		return;
	}
	_alloc->freeBuf(_hnd, _internal_allocHnd());
	Base::destroy();
}

Vk_ScopedMemMapBuf 
Vk_Buffer::scopedMemMap()
{
	return Vk_ScopedMemMapBuf{ this };
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_BufferView>-Impl ---
#endif // 0
#if 1

void 
Vk_BufferView::create	(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
Vk_BufferView::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

}

#endif

#if 0
#pragma mark --- Vk_DescriptorSetLayout-Impl ---
#endif // 0
#if 1

void 
Vk_DescriptorSetLayout::create(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateDescriptorSetLayout(vkDev, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_DescriptorSetLayout::destroy(RenderDevice_Vk* rdDevVk)
{
	if (!hnd())
		return;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyDescriptorSetLayout(vkDev, _hnd, vkAllocCbs);
	Base::destroy();
}

#endif

#if 0
#pragma mark --- Vk_DescriptorPool-Impl ---
#endif // 0
#if 1

VkResult 
Vk_DescriptorPool::create(VkDescriptorPoolCreateInfo* cInfo, RenderDevice_Vk* rdDevVk)
{
	cInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateDescriptorPool(vkDev, cInfo, vkAllocCbs, hndForInit());
	return ret;
}

void 
Vk_DescriptorPool::destroy(RenderDevice_Vk* rdDevVk)
{
	if (!_hnd || !rdDevVk)
		return;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyDescriptorPool(vkDev, _hnd, vkAllocCbs);
	Base::destroy();
}

void 
Vk_DescriptorPool::reset(VkDescriptorPoolResetFlags flag, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev	= rdDevVk->vkDevice();
	vkResetDescriptorPool(vkDev, _hnd, flag);
}


#endif

#if 0
#pragma mark --- Vk_DescriptorPool-Impl ---
#endif // 0
#if 1

Vk_DescriptorSet::~Vk_DescriptorSet()
{
	destroy();
}

VkResult 
Vk_DescriptorSet::create(VkDescriptorSetAllocateInfo* cInfo, RenderDevice_Vk* rdDevVk)
{
	cInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

	auto* vkDev	= rdDevVk->vkDevice();
	auto ret = vkAllocateDescriptorSets(vkDev, cInfo, hndForInit());
	return ret;
}

void 
Vk_DescriptorSet::destroy()
{
	Base::destroy();
}

#endif

}

#endif

#endif