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
using Vk_Surface_T				= VkSurfaceKHR_T;
using Vk_Swapchain_T			= VkSwapchainKHR_T;
using Vk_Framebuffer_T			= VkFramebuffer_T;

using Vk_RenderPass_T			= VkRenderPass_T;
using Vk_Pipeline_T				= VkPipeline_T;
using Vk_PipelineLayout_T		= VkPipelineLayout_T;
using Vk_PipelineCache_T		= VkPipelineCache_T;
using Vk_DescriptorSet_T		= VkDescriptorSet_T;
using Vk_DescriptorSetLayout_T	= VkDescriptorSetLayout_T;
using Vk_DescriptorPool_T		= VkDescriptorPool_T;
using Vk_ShaderModule_T			= VkShaderModule_T;

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

#if		RDS_VK_VER_1_3

using Vk_PipelineStageFlags		= VkPipelineStageFlags2;
using Vk_PipelineStageFlagBit	= VkPipelineStageFlagBits2;

//enum class Vk_PipelineStageFlag : u64
//{
//	STAGE_2_NONE = VK_PIPELINE_STAGE_2_NONE,
//};

#elif	RDS_VK_VER_1_2

using Vk_PipelineStageFlags = VkPipelineStageFlags2KHR;

#else
	#error "unsupported vulkan version"
#endif // RDS_VK_VER_1_3


class Vk_PipelineCache;

struct Vk_RenderApiUtil;
class Renderer_Vk;

struct SamplerState;
class Texture2D_Vk;

template<class T>
class RenderApiResource_Vk : public NonCopyable
{
public:
	using Util = Vk_RenderApiUtil;

public:
	RenderApiResource_Vk()	= default;
	~RenderApiResource_Vk() = default;

