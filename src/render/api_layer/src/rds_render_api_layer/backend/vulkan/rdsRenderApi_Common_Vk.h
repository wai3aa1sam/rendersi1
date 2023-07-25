#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"
#include "rdsRenderApi_Include_Vk.h"
#include "rdsVkPtr.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 1

template<size_t N>	using SwapChainImages_Vk_N			= Vector<Vk_Image*, N>;
					using SwapChainImages_Vk			= Vector<Vk_Image*,				RenderApiLayerTraits::s_kSwapchainImageLocalSize>;
template<size_t N>	using SwapChainImageViews_Vk_N		= Vector<VkPtr<Vk_ImageView>, N>;
					using SwapChainImageViews_Vk		= Vector<VkPtr<Vk_ImageView>,	RenderApiLayerTraits::s_kSwapchainImageLocalSize>;
template<size_t N>	using SwapChainFramebuffers_Vk_N	= Vector<VkPtr<Vk_Framebuffer>, N>;
					using SwapChainFramebuffers_Vk		= Vector<VkPtr<Vk_Framebuffer>,	RenderApiLayerTraits::s_kSwapchainImageLocalSize>;

#endif // 1


#if 1	// TODO: remove / modify

struct QueueFamilyIndices 
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kQueueTypeCount = 4;

	void clear() 
	{ 
		graphics.reset();
		present.reset();
		transfer.reset();
	}

	u32 getQueueIdx(QueueTypeFlags flags) const 
	{
		using SRC = QueueTypeFlags;
		switch (flags)
		{
			case rds::QueueTypeFlags::Graphics:		{ return graphics.value(); } break;
			case rds::QueueTypeFlags::Compute:		{ return graphics.value(); } break;
			case rds::QueueTypeFlags::Transfer:		{ return transfer.value(); } break;
			case rds::QueueTypeFlags::Present:		{ return present.value();  } break;
		}
		return ~u32(0);
	}

	bool isFoundAll()		const { return graphics.has_value() && present.has_value() && transfer.has_value(); }
	bool isUniqueGraphics() const { return isFoundAll() && (graphics.value() != present.value()  && graphics.value() != transfer.value()); }
	bool isUniquePresent () const { return isFoundAll() && (present.value()  != graphics.value() && present.value()  != transfer.value()); }
	bool isUniqueTransfer() const { return isFoundAll() && (transfer.value() != graphics.value() && transfer.value() != present.value()); }
	bool isAllUnique()		const { return isUniqueGraphics() && isUniquePresent() && isUniqueTransfer(); }

	template<size_t N>
	u32 get(Vector<u32, N>& out, QueueTypeFlags flag)
	{
		auto count = BitUtil::count1(sCast<u32>(flag));
		out.clear();
		out.reserve(count);
		if (BitUtil::has(flag, QueueTypeFlags::Graphics))	{ out.emplace_back(graphics.value()); }
		if (BitUtil::has(flag, QueueTypeFlags::Present))	{ out.emplace_back(present.value()); }
		if (BitUtil::has(flag, QueueTypeFlags::Transfer))	{ out.emplace_back(transfer.value()); }
		return sCast<u32>(count);
	}

public:
	Opt<u32> graphics;
	Opt<u32> present;
	Opt<u32> transfer;
};
#endif // 1

#if 1	// TODO: remove / modify

struct SwapchainAvailableInfo_Vk 
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 4;

public:
	VkSurfaceCapabilitiesKHR					capabilities;
	Vector<VkSurfaceFormatKHR,	s_kLocalSize>	formats;
	Vector<VkPresentModeKHR,	s_kLocalSize>	presentModes;
};

// TODO: change to our Swapchain_CreateDesc
struct SwapchainInfo_Vk
{
	//VkSurfaceCapabilitiesKHR	capabilities;
	VkSurfaceFormatKHR			surfaceFormat;
	VkPresentModeKHR			presentMode;
	VkExtent2D					extent;
};

#endif

#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Decl ---
#endif // 0
#if 1


struct RenderApiUtil_Vk : public RenderApiUtil
{
public:
	friend class Renderer_Vk;

