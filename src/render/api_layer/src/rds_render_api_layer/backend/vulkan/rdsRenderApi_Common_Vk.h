#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"
#include "rdsRenderApi_Include_Vk.h"
#include "rdsVkPtr.h"

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

public:
	Opt<u32> graphics;
	Opt<u32> present;
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
	static VkExtent2D toVkExtent2D(const Rect2f& rect2);
	static VkExtent2D toVkExtent2D(const Vec2f&  vec2);

	template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& out, T** vkData, u32 n);
	template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& dst, const Vector<T*, N>& src);


	// for create vk objects
public:
	static void createDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& out);
	static void createSurface(Vk_Surface** out, Vk_Instance* vkInstance, const VkAllocationCallbacks* allocCallbacks, NativeUIWindow* window);
	static void createSwapchain(Vk_Swapchain** out, Vk_Surface* vkSurface, Vk_Device* vkDevice, 
								const SwapchainInfo_Vk& info, const SwapchainAvailableInfo_Vk& avaInfo, const QueueFamilyIndices& queueFamilyIndices);
	static void createImageView(Vk_ImageView** out, Vk_Image* vkImage, Vk_Device* vkDevice, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	static void createShaderModule(Vk_ShaderModule** out, StrView filename, Vk_Device* vkDevice);

	static void createSemaphore(Vk_Semaphore** out, Vk_Device* vkDevice);
	static void createFence(Vk_Fence** out, Vk_Device* vkDevice);


	template<size_t N> static void createImageViews(Vector<VkPtr<Vk_ImageView>, N>& out, const Vector<VkPtr<Vk_Image>, N>& vkImages, Vk_Device* vkDevice, 
													VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	template<size_t N> static u32  createSwapchainImages	(SwapChainImages_Vk_N<N>& out, Vk_Swapchain* vkSwapchain, Vk_Device* vkDevice);
	template<size_t N> static void createSwapchainImageViews(SwapChainImageViews_Vk_N<N>& out, const SwapChainImages_Vk_N<N>& vkImages, Vk_Device* vkDevice, 
															 VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

public:
	template<size_t N> static u32 getAvailableGPUDevicesTo	(Vector<Vk_PhysicalDevice*,		 N>& out, Vk_Instance* vkInstance);
	template<size_t N> static u32 getQueueFaimlyPropertiesTo(Vector<VkQueueFamilyProperties, N>& out, Vk_PhysicalDevice* vkPhyDevice);

	static void getPhyDevicePropertiesTo	(RenderAdapterInfo& info, Vk_PhysicalDevice* phyDevice);
	static void getPhyDeviceFeaturesTo		(RenderAdapterInfo& info, Vk_PhysicalDevice* phyDevice);
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
	static u32 getAvailableExtensionsTo			(Vector<VkExtensionProperties,	s_kLocalSize>& out, bool logAvaliable = false);
	static u32 getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties,	s_kLocalSize>& out, Vk_PhysicalDevice* phyDevice, bool logAvaliable = false);

public:
	void createExtensions			(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension = false);
	void createValidationLayers		(const RenderAdapterInfo& adapterInfo);
	void createPhyDeviceExtensions	(const RenderAdapterInfo& adapterInfo, const Renderer_CreateDesc& cDesc, Vk_PhysicalDevice* phyDevice);

	PFN_vkVoidFunction getExtFunction(const char* funcName) const;

	bool isSupportValidationLayer(const char* validationLayerName) const;

public:
	const Vector<const char*,			s_kLocalSize>&	exts()				const;
	const Vector<const char*,			s_kLocalSize>&	validationLayers()	const;
	const Vector<const char*,			s_kLocalSize>&	phyDeviceExts()		const;
	const StringMap<PFN_vkVoidFunction>&				extFuncTable()		const;
		  StringMap<PFN_vkVoidFunction>&				extFuncTable()		;

	const Vector<VkLayerProperties,		s_kLocalSize>&	availableLayers()			const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availableExts()				const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availablePhyDeviceExts()	const;

private:
	void checkValidationLayersExist();

private:
	Vector<const char*,				s_kLocalSize>	_exts;
	Vector<const char*,				s_kLocalSize>	_validationLayers;
	Vector<const char*,				s_kLocalSize>	_phyDeviceExts;
	StringMap<PFN_vkVoidFunction>					_extFuncTable;

	Vector<VkLayerProperties,		s_kLocalSize>	_availableLayers;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availableExts;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availablePhyDeviceExts;
};

inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::exts()				const		{ return _exts; }
inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::validationLayers()	const		{ return _validationLayers; }
inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::phyDeviceExts()		const		{ return _phyDeviceExts; }
inline const StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::extFuncTable()		const		{ return _extFuncTable; }
inline		 StringMap<PFN_vkVoidFunction>&									 ExtensionInfo_Vk::extFuncTable()					{ return _extFuncTable; }

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
RenderApiUtil_Vk::getAvailableGPUDevicesTo(Vector<Vk_PhysicalDevice*, N>& out, Vk_Instance* vkInstance)
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


