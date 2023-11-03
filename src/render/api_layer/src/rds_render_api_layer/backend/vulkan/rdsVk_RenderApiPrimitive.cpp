#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderApiPrimitive.h"
#include "rdsRenderer_Vk.h"
#include "rdsVk_Allocator.h"

#include "texture/rdsTexture_Vk.h"

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

//void Vk_RenderApiPrimitive<Vk_Instance_T>::destroy()
//{
//	vkDestroyInstance(_p, Vk_MemoryContext::instance()->allocCallbacks());
//}

void Vk_Instance::create(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator)
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

void Vk_Instance::destroy()
{
	if (!_hnd)
		return;

	vkDestroyInstance(hnd(), Renderer_Vk::instance()->allocCallbacks());
}

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

void 
Vk_RenderApiPrimitive<Vk_DebugUtilsMessenger>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	auto func = renderer->extInfo().getInstanceExtFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
	func(renderer->vkInstance(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

void 
Vk_RenderApiPrimitive<Vk_PhysicalDevice>::destroy()
{

}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Device>-Impl ---
#endif // 0
#if 1

void Vk_RenderApiPrimitive<Vk_Device>::destroy()
{
	vkDestroyDevice(_p, Vk_MemoryContext::instance()->allocCallbacks());
}
#endif

#if 0
#pragma mark --- rdsVk_Surface-Impl ---
#endif // 0
#if 1

void 
Vk_Surface::create(NativeUIWindow* wnd)
{
	if (!wnd)
		return;

	auto* vkInst			= Renderer_Vk::instance()->vkInstance();
	auto* vkAllocCallbacks	= Renderer_Vk::instance()->allocCallbacks();

#if RDS_OS_WINDOWS

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd			= wnd->wndHnd();
	createInfo.hinstance	= ::GetModuleHandle(nullptr);		// get handle of current process

#else
	#error("createSurface() not support int this platform")
#endif // RDS_OS_WINDOWS

	create(wnd, vkInst, &createInfo, vkAllocCallbacks);
}

void 
Vk_Surface::create(NativeUIWindow* wnd, Vk_Instance_T* instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator)
{
	if (wnd == _wnd || !wnd)
		return;

	destroy(wnd);

	_wnd = wnd;
	auto ret = vkCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Surface::destroy(NativeUIWindow* wnd)
{
	if (!hnd() || _wnd == wnd)
		return;

	auto* renderer = Renderer_Vk::instance();
	vkDestroySurfaceKHR(renderer->vkInstance(), hnd(), renderer->allocCallbacks());

	_hnd = VK_NULL_HANDLE;
	_wnd = nullptr;
}


#else

void
Vk_RenderApiPrimitive<Vk_Surface>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroySurfaceKHR(renderer->vkInstance(), _p, renderer->allocCallbacks());
}

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Queue>-Impl ---
#endif // 0
#if 1

//void 
//Vk_RenderApiPrimitive<Vk_Queue>::destroy()
//{
//
//}

void 
Vk_Queue::create(u32 familyIdx, Vk_Device* vkDevice)
{
	vkGetDeviceQueue(vkDevice, familyIdx, _queueIdx, hndForInit());
	_familyIdx = familyIdx;
}

void 
Vk_Queue::destroy()
{
	_familyIdx	= ~u32(0);
	_queueIdx	= 0;
}

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Swapchain>-Impl ---
#endif // 0
#if 0

void
Vk_RenderApiPrimitive<Vk_Swapchain>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroySwapchainKHR(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_Image-Impl ---
#endif // 0
#if 1

//void
//Vk_RenderApiPrimitive<Vk_Image>::destroy()
//{
//	auto* renderer = Renderer_Vk::instance();
//	vkDestroyImage(renderer->vkDevice(), _p, renderer->allocCallbacks());
//}

void 
Vk_Image::create(Vk_Allocator* vkAlloc, const VkImageCreateInfo* imageInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	_internal_setAlloc(vkAlloc);
	auto ret = vkAlloc->allocImage(hndForInit(), &_allocHnd, imageInfo, allocInfo, vkMemPropFlags);
	Util::throwIfError(ret);
}

void 
Vk_Image::create(Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, u32 width, u32 height, VkFormat vkFormat, VkImageTiling vkTiling, VkImageUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags)
{
	auto& vkQueueIndices	= Renderer_Vk::instance()->queueFamilyIndices();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageInfo.extent.width	= width;
	imageInfo.extent.height	= height;
	imageInfo.extent.depth	= 1;
	imageInfo.mipLevels		= 1;
	imageInfo.arrayLayers	= 1;
	imageInfo.format		= vkFormat;
	imageInfo.tiling		= vkTiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage			= usage;
	imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples		= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags			= VkImageCreateFlagBits::VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; // Optional

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
	if (!_hnd || !_alloc)
		return;
	_alloc->freeImage(_hnd, &_allocHnd);
	reset();
}

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Impl ---
#endif // 0
#if 1

//void
//Vk_RenderApiPrimitive<Vk_ImageView>::destroy()
//{
//	auto* renderer = Renderer_Vk::instance();
//	vkDestroyImageView(renderer->vkDevice(), _p, renderer->allocCallbacks());
//}

void 
Vk_ImageView::create(VkImageViewCreateInfo* viewInfo)
{
	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
	auto* vkAllocCallBacks	= Renderer_Vk::instance()->allocCallbacks();

	auto ret = vkCreateImageView(vkDev, viewInfo, vkAllocCallBacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_ImageView::create(Vk_Image* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount)
{
	create(vkImage->hnd(), vkFormat, aspectFlags, mipCount);
}

void 
Vk_ImageView::create(Vk_Image_T* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount)
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

	create(&viewInfo);
}


void 
Vk_ImageView::destroy()
{
	if (!_hnd)
		return;
	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
	auto* vkAllocCallBacks	= Renderer_Vk::instance()->allocCallbacks();
	vkDestroyImageView(vkDev, _hnd, vkAllocCallBacks);
	reset();
}


void 
Vk_ImageView::destroy(Renderer_Vk* rdr)
{
	if (!_hnd)
		return;
	auto* vkDev				= rdr->vkDevice();
	auto* vkAllocCallBacks	= rdr->allocCallbacks();
	vkDestroyImageView(vkDev, _hnd, vkAllocCallBacks);
	reset();
}

#endif


#if 0
#pragma mark --- rdsVk_Sampler-Impl ---
#endif // 0
#if 1

void 
Vk_Sampler::create(VkSamplerCreateInfo* samplerInfo)
{
	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
	auto* vkAllocCallBacks	= Renderer_Vk::instance()->allocCallbacks();

	auto ret = vkCreateSampler(vkDev, samplerInfo, vkAllocCallBacks, hndForInit());
	Util::throwIfError(ret);
}


void 
Vk_Sampler::destroy(Renderer_Vk* rdr)
{
	if (!_hnd)
		return;
	auto* vkDev				= rdr->vkDevice();
	auto* vkAllocCallBacks	= rdr->allocCallbacks();
	vkDestroySampler(vkDev, _hnd, vkAllocCallBacks);
	reset();
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Framebuffer>-Impl ---
#endif // 0
#if 1

//void
//Vk_RenderApiPrimitive<Vk_Framebuffer>::destroy()
//{
//	auto* renderer = Renderer_Vk::instance();
//	vkDestroyFramebuffer(renderer->vkDevice(), _p, renderer->allocCallbacks());
//}

void 
Vk_Framebuffer::create(const VkFramebufferCreateInfo* pCreateInfo)
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	auto ret = vkCreateFramebuffer(vkDev, pCreateInfo, vkAllocCallbacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Framebuffer::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyFramebuffer(renderer->vkDevice(), _hnd, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_ShaderModule-Impl ---
#endif // 0
#if 1

void 
Vk_ShaderModule::create(Renderer_Vk* rdr, StrView filename)
{
	_rdr = rdr;

	auto* vkDev			= _rdr->vkDevice();
	auto* vkAllocCbs	= _rdr->allocCallbacks();

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
Vk_ShaderModule::destroy()
{
	auto* vkDev			= _rdr->vkDevice();
	auto* vkAllocCbs	= _rdr->allocCallbacks();
	vkDestroyShaderModule(vkDev, hnd(), vkAllocCbs);
}

#endif

#if 0
#pragma mark --- rdsVk_RenderPass-Impl ---
#endif // 0
#if 1
//
//void 
//Vk_RenderApiPrimitive<Vk_RenderPass>::destroy()
//{
//	auto* renderer = Renderer_Vk::instance();
//	vkDestroyRenderPass(renderer->vkDevice(), _p, renderer->allocCallbacks());
//}

void 
Vk_RenderPass::create(const VkRenderPassCreateInfo* pCreateInfo)
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	auto ret = vkCreateRenderPass(vkDev, pCreateInfo, vkAllocCallbacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_RenderPass::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyRenderPass(renderer->vkDevice(), hnd(), renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_PipelineLayout-Impl ---
#endif // 0
#if 1

void 
Vk_PipelineLayout::create(Renderer_Vk* rdr, const VkPipelineLayoutCreateInfo* pCreateInfo)
{
	//auto* rdr			= Renderer_Vk::instance();
	auto* vkDev			= rdr->vkDevice();
	auto* vkAllocCbs	= rdr->allocCallbacks();

	auto ret = vkCreatePipelineLayout(vkDev, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_PipelineLayout::destroy()
{
	auto* rdr			= Renderer_Vk::instance();
	auto* vkDev			= rdr->vkDevice();
	auto* vkAllocCbs	= rdr->allocCallbacks();

	vkDestroyPipelineLayout(vkDev, hnd(), vkAllocCbs);
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
Vk_Pipeline::create(Renderer_Vk* rdr, const VkGraphicsPipelineCreateInfo* pCreateInfo, u32 infoCount, Vk_PipelineCache* vkPipelineCache)
{
	//auto* rdr			= Renderer_Vk::instance();
	auto* vkDev			= rdr->vkDevice();
	auto* vkAllocCbs	= rdr->allocCallbacks();

	auto cacheHnd = vkPipelineCache ? vkPipelineCache->hnd() : VK_NULL_HANDLE;

	auto ret = vkCreateGraphicsPipelines(vkDev, cacheHnd, infoCount, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Pipeline::destroy()
{
	auto* rdr			= Renderer_Vk::instance();
	auto* vkDev			= rdr->vkDevice();
	auto* vkAllocCbs	= rdr->allocCallbacks();

	vkDestroyPipeline(vkDev, hnd(), vkAllocCbs);
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_CommandPool_T>-Impl ---
#endif // 0
#if 0

void 
Vk_RenderApiPrimitive<Vk_CommandPool_T>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyCommandPool(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_CommandBuffer_T>-Impl ---
#endif // 0
#if 0

void 
Vk_RenderApiPrimitive<Vk_CommandBuffer_T>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkFreeCommandBuffers(renderer->vkDevice(), _p, renderer->allocCallbacks()); 
}

#endif

#if 0
#pragma mark --- rdsVk_Semaphore-Impl ---
#endif // 0
#if 1

void 
Vk_Semaphore::create()
{
	VkSemaphoreCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	create(&cInfo);
}

void 
Vk_Semaphore::create(const VkSemaphoreCreateInfo* pCreateInfo)
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	auto ret = vkCreateSemaphore(vkDev, pCreateInfo, vkAllocCallbacks, hndForInit());	
	Util::throwIfError(ret);
}

void 
Vk_Semaphore::destroy()
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	vkDestroySemaphore(vkDev, hnd(), vkAllocCallbacks);
}


#endif

#if 0
#pragma mark --- rdsVk_Fence-Impl ---
#endif // 0
#if 1

void 
Vk_Fence::create()
{
	VkFenceCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	cInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;			// create with signaled state

	create(&cInfo);
}

void 
Vk_Fence::create(const VkFenceCreateInfo* pCreateInfo)
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	auto ret = vkCreateFence(vkDev, pCreateInfo, vkAllocCallbacks, hndForInit());			
	Util::throwIfError(ret);
}

void 
Vk_Fence::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyFence(renderer->vkDevice(), hnd(), renderer->allocCallbacks());
}

#endif
#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_DeviceMemory>-Impl ---
#endif // 0
#if 1

void 
Vk_RenderApiPrimitive<Vk_DeviceMemory>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkFreeMemory(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsVk_Buffer-Impl ---
#endif // 0
#if 1

void 
Vk_Buffer::create(Vk_Allocator* alloc, const VkBufferCreateInfo* bufferInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags)
{
	_internal_setAlloc(alloc);
	auto ret = alloc->allocBuf(hndForInit(), &_allocHnd, bufferInfo, allocInfo, vkMemPropFlags);
	Util::throwIfError(ret);
}

void 
Vk_Buffer::create(Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags)
{
	auto& vkQueueIndices	= Renderer_Vk::instance()->queueFamilyIndices();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
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
	_alloc->freeBuf(_hnd, _internal_allocHnd());
}

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_BufferView>-Impl ---
#endif // 0
#if 1

void 
Vk_RenderApiPrimitive<Vk_BufferView>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkDestroyFence(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- Vk_DescriptorSetLayout-Impl ---
#endif // 0
#if 1

void 
Vk_DescriptorSetLayout::create(const VkDescriptorSetLayoutCreateInfo* pCreateInfo)
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	auto ret = vkCreateDescriptorSetLayout(vkDev, pCreateInfo, vkAllocCallbacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_DescriptorSetLayout::destroy()
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* vkAllocCallbacks	= renderer->allocCallbacks();

	vkDestroyDescriptorSetLayout(vkDev, _hnd, vkAllocCallbacks);
}

#endif

#if 0
#pragma mark --- Vk_DescriptorPool-Impl ---
#endif // 0
#if 1

Vk_DescriptorPool::~Vk_DescriptorPool()
{
	RDS_CORE_ASSERT(!hnd(), "not yet destroy");
}

VkResult 
Vk_DescriptorPool::create(VkDescriptorPoolCreateInfo* cInfo, Renderer_Vk* rdr)
{
	cInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

	auto* vkDev		= rdr->vkDevice();
	auto* vkAllocCb	= rdr->allocCallbacks();

	auto ret = vkCreateDescriptorPool(vkDev, cInfo, vkAllocCb, hndForInit());
	return ret;
	//Util::throwIfError(ret);	// should not throw, let upper layer decide
}

void 
Vk_DescriptorPool::destroy(Renderer_Vk* rdr)
{
	if (!_hnd || !rdr)
		return;
	auto* vkDev				= rdr->vkDevice();
	auto* vkAllocCallbacks	= rdr->allocCallbacks();

	vkDestroyDescriptorPool(vkDev, _hnd, vkAllocCallbacks);
	_hnd = VK_NULL_HANDLE;
}

void 
Vk_DescriptorPool::reset(VkDescriptorPoolResetFlags flag, Renderer_Vk* rdr)
{
	auto* vkDev	= rdr->vkDevice();
	vkResetDescriptorPool(vkDev, _hnd, flag);
}


#endif

#if 0
#pragma mark --- Vk_DescriptorPool-Impl ---
#endif // 0
#if 1

VkResult 
Vk_DescriptorSet::create(VkDescriptorSetAllocateInfo* cInfo)
{
	auto* rdr = Renderer_Vk::instance();
	auto ret = create(cInfo, rdr);
	return ret;
}

VkResult 
Vk_DescriptorSet::create(VkDescriptorSetAllocateInfo* cInfo, Renderer_Vk* rdr)
{
	cInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

	auto* vkDev	= rdr->vkDevice();
	auto ret = vkAllocateDescriptorSets(vkDev, cInfo, hndForInit());
	return ret;
	//Util::throwIfError(ret);	// should not throw, let upper layer decide
}

void 
Vk_DescriptorSet::destroy()
{
	
}

#endif

}

#endif

#endif