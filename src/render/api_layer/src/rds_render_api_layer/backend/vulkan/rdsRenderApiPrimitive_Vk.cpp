#include "rds_render_api_layer-pch.h"
#include "rdsRenderApiPrimitive_Vk.h"
#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Instance>-Impl ---
#endif // 0
#if 1

void RenderApiPrimitive_Vk<Vk_Instance>::destroy()
{
	vkDestroyInstance(_p, MemoryContext_Vk::instance()->allocCallbacks());
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
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(renderer->extInfo().getExtFunction("vkDestroyDebugUtilsMessengerEXT"));
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
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandPool>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_CommandPool>::destroy()
{
	auto* renderer = Renderer_Vk::instance();
	vkDestroyCommandPool(renderer->vkDevice(), _p, renderer->allocCallbacks());
}

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandBuffer>-Impl ---
#endif // 0
#if 1

void 
RenderApiPrimitive_Vk<Vk_CommandBuffer>::destroy()
{
	//auto* renderer = Renderer_Vk::instance();
	//vkDestroyPipeline(renderer->vkDevice(), _p, renderer->allocCallbacks());
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

#endif

}

#endif