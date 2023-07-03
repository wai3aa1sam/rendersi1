#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderApi_Include_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

using Vk_Instance				= VkInstance_T;
using Vk_PhysicalDevice			= VkPhysicalDevice_T;
using Vk_Device					= VkDevice_T;
using Vk_Queue					= VkQueue_T;
using Vk_Surface				= VkSurfaceKHR_T;
using Vk_Swapchain				= VkSwapchainKHR_T;
using Vk_Framebuffer			= VkFramebuffer_T;

using Vk_RenderPass				= VkRenderPass_T;
using Vk_Pipeline				= VkPipeline_T;
using Vk_PipelineLayout			= VkPipelineLayout_T;
using Vk_PipelineCache			= VkPipelineCache_T;
using Vk_DescriptorSet			= VkDescriptorSet_T;
using Vk_DescriptorSetLayout	= VkDescriptorSetLayout_T;
using Vk_ShaderModule			= VkShaderModule_T;

using Vk_CommandPool			= VkCommandPool_T;
using Vk_CommandBuffer			= VkCommandBuffer_T;

using Vk_Semaphore				= VkSemaphore_T;
using Vk_Fence					= VkFence_T;

using Vk_Buffer					= VkBuffer_T;
using Vk_BufferView				= VkBufferView_T;
using Vk_Image					= VkImage_T;
using Vk_Sampler				= VkSampler_T;
using Vk_ImageView				= VkImageView_T;
using Vk_DeviceMemory			= VkDeviceMemory_T;

using Vk_DebugUtilsMessenger	= VkDebugUtilsMessengerEXT_T;

#if 0
#pragma mark --- rdsRenderApiPrmivitive_Vk-Decl ---
#endif // 0
#if 1

template<class T>
class RenderApiPrimitive_Base_Vk : public RefCount_Base
{
public:
	RenderApiPrimitive_Base_Vk()	= default;
	~RenderApiPrimitive_Base_Vk()	= default;

	void destroy();

protected:
	T* _p = VK_NULL_HANDLE;
};

template<class T> class RenderApiPrimitive_Vk;

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Instance>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Instance> : public RenderApiPrimitive_Base_Vk<Vk_Instance>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_DebugUtilsMessenger> : public RenderApiPrimitive_Base_Vk<Vk_DebugUtilsMessenger>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_PhysicalDevice> : public RenderApiPrimitive_Base_Vk<Vk_PhysicalDevice>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Device>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Device> : public RenderApiPrimitive_Base_Vk<Vk_Device>
{
public:
	void destroy();
};

#endif


#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Surface>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Surface> : public RenderApiPrimitive_Base_Vk<Vk_Surface>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Queue>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Queue> : public RenderApiPrimitive_Base_Vk<Vk_Queue>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Swapchain>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Swapchain> : public RenderApiPrimitive_Base_Vk<Vk_Swapchain>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Image>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Image> : public RenderApiPrimitive_Base_Vk<Vk_Image>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_ImageView>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_ImageView> : public RenderApiPrimitive_Base_Vk<Vk_ImageView>
{
public:
	void destroy();
};

#endif


#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Framebuffer>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Framebuffer> : public RenderApiPrimitive_Base_Vk<Vk_Framebuffer>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_ShaderModule>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_ShaderModule> : public RenderApiPrimitive_Base_Vk<Vk_ShaderModule>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_RenderPass>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_RenderPass> : public RenderApiPrimitive_Base_Vk<Vk_RenderPass>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_PipelineLayout>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_PipelineLayout> : public RenderApiPrimitive_Base_Vk<Vk_PipelineLayout>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Pipeline>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Pipeline> : public RenderApiPrimitive_Base_Vk<Vk_Pipeline>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandPool>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_CommandPool> : public RenderApiPrimitive_Base_Vk<Vk_CommandPool>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_CommandBuffer>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_CommandBuffer> : public RenderApiPrimitive_Base_Vk<Vk_CommandBuffer>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Semaphore>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Semaphore> : public RenderApiPrimitive_Base_Vk<Vk_Semaphore>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsRenderApiPrimitive_Vk<Vk_Fence>-Impl ---
#endif // 0
#if 1

template<>
class RenderApiPrimitive_Vk<Vk_Fence> : public RenderApiPrimitive_Base_Vk<Vk_Fence>
{
public:
	void destroy();
};

#endif


#endif


}
#endif