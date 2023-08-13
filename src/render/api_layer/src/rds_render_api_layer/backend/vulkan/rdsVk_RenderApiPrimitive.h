#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderApi_Include_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

using Vk_Instance_T				= VkInstance_T;
using Vk_PhysicalDevice			= VkPhysicalDevice_T;
using Vk_Device					= VkDevice_T;
using Vk_Queue_T				= VkQueue_T;
using Vk_Surface				= VkSurfaceKHR_T;
using Vk_Swapchain				= VkSwapchainKHR_T;
using Vk_Framebuffer_T			= VkFramebuffer_T;

using Vk_RenderPass_T			= VkRenderPass_T;
using Vk_Pipeline				= VkPipeline_T;
using Vk_PipelineLayout			= VkPipelineLayout_T;
using Vk_PipelineCache			= VkPipelineCache_T;
using Vk_DescriptorSet_T		= VkDescriptorSet_T;
using Vk_DescriptorSetLayout_T	= VkDescriptorSetLayout_T;
using Vk_DescriptorPool_T		= VkDescriptorPool_T;
using Vk_ShaderModule			= VkShaderModule_T;

using Vk_CommandPool_T			= VkCommandPool_T;
using Vk_CommandBuffer_T		= VkCommandBuffer_T;

using Vk_Semaphore_T			= VkSemaphore_T;
using Vk_Fence_T				= VkFence_T;

using Vk_Buffer_T				= VkBuffer_T;
using Vk_BufferView_T			= VkBufferView_T;
using Vk_Image_T				= VkImage_T;
using Vk_Sampler_T				= VkSampler_T;
using Vk_ImageView_T			= VkImageView_T;
using Vk_DeviceMemory			= VkDeviceMemory_T;

using Vk_DebugUtilsMessenger	= VkDebugUtilsMessengerEXT_T;

struct RenderApiUtil_Vk;

template<class T>
class RenderApiResource_Vk : public NonCopyable
{
public:
	using Util = RenderApiUtil_Vk;

public:
	RenderApiResource_Vk()	= default;
	~RenderApiResource_Vk() = default;

	void destroy();

	T* hnd() { return _hnd; }
	T** hndForInit() { return &_hnd; }

protected:
	T* _hnd = VK_NULL_HANDLE;
};


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

template<class T> 
class Vk_RenderApiPrimitive : public NonCopyable
{
public:
	using Util = RenderApiUtil_Vk;

	using HndType = T;

public:
	Vk_RenderApiPrimitive()		= default;
	~Vk_RenderApiPrimitive()	= default;

	void destroy();

				T* hnd()		{ return _hnd; }
	/*const*/	T* hnd() const	{ return _hnd; }

	T** hndForInit() { return &_hnd; }

protected:
	T* _hnd = VK_NULL_HANDLE;
};

#endif

#if 0

#endif // 0
#if 1

class  Vk_Allocator;
struct Vk_AllocInfo;
template<class T>
class Vk_AllocableRenderApiPrimitive : public Vk_RenderApiPrimitive<T>
{
public:
	Vk_AllocHnd*	_internal_allocHnd();

	void			_internal_setAlloc(Vk_Allocator* alloc);
	Vk_Allocator*	_internal_alloc();

protected:
	Vk_Allocator*	_alloc		= nullptr;	// TODO: AllocablePrimitive_Vk<T>::Vk_Allocator* should be strong ref?
	Vk_AllocHnd		_allocHnd	= {};
};

template<class T> inline Vk_AllocHnd*	Vk_AllocableRenderApiPrimitive<T>::_internal_allocHnd()						{ return &_allocHnd; }

template<class T> inline void			Vk_AllocableRenderApiPrimitive<T>::_internal_setAlloc(Vk_Allocator* alloc)	{ _alloc = alloc; }
template<class T> inline Vk_Allocator*	Vk_AllocableRenderApiPrimitive<T>::_internal_alloc()						{ return _alloc; }


#endif // 1


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive-Impl ---
#endif // 0
#if 1

#if 0
#pragma mark --- Vk_Instance-Decl ---
#endif // 0
#if 1

class Vk_Instance : public Vk_RenderApiPrimitive<Vk_Instance_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Instance_T>;

