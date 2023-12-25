#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderApi_Include_Vk.h"

#if RDS_RENDER_HAS_VULKAN

#if RDS_ENABLE_RenderResouce_DEBUG_NAME

#define RDS_VK_SET_DEBUG_NAME_IMPL(VK_OBJ, NAME, RD_DEV_VK)						(VK_OBJ).setDebugName(NAME, RD_DEV_VK)
#define RDS_VK_SET_DEBUG_NAME_SRCLOC_IMPL(VK_OBJ, RD_DEV_VK, SRCLOC_)			RDS_VK_SET_DEBUG_NAME_IMPL		 (VK_OBJ,	::rds::fmtAs_T<::rds::TempString>("{}:{}-{}", SRCLOC_.func, SRCLOC_.line, RDS_STRINGIFY(VK_OBJ) ), renderDeviceVk() )
#define RDS_VK_SET_DEBUG_NAME_SRCLOC(VK_OBJ)									RDS_VK_SET_DEBUG_NAME_SRCLOC_IMPL(VK_OBJ, renderDeviceVk(), RDS_SRCLOC)
#define RDS_VK_SET_DEBUG_NAME(VK_OBJ, NAME)										RDS_VK_SET_DEBUG_NAME_IMPL		 (VK_OBJ, NAME, renderDeviceVk())

#define RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC_IMPL(VK_OBJ, RD_DEV_VK, SRCLOC_, ...)	RDS_VK_SET_DEBUG_NAME_IMPL				(VK_OBJ, ::rds::fmtAs_T<::rds::TempString>("{}:{}-{}", SRCLOC_.func, SRCLOC_.line, ::rds::fmtAs_T<::rds::TempString>(__VA_ARGS__) ), RD_DEV_VK )
#define RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC(VK_OBJ, SRCLOC_, ...)					RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC_IMPL	(VK_OBJ, renderDeviceVk(), SRCLOC_, __VA_ARGS__)
#define RDS_VK_SET_DEBUG_NAME_FMT_IMPL(VK_OBJ, RD_DEV_VK, ...)					RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC_IMPL	(VK_OBJ, RD_DEV_VK, RDS_SRCLOC, __VA_ARGS__ )
#define RDS_VK_SET_DEBUG_NAME_FMT(VK_OBJ, ...)									RDS_VK_SET_DEBUG_NAME_IMPL				(VK_OBJ, ::rds::fmtAs_T<::rds::TempString>(__VA_ARGS__), renderDeviceVk() )

#else

#define RDS_VK_SET_DEBUG_NAME_IMPL(VK_OBJ, NAME, RD_DEV_VK)						
#define RDS_VK_SET_DEBUG_NAME_SRCLOC_IMPL(VK_OBJ, RD_DEV_VK, SRCLOC_)			
#define RDS_VK_SET_DEBUG_NAME_SRCLOC(VK_OBJ)									
#define RDS_VK_SET_DEBUG_NAME(VK_OBJ, NAME)										

#define RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC_IMPL(VK_OBJ, RD_DEV_VK, SRCLOC_, ...)	
#define RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC(VK_OBJ, SRCLOC_, ...)					
#define RDS_VK_SET_DEBUG_NAME_FMT_IMPL(VK_OBJ, RD_DEV_VK, ...)					
#define RDS_VK_SET_DEBUG_NAME_FMT(VK_OBJ, ...)									


#endif // RDS_DEVELOPMENT


namespace rds
{

using Vk_Instance_T				= VkInstance_T;
using Vk_PhysicalDevice_T		= VkPhysicalDevice_T;
using Vk_Device_T				= VkDevice_T;
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
using Vk_DeviceMemory_T			= VkDeviceMemory_T;

using Vk_DebugUtilsMessenger_T	= VkDebugUtilsMessengerEXT_T;

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


class	Vk_PipelineCache;

struct	Vk_RenderApiUtil;
class	RenderDevice_Vk;

struct	SamplerState;
class	Texture2D_Vk;

class	Vk_Buffer;

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive-Decl ---
#endif // 0
#if 1

template<class T, VkObjectType VK_OBJ_T> 
class Vk_RenderApiPrimitive : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Util		= Vk_RenderApiUtil;
	using HndType	= T*;

public:
	static VkObjectType vkObjectType() { return VK_OBJ_T; }

public:
	Vk_RenderApiPrimitive()		= default;
	~Vk_RenderApiPrimitive() { RDS_CORE_ASSERT(!hnd(), "forgot call Base::destroy() ?"); destroy(); }