	using Rect2f = math::Rect2f;

public:
	using Base					= RenderApiUtil;
	using Result				= VkResult;
	//using SwapchainAvailInfo	= SwapchainAvailInfo_Vk;

public:
	RenderApiUtil_Vk() = delete;

public:
	static void throwIfError	(Result ret);
	static bool assertIfError	(Result ret);
	static void reportError		(Result ret);

	static String getStrFromVkResult(Result ret);

public:
	template<class T> static VkDeviceSize toVkDeviceSize(T v);

	static VkExtent2D toVkExtent2D(const Rect2f& rect2);
	static VkExtent2D toVkExtent2D(const Vec2f&  vec2);

	static VkFormat	toVkFormat(RenderDataType v);

	static VkBufferUsageFlagBits toVkBufferUsage(RenderGpuBufferTypeFlags type);

	static VkMemoryPropertyFlags toVkMemoryPropFlags(RenderMemoryUsage memUsage);

	template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& out, T** vkData, u32 n);
	template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& dst, const Vector<T*, N>& src);


public:
	static u32 getMemoryTypeIdx(u32 memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties);

	// for create vk objects
public:
	static void createDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& out);
	static void createSurface(Vk_Surface** out, Vk_Instance_T* vkInstance, const VkAllocationCallbacks* allocCallbacks, NativeUIWindow* window);
	static void createSwapchain(Vk_Swapchain** out, Vk_Surface* vkSurface, Vk_Device* vkDevice, 
								const SwapchainInfo_Vk& info, const SwapchainAvailableInfo_Vk& avaInfo, const QueueFamilyIndices& queueFamilyIndices);
	static void createImageView(Vk_ImageView** out, Vk_Image* vkImage, Vk_Device* vkDevice, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	static void createShaderModule(Vk_ShaderModule** out, StrView filename, Vk_Device* vkDevice);

	static void createSemaphore(Vk_Semaphore** out, Vk_Device* vkDevice);
	static void createFence(Vk_Fence** out, Vk_Device* vkDevice);

	static void createCommandPool(Vk_CommandPool_T** outVkCmdPool, u32 queueIdx, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	static void createCommandBuffer(Vk_CommandBuffer_T** outVkCmdBuf, Vk_CommandPool_T* vkCmdPool, VkCommandBufferLevel vkBufLevel);

	template<size_t N> static void createImageViews(Vector<VkPtr<Vk_ImageView>, N>& out, const Vector<VkPtr<Vk_Image>, N>& vkImages, Vk_Device* vkDevice, 
													VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	template<size_t N> static u32  createSwapchainImages	(SwapChainImages_Vk_N<N>& out, Vk_Swapchain* vkSwapchain, Vk_Device* vkDevice);
	template<size_t N> static void createSwapchainImageViews(SwapChainImageViews_Vk_N<N>& out, const SwapChainImages_Vk_N<N>& vkImages, Vk_Device* vkDevice, 
															 VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	static void createBuffer(Vk_Buffer_T** outBuf, Vk_DeviceMemory** outBufMem, VkDeviceSize size
							, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, QueueTypeFlags queueTypeFlags);
	static void copyBuffer	(Vk_Buffer* dstBuffer, Vk_Buffer* srcBuffer, VkDeviceSize size, Vk_CommandPool_T* vkCmdPool, Vk_Queue* vkTransferQueue);

	static void createBuffer(Vk_Buffer& outBuf, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags);
	static void createBuffer(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags);

public:
	template<size_t N> static u32 getAvailableGPUDevicesTo	(Vector<Vk_PhysicalDevice*,		 N>& out, Vk_Instance_T* vkInstance);
	template<size_t N> static u32 getQueueFaimlyPropertiesTo(Vector<VkQueueFamilyProperties, N>& out, Vk_PhysicalDevice* vkPhyDevice);

	static void getPhyDevicePropertiesTo	(RenderAdapterInfo& outInfo, Vk_PhysicalDevice* phyDevice);
	static void getPhyDeviceFeaturesTo		(RenderAdapterInfo& outInfo, Vk_PhysicalDevice* phyDevice);
	static void getVkPhyDeviceFeaturesTo	(VkPhysicalDeviceFeatures& out, const RenderAdapterInfo& info);
	static bool getSwapchainAvailableInfoTo	(SwapchainAvailableInfo_Vk& out, Vk_PhysicalDevice* vkPhydevice, Vk_Surface* vkSurface);


private:
	static bool _checkError(Result ret);

	// return VK_TRUE, then abort with VK_ERROR_VALIDATION_FAILED_EXT error
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*										pUserData);
};

template<class T> inline VkDeviceSize toVkDeviceSize(T v) { RDS_S_ASSERT(IsIntegral<T>); return sCast<VkDeviceSize>(v); }

#endif

#if 0
#pragma mark --- rdsExtensionInfo_Vk-Decl ---
#endif // 0
#if 1
struct Renderer_CreateDesc;

struct ExtensionInfo_Vk
{
public:
	using Util		= RenderApiUtil_Vk;
	using SizeType	= RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 12;

public:
	static u32 getAvailableValidationLayersTo	(Vector<VkLayerProperties,		s_kLocalSize>& out, bool logAvaliable = false);
	static u32 getAvailableInstanceExtensionsTo	(Vector<VkExtensionProperties,	s_kLocalSize>& out, bool logAvaliable = false);
	static u32 getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties,	s_kLocalSize>& out, Vk_PhysicalDevice* phyDevice, bool logAvaliable = false);

public:
	void createInstanceExtensions	(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension = false);
	void createValidationLayers		(const RenderAdapterInfo& adapterInfo);
	void createPhyDeviceExtensions	(const RenderAdapterInfo& adapterInfo, const Renderer_CreateDesc& cDesc, Vk_PhysicalDevice* phyDevice);

	template<class T> T getInstanceExtFunction(const char* funcName) const;
	template<class T> T getDeviceExtFunction(const char* funcName) const;

	bool isSupportValidationLayer(const char* validationLayerName) const;

public:
	const Vector<const char*,			s_kLocalSize>&	instanceExts()		const;
	const Vector<const char*,			s_kLocalSize>&	validationLayers()	const;
	const Vector<const char*,			s_kLocalSize>&	phyDeviceExts()		const;

	const StringMap<PFN_vkVoidFunction>&				instanceExtFuncTable()		const;
		  StringMap<PFN_vkVoidFunction>&				instanceExtFuncTable()		;
	const StringMap<PFN_vkVoidFunction>&				deviceExtFuncTable()		const;
		  StringMap<PFN_vkVoidFunction>&				deviceExtFuncTable()		;

	const Vector<VkLayerProperties,		s_kLocalSize>&	availableLayers()			const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availableExts()				const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availablePhyDeviceExts()	const;

private:
	void checkValidationLayersExist();

private:
	Vector<const char*,				s_kLocalSize>	_instanceExts;
	Vector<const char*,				s_kLocalSize>	_validationLayers;
	Vector<const char*,				s_kLocalSize>	_phyDeviceExts;
	StringMap<PFN_vkVoidFunction>					_instanceExtFuncTable;
	StringMap<PFN_vkVoidFunction>					_deviceExtFuncTable;

	Vector<VkLayerProperties,		s_kLocalSize>	_availableLayers;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availableExts;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availablePhyDeviceExts;
};


template<class T> inline 
T 
ExtensionInfo_Vk::getInstanceExtFunction(const char* funcName) const
{
	auto& table = _deviceExtFuncTable;
	auto it = table.find(funcName);
	if (it == table.end())
	{
		PFN_vkVoidFunction func = vkGetInstanceProcAddr(Renderer_Vk::instance()->vkInstance(), funcName);
		if (!func)
			return nullptr;
		constCast<StringMap<PFN_vkVoidFunction>&>(table)[funcName] = func;
		return reinCast<T>(func);
	}
	return reinCast<T>(it->second);
}

template<class T> inline 
T
ExtensionInfo_Vk::getDeviceExtFunction(const char* funcName) const
{
	auto& table = _deviceExtFuncTable;
	auto it = table.find(funcName);
	if (it == table.end())
	{
		PFN_vkVoidFunction func = vkGetDeviceProcAddr(Renderer_Vk::instance()->vkDevice(), funcName);
		if (!func)
			return nullptr;
		constCast<StringMap<PFN_vkVoidFunction>&>(table)[funcName] = func;
		return reinCast<T>(func);
	}
	return reinCast<T>(it->second);
}

inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::instanceExts()		const		{ return _instanceExts; }
inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::validationLayers()	const		{ return _validationLayers; }
inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::phyDeviceExts()		const		{ return _phyDeviceExts; }

inline const StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::instanceExtFuncTable()	const		{ return _instanceExtFuncTable; }
inline		 StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::instanceExtFuncTable()				{ return _instanceExtFuncTable; }
inline const StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::deviceExtFuncTable()		const		{ return _deviceExtFuncTable; }
inline		 StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::deviceExtFuncTable()					{ return _deviceExtFuncTable; }

inline const Vector<VkLayerProperties,		ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availableLayers()	const		{ return _availableLayers; };
inline const Vector<VkExtensionProperties,	ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availableExts()		const		{ return _availableExts; };
inline const Vector<VkExtensionProperties,	ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availablePhyDeviceExts()const	{ return _availablePhyDeviceExts; };

#endif

#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Decl ---
#endif // 0
#if 1

template<class T, size_t N> inline
void
RenderApiUtil_Vk::convertToVkPtrs(Vector<VkPtr<T>, N>& out, T** vkData, u32 n)
{
	out.clear();
	out.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		out[i].reset(vkData[i]);
	}
}