public:
	Vk_Instance() = default;
	~Vk_Instance() { destroy(); }

	void create(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_DebugUtilsMessenger>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_DebugUtilsMessenger> : public RenderApiPrimitive_Base_Vk<Vk_DebugUtilsMessenger>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_PhysicalDevice>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_PhysicalDevice> : public RenderApiPrimitive_Base_Vk<Vk_PhysicalDevice>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Device>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_Device> : public RenderApiPrimitive_Base_Vk<Vk_Device>
{
public:
	void destroy();
};

#endif


#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Surface>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_Surface> : public RenderApiPrimitive_Base_Vk<Vk_Surface>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Queue>-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_Queue> : public RenderApiPrimitive_Base_Vk<Vk_Queue>
//{
//public:
//	void destroy();
//};

class Vk_Queue : public Vk_RenderApiPrimitive<Vk_Queue_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Queue_T>;

public:
	Vk_Queue() = default;
	~Vk_Queue() { destroy(); }

	Vk_Queue(Vk_Queue&&)		{ throwIf(true, ""); }
	void operator=(Vk_Queue&&)	{ throwIf(true, ""); }

	void create(u32 familyIdx, Vk_Device* vkDevice);
	void destroy();

	u32 familyIdx() const;
	u32 queueIdx()	const;

protected:
	u32 _familyIdx = ~u32(0);
	u32 _queueIdx  = 0;
};

inline u32 Vk_Queue::familyIdx()	const { return _familyIdx; }
inline u32 Vk_Queue::queueIdx()		const { return _queueIdx; }


#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Swapchain>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_Swapchain> : public RenderApiPrimitive_Base_Vk<Vk_Swapchain>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Image-Impl ---
#endif // 0
#if 1

class Vk_Image : public Vk_AllocableRenderApiPrimitive<Vk_Image_T>
{
public:
	using Base = Vk_AllocableRenderApiPrimitive<Vk_Image_T>;

public:
	Vk_Image() = default;
	~Vk_Image() { destroy(); }

	Vk_Image(Vk_Image&&)		{ throwIf(true, ""); }
	void operator=(Vk_Image&&)	{ throwIf(true, ""); }