	RenderApiResource_Vk(RenderApiResource_Vk&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=		(RenderApiResource_Vk&& rhs) noexcept { move(rds::move(rhs)); } // intended no check for this == &rhs, it is rare case

	void destroy();

	T*	hnd()			{ return _hnd; }
	T**	hndArray()		{ return &_hnd; }
	T** hndForInit()	{ return &_hnd; }

protected:
	void move(RenderApiResource_Vk&& rhs) 
	{
		_hnd = rhs._hnd; 
		rhs._hnd = VK_NULL_HANDLE;
	}

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
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	//using Base = typename Vk_RenderApiPrimitive<T>;

	using Util = Vk_RenderApiUtil;

	using HndType = T;

public:
	Vk_RenderApiPrimitive()		= default;
	~Vk_RenderApiPrimitive() { RDS_CORE_ASSERT(!hnd(), "forgot call Base::destroy() ?"); destroy(); }

	Vk_RenderApiPrimitive	(Vk_RenderApiPrimitive&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=			(Vk_RenderApiPrimitive&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void destroy() {  _hnd = VK_NULL_HANDLE; }

	T*	hnd()		{ return _hnd; }
	T*	hnd() const	{ return _hnd; }

	T**	hndArray()			{ return &_hnd; }
	T**	hndArray() const	{ return constCast(&_hnd); }

	T** hndForInit()	{ return &_hnd; }

	explicit operator bool () const { return _hnd; }

protected:
	void move(Vk_RenderApiPrimitive&& rhs) 
	{
		_hnd = rhs._hnd; 
		rhs._hnd = VK_NULL_HANDLE;
	}

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
	Vk_AllocableRenderApiPrimitive() = default;
	~Vk_AllocableRenderApiPrimitive() { RDS_CORE_ASSERT(isInvalid(), "forgot to call Base::destroy() ?"); destroy(); }

	Vk_AllocableRenderApiPrimitive	(Vk_AllocableRenderApiPrimitive&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=					(Vk_AllocableRenderApiPrimitive&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void destroy() { Vk_RenderApiPrimitive<T>::destroy(); _alloc = nullptr; }

	Vk_AllocHnd*	_internal_allocHnd();

	void			_internal_setAlloc(Vk_Allocator* alloc);
	Vk_Allocator*	_internal_alloc();

	bool isInvalid() { return !_alloc; }

protected:
	void move(Vk_AllocableRenderApiPrimitive&& rhs) 
	{
		_hnd		= rhs._hnd;
		_alloc		= rhs._alloc;
		_allocHnd	= rhs._allocHnd;

		rhs._hnd		= VK_NULL_HANDLE;
		rhs._alloc		= nullptr;
		rhs._allocHnd	= {};
	}
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
#pragma mark --- rdsVk_Surface-Impl ---
#endif // 0
#if 1

class Vk_Surface : public Vk_RenderApiPrimitive<Vk_Surface_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Surface_T>;

public:
	Vk_Surface() = default;
	~Vk_Surface() 
	{
		destroy(nullptr);
	}

	Vk_Surface(Vk_Surface&&)		{ throwIf(true, ""); }
	void operator=(Vk_Surface&&)	{ throwIf(true, ""); }

	void create(NativeUIWindow* wnd);
	void create(NativeUIWindow* wnd, Vk_Instance_T* instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator);
	void destroy(NativeUIWindow* wnd);

protected:
	NativeUIWindow* _wnd = nullptr;
};

#else

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

	void create(QueueTypeFlags type, RenderDevice_Vk* rdDev);
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
#if 0

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

	Vk_Image		(Vk_Image&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=	(Vk_Image&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

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
	~Vk_ImageView() { RDS_CORE_ASSERT(!hnd(), ""); }

	Vk_ImageView(Vk_ImageView&&)	{ throwIf(true, ""); }
	void operator=(Vk_ImageView&&)	{ throwIf(true, ""); }

	void create(VkImageViewCreateInfo* viewInfo);
	void create(Vk_Image*   vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount = 1);
	void create(Vk_Image_T* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount = 1);
	void create(Texture2D_Vk* tex2DVk, Renderer_Vk* rdr);
	void destroy();
	void destroy(RenderDevice_Vk* rdDev);

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
	~Vk_Sampler() { RDS_CORE_ASSERT(!hnd(), ""); }

	Vk_Sampler(Vk_Sampler&&)		{ throwIf(true, ""); }
	void operator=(Vk_Sampler&&)	{ throwIf(true, ""); }

	void create(VkSamplerCreateInfo* samplerInfo);
	void create(const SamplerState& samplerState, Renderer_Vk* rdr);

	void destroy(Renderer_Vk* rdr);
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
#pragma mark --- rdsVk_ShaderModule-Impl ---
#endif // 0
#if 1

class Vk_ShaderModule : public Vk_RenderApiPrimitive<Vk_ShaderModule_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_ShaderModule_T>;

public:
	Vk_ShaderModule() = default;
	~Vk_ShaderModule() { destroy(); }

	Vk_ShaderModule(Vk_ShaderModule&&)	{ throwIf(true, ""); }
	void operator=(Vk_ShaderModule&&)	{ throwIf(true, ""); }

	void create(Renderer_Vk* rdr, StrView filename);
	void destroy();

protected:
	Renderer_Vk* _rdr = nullptr;
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

	Vk_RenderPass	(Vk_RenderPass&&) { throwIf(true, ""); }
	void operator=	(Vk_RenderPass&&) { throwIf(true, ""); }

	void create(const VkRenderPassCreateInfo* pCreateInfo);
	void destroy();
};


#endif

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive<Vk_PipelineLayout>-Impl ---
#endif // 0
#if 1

class Vk_PipelineLayout : public Vk_RenderApiPrimitive<Vk_PipelineLayout_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_PipelineLayout_T>;

public:
	Vk_PipelineLayout() = default;
	~Vk_PipelineLayout() { destroy(); }

	Vk_PipelineLayout(Vk_PipelineLayout&& rhs)	noexcept : Base(rds::move(rhs)) {}
	void operator=	 (Vk_PipelineLayout&& rhs)	noexcept { return Base::operator=(rds::move(rhs)); }

	void create(Renderer_Vk* rdr, const VkPipelineLayoutCreateInfo* pCreateInfo);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_PipelineCache-Impl ---
#endif // 0
#if 1

class Vk_PipelineCache : public Vk_RenderApiPrimitive<Vk_PipelineCache_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_PipelineCache_T>;

public:
	Vk_PipelineCache() = default;
	~Vk_PipelineCache() { destroy(); }

	Vk_PipelineCache(Vk_PipelineCache&&)	{ throwIf(true, ""); }
	void operator=	(Vk_PipelineCache&&)	{ throwIf(true, ""); }

	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Pipeline-Impl ---
#endif // 0
#if 1

class Vk_Pipeline : public Vk_RenderApiPrimitive<Vk_Pipeline_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Pipeline_T>;

public:
	Vk_Pipeline() = default;
	~Vk_Pipeline() { destroy(); }

	Vk_Pipeline		(Vk_Pipeline&& rhs)	noexcept : Base(rds::move(rhs)) {}
	void operator=	(Vk_Pipeline&& rhs)	noexcept { return Base::operator=(rds::move(rhs)); }

	void create(Renderer_Vk* rdr, const VkGraphicsPipelineCreateInfo* pCreateInfo, u32 infoCount, Vk_PipelineCache* vkPipelineCache);
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

	VkResult wait(RenderDevice_Vk* rdDev, u64 timeout = NumLimit<u64>::max());
	VkResult reset(RenderDevice_Vk* rdDev);
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

	Vk_Buffer		(Vk_Buffer&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=	(Vk_Buffer&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

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

	void create(const VkDescriptorSetLayoutCreateInfo* cInfo);
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
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorPool_T>;

public:
	Vk_DescriptorPool() = default;
	~Vk_DescriptorPool();

	Vk_DescriptorPool	(Vk_DescriptorPool&& rhs) noexcept : Base(rds::move(rhs)) {}
	void operator=		(Vk_DescriptorPool&& rhs) noexcept { return Base::operator=(rds::move(rhs)); }
	
	VkResult create(VkDescriptorPoolCreateInfo* cInfo, Renderer_Vk* rdr);

	void destroy(Renderer_Vk* rdr);

	void reset(VkDescriptorPoolResetFlags flag, Renderer_Vk* rdr);
};

#endif

#if 0
#pragma mark --- Vk_DescriptorSet-Decl ---
#endif // 0
#if 1

class Vk_DescriptorSet : public Vk_RenderApiPrimitive<Vk_DescriptorSet_T>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSet_T>;

public:
	Vk_DescriptorSet() = default;
	~Vk_DescriptorSet() { destroy(); }

	Vk_DescriptorSet(Vk_DescriptorSet&& rhs) noexcept : Base(rds::move(rhs)) {}
	void operator=	(Vk_DescriptorSet&& rhs) noexcept { return Base::operator=(rds::move(rhs)); }


	VkResult create(VkDescriptorSetAllocateInfo* pAllocateInfo);
	VkResult create(VkDescriptorSetAllocateInfo* pAllocateInfo, Renderer_Vk* rdr);

	void destroy();

protected:

};

#endif

#endif


}
#endif