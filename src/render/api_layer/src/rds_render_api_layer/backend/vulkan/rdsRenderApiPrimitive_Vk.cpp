#include "rds_render_api_layer-pch.h"
#include "rdsRenderApiPrimitive_Vk.h"
#include "rdsRenderer_Vk.h"
#include "rdsAllocator_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- Vk_Instance-Impl ---
#endif // 0
#if 1

//void RenderApiPrimitive_Vk<Vk_Instance_T>::destroy()
//{
//	vkDestroyInstance(_p, MemoryContext_Vk::instance()->allocCallbacks());
//}

void Vk_Instance::create(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator)
{
	VkResult ret = vkCreateInstance(pCreateInfo, pAllocator, &_hnd);

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


}

#endif


#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_DebugUtilsMessenger>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	auto func = renderer->extInfo().getInstanceExtFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
	func(renderer->vkInstance(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_PhysicalDevice>::destroy()
{

}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Device>-Impl ---
#endif // 0
#if 1

void RenderApiPrimitive_Vk<Vk_Device>::destroy()
{
	vkDestroyDevice(_p, MemoryContext_Vk::instance()->allocCallbacks());
}
#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Surface>-Impl ---
#endif // 0
#if 1

void
RenderApiPrimitive_Vk<Vk_Surface>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroySurfaceKHR(renderer->vkInstance(), _p, renderer->allocCallbacks());
}

#endif


#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Queue>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_Queue>::destroy()
{

}

#endif


#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Swapchain>-Impl ---
#endif // 0
#if 1

void
RenderApiPrimitive_Vk<Vk_Swapchain>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroySwapchainKHR(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Image>-Impl ---
#endif // 0
#if 1

void
RenderApiPrimitive_Vk<Vk_Image>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyImage(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_ImageView>-Impl ---
#endif // 0
#if 1

void
RenderApiPrimitive_Vk<Vk_ImageView>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyImageView(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Framebuffer>-Impl ---
#endif // 0
#if 1

void
RenderApiPrimitive_Vk<Vk_Framebuffer>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyFramebuffer(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_ShaderModule>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_ShaderModule>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyShaderModule(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_RenderPass>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_RenderPass>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyRenderPass(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_PipelineLayout>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_PipelineLayout>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyPipelineLayout(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Pipeline>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_Pipeline>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyPipeline(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandPool_T>-Impl ---
#endif // 0
#if 0

void 
RenderApiPrimitive_Vk<Vk_CommandPool_T>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyCommandPool(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandBuffer_T>-Impl ---
#endif // 0
#if 0

void 
RenderApiPrimitive_Vk<Vk_CommandBuffer_T>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkFreeCommandBuffers(renderer->vkDevice(), _p, renderer->allocCallbacks()); 
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Semaphore>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_Semaphore>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroySemaphore(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Fence>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_Fence>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyFence(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif
#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_DeviceMemory>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_DeviceMemory>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkFreeMemory(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Buffer>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_Buffer>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkDestroyBuffer(renderer->vkDevice(), _p, renderer->allocCallbacks());
	_alloc->freeBuf(_p, _internal_allocHnd());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_BufferView>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_BufferView>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkDestroyFence(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#endif

}

#endif