	void create(Vk_Allocator* vkAlloc, const VkImageCreateInfo* imageInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create(Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, u32 width, u32 height, VkFormat vkFormat, VkImageTiling vkTiling, VkImageUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create(Vk_Image_T* vkImage);

	void destroy();
};


//template<>
//class Vk_RenderApiPrimitive<Vk_Image> : public RenderApiPrimitive_Base_Vk<Vk_Image>
//{
//public:
//	void destroy();
//};

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_ImageView> : public RenderApiPrimitive_Base_Vk<Vk_ImageView>
//{
//public:
//	void destroy();
//};

class Vk_ImageView : public Vk_RenderApiPrimitive<Vk_ImageView_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_ImageView_T>;

public:
	Vk_ImageView() = default;
	~Vk_ImageView() { destroy(); }

	Vk_ImageView(Vk_ImageView&&)		{ throwIf(true, ""); }
	void operator=(Vk_ImageView&&)	{ throwIf(true, ""); }

	void create(VkImageViewCreateInfo* viewInfo);
	void create(Vk_Image*   vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount = 1);
	void create(Vk_Image_T* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount = 1);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_ImageView> : public RenderApiPrimitive_Base_Vk<Vk_ImageView>
//{
//public:
//	void destroy();
//};

class Vk_Sampler : public Vk_RenderApiPrimitive<Vk_Sampler_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Sampler_T>;

public:
	Vk_Sampler() = default;
	~Vk_Sampler() { destroy(); }

	Vk_Sampler(Vk_Sampler&&)		{ throwIf(true, ""); }
	void operator=(Vk_Sampler&&)	{ throwIf(true, ""); }

	void create(VkSamplerCreateInfo* samplerInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Framebuffer-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_Framebuffer> : public RenderApiPrimitive_Base_Vk<Vk_Framebuffer>
//{
//public:
//	void destroy();
//};

class Vk_Framebuffer : public Vk_RenderApiPrimitive<Vk_Framebuffer_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Framebuffer_T>;

public:
	Vk_Framebuffer() = default;
	~Vk_Framebuffer() { destroy(); }

	Vk_Framebuffer(Vk_Framebuffer&&)	{ throwIf(true, ""); }
	void operator=(Vk_Framebuffer&&)	{ throwIf(true, ""); }

	void create(const VkFramebufferCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_ShaderModule>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_ShaderModule> : public RenderApiPrimitive_Base_Vk<Vk_ShaderModule>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderPass-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_RenderPass> : public RenderApiPrimitive_Base_Vk<Vk_RenderPass>
//{
//public:
//	void destroy();
//};

class Vk_RenderPass : public Vk_RenderApiPrimitive<Vk_RenderPass_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_RenderPass_T>;

public:
	Vk_RenderPass() = default;
	~Vk_RenderPass() { destroy(); }

	Vk_RenderPass(Vk_RenderPass&&)	{ throwIf(true, ""); }
	void operator=(Vk_RenderPass&&)	{ throwIf(true, ""); }

	void create(const VkRenderPassCreateInfo* pCreateInfo);
	void destroy();
};


#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_PipelineLayout>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_PipelineLayout> : public RenderApiPrimitive_Base_Vk<Vk_PipelineLayout>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_Pipeline>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_Pipeline> : public RenderApiPrimitive_Base_Vk<Vk_Pipeline>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_CommandPool_T>-Impl ---
#endif // 0
#if 0

template<>
class Vk_RenderApiPrimitive<Vk_CommandPool_T> : public RenderApiPrimitive_Base_Vk<Vk_CommandPool_T>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_CommandBuffer_T>-Impl ---
#endif // 0
#if 0

template<>
class Vk_RenderApiPrimitive<Vk_CommandBuffer_T> : public RenderApiPrimitive_Base_Vk<Vk_CommandBuffer_T>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Semaphore-Impl ---
#endif // 0
#if 1


class Vk_Semaphore : public Vk_RenderApiPrimitive<Vk_Semaphore_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Semaphore_T>;

public:
	Vk_Semaphore() = default;
	~Vk_Semaphore() { destroy(); }

	Vk_Semaphore(Vk_Semaphore&&)	{ throwIf(true, ""); }
	void operator=(Vk_Semaphore&&)	{ throwIf(true, ""); }

	void create();
	void create(const VkSemaphoreCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Fence-Impl ---
#endif // 0
#if 1

class Vk_Fence : public Vk_RenderApiPrimitive<Vk_Fence_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Fence_T>;

public:
	Vk_Fence() = default;
	~Vk_Fence() { destroy(); }

	Vk_Fence(Vk_Fence&&)		{ throwIf(true, ""); }
	void operator=(Vk_Fence&&)	{ throwIf(true, ""); }

	void create();
	void create(const VkFenceCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_DeviceMemory>-Impl ---
#endif // 0
#if 1

template<>
class Vk_RenderApiPrimitive<Vk_DeviceMemory> : public RenderApiPrimitive_Base_Vk<Vk_DeviceMemory>
{
public:
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Buffer-Decl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_Buffer> : public AllocablePrimitive_Vk<Vk_Buffer>
//{
//public:
//	void destroy();
//};


class Vk_Buffer : public Vk_AllocableRenderApiPrimitive<Vk_Buffer_T>
{
public:
	using Base = Vk_AllocableRenderApiPrimitive<Vk_Buffer_T>;

public:
	Vk_Buffer() = default;
	~Vk_Buffer() { destroy(); }

	Vk_Buffer(Vk_Buffer&&)		{ throwIf(true, ""); }
	void operator=(Vk_Buffer&&) { throwIf(true, ""); }

	void create(Vk_Allocator* alloc, const VkBufferCreateInfo* bufferInfo, const Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create(Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void destroy();
};


#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_BufferView>-Impl ---
#endif // 0
#if 1

//template<>
//class Vk_RenderApiPrimitive<Vk_BufferView> : public RenderApiPrimitive_Base_Vk<Vk_BufferView>
//{
//public:
//	void destroy();
//};

class Vk_BufferView : public Vk_RenderApiPrimitive<Vk_BufferView_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_BufferView_T>;

public:
	Vk_BufferView() = default;
	~Vk_BufferView() { destroy(); }

	void create(const VkDescriptorSetLayoutCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- Vk_DescriptorSetLayout-Decl ---
#endif // 0
#if 1

class Vk_DescriptorSetLayout : public Vk_RenderApiPrimitive<Vk_DescriptorSetLayout_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSetLayout_T>;

public:
	Vk_DescriptorSetLayout() = default;
	~Vk_DescriptorSetLayout() { destroy(); }

	void create(const VkDescriptorSetLayoutCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- Vk_DescriptorPool-Decl ---
#endif // 0
#if 1

class Vk_DescriptorPool : public Vk_RenderApiPrimitive<Vk_DescriptorPool_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSetLayout_T>;

public:
	Vk_DescriptorPool() = default;
	~Vk_DescriptorPool() { destroy(); }

	void create(const VkDescriptorPoolCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- Vk_DescriptorSet-Decl ---
#endif // 0
#if 1

//template<> 
//class Vk_RenderApiPrimitive<Vk_DescriptorSet_T> : public NonCopyable
//{
//public:
//	using Util = RenderApiUtil_Vk;
//
//public:
//	Vk_RenderApiPrimitive()		= default;
//	~Vk_RenderApiPrimitive()	= default;
//
//	void destroy();
//
//protected:
//	//Vector<
//};

class Vk_DescriptorSet : public Vk_RenderApiPrimitive<Vk_DescriptorSet_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSet_T>;

public:
	Vk_DescriptorSet() = default;
	~Vk_DescriptorSet() { destroy(); }

	Vk_DescriptorSet(Vk_DescriptorSet&&)	{ throwIf(true, ""); }
	void operator=(Vk_DescriptorSet&&)		{ throwIf(true, ""); }

	void create(const VkDescriptorSetAllocateInfo* pAllocateInfo);
	void destroy();

protected:

};

#endif

#endif


}
#endif