template<class T, size_t N> inline
void 
RenderApiUtil_Vk::convertToVkPtrs(Vector<VkPtr<T>, N>& dst, const Vector<T*, N>& src)
{
	convertToVkPtrs(dst, (T**)src.data(), sCast<u32>(src.size()));
}

template<size_t N> inline
void 
RenderApiUtil_Vk::createImageViews(Vector<VkPtr<Vk_ImageView>, N>& out, const Vector<VkPtr<Vk_Image>, N>& vkImages, Vk_Device* vkDevice,
								   VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
{
	out.clear();
	out.resize(vkImages.size());
	for (size_t i = 0; i < vkImages.size(); ++i)
	{
		Vk_ImageView* p = nullptr;
		createImageView(&p, (Vk_Image*)vkImages[i].ptr(), vkDevice, format, aspectFlags, mipLevels);
		out[i].reset(p);
	}
}

template<size_t N> inline
u32 
RenderApiUtil_Vk::createSwapchainImages(SwapChainImages_Vk_N<N>& out, Vk_Swapchain* vkSwapchain, Vk_Device* vkDevice)
{
	u32 imageCount = 0;
	vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &imageCount, nullptr);
	out.resize(imageCount);
	auto ret = vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &imageCount, out.data());
	throwIfError(ret);
	return imageCount;
}