	Vk_RenderApiPrimitive	(Vk_RenderApiPrimitive&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=			(Vk_RenderApiPrimitive&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void destroy() 
	{  
		_hnd = VK_NULL_HANDLE;
		#if RDS_ENABLE_RenderResouce_DEBUG_NAME
		_debugName.clear();
		#endif
	}

	void setDebugName(StrView name, RenderDevice_Vk* rdDevVk)
	{
		RDS_CORE_ASSERT(hnd(), "VkObjectType: {}, setDebugName, hnd == nullptr", enumInt(VK_OBJ_T));
		#if RDS_ENABLE_RenderResouce_DEBUG_NAME
		_debugName = name;
		Util::setDebugUtilObjectName(rdDevVk->vkDevice(), vkObjectType(), _debugName, hnd());
		#endif // RDS_DEVELOPMENT
	}

	T*	hnd()		{ return _hnd; }
	T*	hnd() const	{ return _hnd; }

	T**	hndArray()			{ return &_hnd; }
	T**	hndArray() const	{ return constCast(&_hnd); }

	T** hndForInit()	{ return &_hnd; }

	explicit operator bool () const { return _hnd; }

	const char* debugName() const
	{
		#if RDS_ENABLE_RenderResouce_DEBUG_NAME
		return _debugName.c_str();
		#else
		return nullptr;
		#endif // 1
	}

protected:
	void move(Vk_RenderApiPrimitive&& rhs) 
	{
		_hnd = rhs._hnd; 
		rhs._hnd = VK_NULL_HANDLE;

		#if RDS_ENABLE_RenderResouce_DEBUG_NAME
		_debugName = rds::move(rhs._debugName);
		#endif
	}

protected:
	T*		_hnd = VK_NULL_HANDLE;

	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	String	_debugName;
	#endif
};

#endif

#if 0
#pragma mark --- rdsVk_AllocableRenderApiPrimitive-Decl ---
#endif // 0
#if 1

class  Vk_Allocator;
struct Vk_AllocInfo;
template<class T, VkObjectType VK_OBJ_T>
class Vk_AllocableRenderApiPrimitive : public Vk_RenderApiPrimitive<T, VK_OBJ_T>
{
public:
	using Base = Vk_RenderApiPrimitive<T, VK_OBJ_T>;

public:
	Vk_AllocableRenderApiPrimitive() = default;
	~Vk_AllocableRenderApiPrimitive() { RDS_CORE_ASSERT(isInvalid(), "forgot to call Base::destroy() ?"); destroy(); }

	Vk_AllocableRenderApiPrimitive	(Vk_AllocableRenderApiPrimitive&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=					(Vk_AllocableRenderApiPrimitive&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void destroy() { Base::destroy(); _alloc = nullptr; }

	void setDebugName(StrView name, RenderDevice_Vk* rdDevVk)
	{
		#if RDS_DEVELOPMENT
		
		RDS_CORE_ASSERT(_alloc && _allocHnd, "");

		Base::setDebugName(name, rdDevVk);
		_alloc->setAllocationDebugName(debugName(), &_allocHnd);
		#endif // RDS_DEVELOPMENT
	}

	Vk_AllocHnd*	_internal_allocHnd();

	void			_internal_setAlloc(Vk_Allocator* alloc);
	Vk_Allocator*	_internal_alloc();

	bool isInvalid() { return !_alloc; }

protected:
	void move(Vk_AllocableRenderApiPrimitive&& rhs) 
	{
		Base::move(rds::move(rhs));
		_alloc		= rhs._alloc;
		_allocHnd	= rhs._allocHnd;

		rhs._alloc		= nullptr;
		rhs._allocHnd	= {};
	}

protected:
	Vk_Allocator*	_alloc		= nullptr;	// TODO: AllocablePrimitive_Vk<T>::Vk_Allocator* should be strong ref?
	Vk_AllocHnd		_allocHnd	= {};
};

template<class T, VkObjectType VK_OBJ_T> inline Vk_AllocHnd*	Vk_AllocableRenderApiPrimitive<T, VK_OBJ_T>::_internal_allocHnd()						{ return &_allocHnd; }

template<class T, VkObjectType VK_OBJ_T> inline void			Vk_AllocableRenderApiPrimitive<T, VK_OBJ_T>::_internal_setAlloc(Vk_Allocator* alloc)	{ _alloc = alloc; }
template<class T, VkObjectType VK_OBJ_T> inline Vk_Allocator*	Vk_AllocableRenderApiPrimitive<T, VK_OBJ_T>::_internal_alloc()							{ return _alloc; }


#endif // 1

#if 0
#pragma mark --- rdsVk_RenderApiPrimitive-Decl ---
#endif // 0
#if 1

#if 0
#pragma mark --- Vk_Instance-Decl ---
#endif // 0
#if 1

class Vk_Instance : public Vk_RenderApiPrimitive<Vk_Instance_T, VK_OBJECT_TYPE_INSTANCE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Instance_T, VK_OBJECT_TYPE_INSTANCE>;

public:
	Vk_Instance() = default;
	~Vk_Instance() = default;

	Vk_Instance(Vk_Instance&&)		{ throwIf(true, ""); }
	void operator=(Vk_Instance&&)	{ throwIf(true, ""); }

	void create(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_DebugUtilsMessenger-Decl ---
#endif // 0
#if 1

class Vk_DebugUtilsMessenger : public Vk_RenderApiPrimitive<Vk_DebugUtilsMessenger_T, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DebugUtilsMessenger_T, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT>;

public:
	Vk_DebugUtilsMessenger()	= default;
	~Vk_DebugUtilsMessenger()	= default;

	Vk_DebugUtilsMessenger(Vk_DebugUtilsMessenger&&)	{ throwIf(true, ""); }
	void operator=(Vk_DebugUtilsMessenger&&)			{ throwIf(true, ""); }

	void create	(RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_PhysicalDevice-Decl ---
#endif // 0
#if 1

class Vk_PhysicalDevice : public Vk_RenderApiPrimitive<Vk_PhysicalDevice_T, VK_OBJECT_TYPE_PHYSICAL_DEVICE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_PhysicalDevice_T, VK_OBJECT_TYPE_PHYSICAL_DEVICE>;

public:
	Vk_PhysicalDevice()		= default;
	~Vk_PhysicalDevice()	= default;

	Vk_PhysicalDevice(Vk_PhysicalDevice&&)	{ throwIf(true, ""); }
	void operator=(Vk_PhysicalDevice&&)		{ throwIf(true, ""); }

	void create	(Vk_PhysicalDevice_T* vkPhyDevHnd);
	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Device-Decl ---
#endif // 0
#if 1

class Vk_Device : public Vk_RenderApiPrimitive<Vk_Device_T, VK_OBJECT_TYPE_DEVICE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Device_T, VK_OBJECT_TYPE_DEVICE>;

public:
	Vk_Device()	= default;
	~Vk_Device()	= default;

	Vk_Device(Vk_Device&&)		{ throwIf(true, ""); }
	void operator=(Vk_Device&&)	{ throwIf(true, ""); }

	void create	(RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif


#if 0
#pragma mark --- rdsVk_Surface-Decl ---
#endif // 0
#if 1

class Vk_Surface : public Vk_RenderApiPrimitive<Vk_Surface_T, VK_OBJECT_TYPE_SURFACE_KHR>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Surface_T, VK_OBJECT_TYPE_SURFACE_KHR>;

public:
	Vk_Surface()	= default;
	~Vk_Surface()	= default;

	Vk_Surface(Vk_Surface&&)		{ throwIf(true, ""); }
	void operator=(Vk_Surface&&)	{ throwIf(true, ""); }

	void create	(NativeUIWindow* wnd, RenderDevice_Vk* rdDevVk);
	void create	(NativeUIWindow* wnd, Vk_Instance_T* instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, RenderDevice_Vk* rdDevVk);
	void destroy(NativeUIWindow* wnd, RenderDevice_Vk* rdDevVk);

protected:
	NativeUIWindow* _wnd = nullptr;
};

#endif

#if 0
#pragma mark --- rdsVk_Queue-Decl ---
#endif // 0
#if 1

class Vk_Queue : public Vk_RenderApiPrimitive<Vk_Queue_T, VK_OBJECT_TYPE_QUEUE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Queue_T, VK_OBJECT_TYPE_QUEUE>;

public:
	Vk_Queue() = default;
	~Vk_Queue() { destroy(); }

	Vk_Queue(Vk_Queue&&)		{ throwIf(true, ""); }
	void operator=(Vk_Queue&&)	{ throwIf(true, ""); }

	void create(QueueTypeFlags type, RenderDevice_Vk* rdDevVk);
	void create(u32 familyIdx, Vk_Device_T* vkDevice);
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
#pragma mark --- rdsVk_Image-Decl ---
#endif // 0
#if 1

class Vk_Image : public Vk_AllocableRenderApiPrimitive<Vk_Image_T, VK_OBJECT_TYPE_IMAGE>
{
public:
	using Base = Vk_AllocableRenderApiPrimitive<Vk_Image_T, VK_OBJECT_TYPE_IMAGE>;

public:
	Vk_Image() = default;
	~Vk_Image() { destroy(); }

	Vk_Image		(Vk_Image&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=	(Vk_Image&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void create(Vk_Allocator* vkAlloc, const VkImageCreateInfo* imageInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create(RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, u32 width, u32 height, VkFormat vkFormat, VkImageTiling vkTiling, VkImageUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create(Vk_Image_T* vkImage);

	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_ImageView-Decl ---
#endif // 0
#if 1

class Vk_ImageView : public Vk_RenderApiPrimitive<Vk_ImageView_T, VK_OBJECT_TYPE_IMAGE_VIEW>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_ImageView_T, VK_OBJECT_TYPE_IMAGE_VIEW>;

public:
	Vk_ImageView()	= default;
	~Vk_ImageView() = default;

	Vk_ImageView(Vk_ImageView&&)	{ throwIf(true, ""); }
	void operator=(Vk_ImageView&&)	{ throwIf(true, ""); }

	void create(VkImageViewCreateInfo* viewInfo, RenderDevice_Vk* rdDevVk);
	void create(Vk_Image*	vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount, RenderDevice_Vk* rdDevVk);
	void create(Vk_Image_T* vkImage, VkFormat vkFormat, VkImageAspectFlags aspectFlags, u32 mipCount, RenderDevice_Vk* rdDevVk);
	void create(Texture2D_Vk* tex2DVk, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);

};

#endif

#if 0
#pragma mark --- rdsVk_Sampler-Decl ---
#endif // 0
#if 1

class Vk_Sampler : public Vk_RenderApiPrimitive<Vk_Sampler_T, VK_OBJECT_TYPE_SAMPLER>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Sampler_T, VK_OBJECT_TYPE_SAMPLER>;

public:
	Vk_Sampler()	= default;
	~Vk_Sampler()	= default;

	Vk_Sampler(Vk_Sampler&&)		{ throwIf(true, ""); }
	void operator=(Vk_Sampler&&)	{ throwIf(true, ""); }

	void create(VkSamplerCreateInfo* samplerInfo, RenderDevice_Vk* rdDevVk);
	void create(const SamplerState& samplerState, RenderDevice_Vk* rdDevVk);

	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_ShaderModule-Decl ---
#endif // 0
#if 1

class Vk_ShaderModule : public Vk_RenderApiPrimitive<Vk_ShaderModule_T, VK_OBJECT_TYPE_SHADER_MODULE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_ShaderModule_T, VK_OBJECT_TYPE_SHADER_MODULE>;

public:
	Vk_ShaderModule() = default;
	~Vk_ShaderModule();

	Vk_ShaderModule	(Vk_ShaderModule&&)	{ throwIf(true, ""); }
	void operator=	(Vk_ShaderModule&&)	{ throwIf(true, ""); }

	void create	(StrView filename, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_PipelineLayout-Decl ---
#endif // 0
#if 1

class Vk_PipelineLayout : public Vk_RenderApiPrimitive<Vk_PipelineLayout_T, VK_OBJECT_TYPE_PIPELINE_LAYOUT>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_PipelineLayout_T, VK_OBJECT_TYPE_PIPELINE_LAYOUT>;

public:
	Vk_PipelineLayout()		= default;
	~Vk_PipelineLayout()	= default;

	Vk_PipelineLayout(Vk_PipelineLayout&& rhs)	noexcept : Base(rds::move(rhs)) {}
	void operator=	 (Vk_PipelineLayout&& rhs)	noexcept { return Base::operator=(rds::move(rhs)); }

	void create	(const VkPipelineLayoutCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_PipelineCache-Decl ---
#endif // 0
#if 1

class Vk_PipelineCache : public Vk_RenderApiPrimitive<Vk_PipelineCache_T, VK_OBJECT_TYPE_PIPELINE_CACHE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_PipelineCache_T, VK_OBJECT_TYPE_PIPELINE_CACHE>;

public:
	Vk_PipelineCache() = default;
	~Vk_PipelineCache() { destroy(); }

	Vk_PipelineCache(Vk_PipelineCache&&)	{ throwIf(true, ""); }
	void operator=	(Vk_PipelineCache&&)	{ throwIf(true, ""); }

	void destroy();
};

#endif

#if 0
#pragma mark --- rdsVk_Pipeline-Decl ---
#endif // 0
#if 1

class Vk_Pipeline : public Vk_RenderApiPrimitive<Vk_Pipeline_T, VK_OBJECT_TYPE_PIPELINE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Pipeline_T, VK_OBJECT_TYPE_PIPELINE>;

public:
	Vk_Pipeline()	= default;
	~Vk_Pipeline() = default;

	Vk_Pipeline		(Vk_Pipeline&& rhs)	noexcept : Base(rds::move(rhs)) {}
	void operator=	(Vk_Pipeline&& rhs)	noexcept { return Base::operator=(rds::move(rhs)); }

	void create	(const VkGraphicsPipelineCreateInfo* pCreateInfo, u32 infoCount, Vk_PipelineCache* vkPipelineCache, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_Semaphore-Decl ---
#endif // 0
#if 1

class Vk_Semaphore : public Vk_RenderApiPrimitive<Vk_Semaphore_T, VK_OBJECT_TYPE_SEMAPHORE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Semaphore_T, VK_OBJECT_TYPE_SEMAPHORE>;

public:
	Vk_Semaphore()	= default;
	~Vk_Semaphore() = default;

	Vk_Semaphore(Vk_Semaphore&&)	{ throwIf(true, ""); }
	void operator=(Vk_Semaphore&&)	{ throwIf(true, ""); }

	void create	(RenderDevice_Vk* rdDevVk);
	void create	(const VkSemaphoreCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_Fence-Decl ---
#endif // 0
#if 1

class Vk_Fence : public Vk_RenderApiPrimitive<Vk_Fence_T, VK_OBJECT_TYPE_FENCE>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Fence_T, VK_OBJECT_TYPE_FENCE>;

public:
	Vk_Fence()	= default;
	~Vk_Fence() = default;

	Vk_Fence(Vk_Fence&&)		{ throwIf(true, ""); }
	void operator=(Vk_Fence&&)	{ throwIf(true, ""); }

	void create	(RenderDevice_Vk* rdDevVk);
	void create	(const VkFenceCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);

	VkResult wait	(RenderDevice_Vk* rdDev, u64 timeout = NumLimit<u64>::max());
	VkResult reset	(RenderDevice_Vk* rdDev);
	VkResult status (RenderDevice_Vk* rdDev) const;

	bool isSignaled(RenderDevice_Vk* rdDev) const;
};

#endif

#if 0
#pragma mark --- rdsVk_DeviceMemory-Decl ---
#endif // 0
#if 1

class Vk_DeviceMemory : public Vk_RenderApiPrimitive<Vk_DeviceMemory_T, VK_OBJECT_TYPE_DEVICE_MEMORY>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DeviceMemory_T, VK_OBJECT_TYPE_DEVICE_MEMORY>;

public:
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_ScopedMemMapBuf-Decl ---
#endif // 0
#if 1

class Vk_ScopedMemMapBuf
{
public:
	Vk_ScopedMemMapBuf(Vk_Buffer* vkBuf);
	~Vk_ScopedMemMapBuf();

	template<class T> T data();

	void unmap();

protected:
	Vk_Buffer*	_p		= nullptr;
	void*		_data	= nullptr;
};

#endif

template<class T> inline T Vk_ScopedMemMapBuf::data() { return reinCast<T>(_data); }

#if 0
#pragma mark --- rdsVk_Buffer-Decl ---
#endif // 0
#if 1

class Vk_Buffer : public Vk_AllocableRenderApiPrimitive<Vk_Buffer_T, VK_OBJECT_TYPE_BUFFER>
{
public:
	using Base = Vk_AllocableRenderApiPrimitive<Vk_Buffer_T, VK_OBJECT_TYPE_BUFFER>;

public:
	Vk_Buffer() = default;
	~Vk_Buffer() { destroy(); }

	Vk_Buffer		(Vk_Buffer&& rhs) noexcept { move(rds::move(rhs)); }
	void operator=	(Vk_Buffer&& rhs) noexcept { RDS_CORE_ASSERT(this != &rhs); move(rds::move(rhs)); }

	void create	(Vk_Allocator* alloc, const VkBufferCreateInfo* bufferInfo, Vk_AllocInfo* allocInfo, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void create	(RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags, VkMemoryPropertyFlags vkMemPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void destroy();

	Vk_ScopedMemMapBuf scopedMemMap();
};


#endif

#if 0
#pragma mark --- rdsVk_BufferView-Decl ---
#endif // 0
#if 1

class Vk_BufferView : public Vk_RenderApiPrimitive<Vk_BufferView_T, VK_OBJECT_TYPE_BUFFER_VIEW>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_BufferView_T, VK_OBJECT_TYPE_BUFFER_VIEW>;

public:
	Vk_BufferView()		= default;
	~Vk_BufferView()	= default;

	void create	(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_DescriptorSetLayout-Decl ---
#endif // 0
#if 1

class Vk_DescriptorSetLayout : public Vk_RenderApiPrimitive<Vk_DescriptorSetLayout_T, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSetLayout_T, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT>;

public:
	Vk_DescriptorSetLayout()	= default;
	~Vk_DescriptorSetLayout()	= default;

	void create	(const VkDescriptorSetLayoutCreateInfo* cInfo, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_DescriptorPool-Decl ---
#endif // 0
#if 1

class Vk_DescriptorPool : public Vk_RenderApiPrimitive<Vk_DescriptorPool_T, VK_OBJECT_TYPE_DESCRIPTOR_POOL>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorPool_T, VK_OBJECT_TYPE_DESCRIPTOR_POOL>;

public:
	Vk_DescriptorPool()		= default;
	~Vk_DescriptorPool()	= default;

	Vk_DescriptorPool	(Vk_DescriptorPool&& rhs) noexcept : Base(rds::move(rhs)) {}
	void operator=		(Vk_DescriptorPool&& rhs) noexcept { return Base::operator=(rds::move(rhs)); }
	
	VkResult	create(VkDescriptorPoolCreateInfo* cInfo, RenderDevice_Vk* rdDevVk);
	void		destroy(RenderDevice_Vk* rdDevVk);

	void reset(VkDescriptorPoolResetFlags flag, RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_DescriptorSet-Decl ---
#endif // 0
#if 1

class Vk_DescriptorSet : public Vk_RenderApiPrimitive<Vk_DescriptorSet_T, VK_OBJECT_TYPE_DESCRIPTOR_SET>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_DescriptorSet_T, VK_OBJECT_TYPE_DESCRIPTOR_SET>;

public:
	Vk_DescriptorSet()	= default;
	~Vk_DescriptorSet();

	Vk_DescriptorSet(Vk_DescriptorSet&& rhs) noexcept : Base(rds::move(rhs)) {}
	void operator=	(Vk_DescriptorSet&& rhs) noexcept { return Base::operator=(rds::move(rhs)); }


	VkResult	create (VkDescriptorSetAllocateInfo* pAllocateInfo, RenderDevice_Vk* rdDevVk);
	void		destroy();

protected:

};

#endif

#endif


}
#endif