template<size_t N> inline 
void 
RenderApiUtil_Vk::createSwapchainImageViews(SwapChainImageViews_Vk_N<N>& out, const SwapChainImages_Vk_N<N>& vkImages, Vk_Device* vkDevice, 
											VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
{
	out.clear();
	out.resize(vkImages.size());
	for (size_t i = 0; i < vkImages.size(); ++i)
	{
		Vk_ImageView* p = nullptr;
		createImageView(&p, (Vk_Image*)vkImages[i], vkDevice, format, aspectFlags, mipLevels);
		out[i].reset(p);
	}
}

template<size_t N> inline
u32
RenderApiUtil_Vk::getAvailableGPUDevicesTo(Vector<Vk_PhysicalDevice*, N>& out, Vk_Instance_T* vkInstance)
{
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	throwIf(deviceCount == 0, "failed to find GPUs with Vulkan support!");
	
	auto& o = out;
	o.clear();
	o.resize(deviceCount);
	auto ret = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, o.data());
	throwIfError(ret);

	return deviceCount;
}

template<size_t N> inline
u32 
RenderApiUtil_Vk::getQueueFaimlyPropertiesTo(Vector<VkQueueFamilyProperties, N>& out,	Vk_PhysicalDevice* vkPhyDevice)
{
	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, nullptr);

	auto& queueFamilies = out;
	queueFamilies.clear();
	queueFamilies.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhyDevice, &queueFamilyCount, queueFamilies.data());

	return queueFamilyCount;
}


#endif // 1


#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Impl ---
#endif // 0
#if 1



#endif

}

#endif // RDS_RENDER_HAS_VULKAN


