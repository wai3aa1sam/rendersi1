#include "rds_render_api_layer-pch.h"
#include "rdsRenderApi_Common_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{
#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Impl ---
#endif // 0
#if 1

void
RenderApiUtil_Vk::throwIfError(Result ret)
{
	if (_checkError(ret))
	{
		reportError(ret);
		RDS_THROW("VkResult = {}", enumInt(ret));
	}
}

bool
RenderApiUtil_Vk::assertIfError(Result ret)
{
	if (_checkError(ret))
	{
		reportError(ret);
		RDS_ASSERT(false);
		return false;
	}
	return true;
}

void
RenderApiUtil_Vk::reportError(Result ret)
{
	RDS_ASSERT(_checkError(ret), "reportError(): not an error");
	auto str = getStrFromVkResult(ret);
	RDS_LOG("VkResult(0x{:0X}) {}", sCast<u32>(ret), str);
}

String
RenderApiUtil_Vk::getStrFromVkResult(Result ret)
{
	const char* str = string_VkResult(ret);
	return String(str);
	//log_core_debug();
}

bool
RenderApiUtil_Vk::_checkError(Result ret)
{
	return ret != VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
RenderApiUtil_Vk::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT				messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	// messageSeverity:
	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
	if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		return VK_FALSE;

	RDS_LOG("Vulkan validation layer: {}", pCallbackData->pMessage);
	return VK_FALSE;
}

VkExtent2D 
RenderApiUtil_Vk::getVkExtent2D(const Rect2f& rect2f)
{
	VkExtent2D vkExtent = 
	{
		static_cast<u32>(rect2f.w),
		static_cast<u32>(rect2f.h)
	};
	return vkExtent;
}

void 
RenderApiUtil_Vk::createDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& out)
{
	out = {};

	out.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	out.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	out.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	out.pfnUserCallback = debugCallback;
	out.pUserData = nullptr; // Optional
}

void 
RenderApiUtil_Vk::createSurface(Vk_Surface** out, Vk_Instance* vkInstance, const VkAllocationCallbacks* allocCallbacks, NativeUIWindow* window)
{
	#if RDS_OS_WINDOWS

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd			= window->wndHnd();
	createInfo.hinstance	= ::GetModuleHandle(nullptr);		// get handle of current process

	auto ret = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, allocCallbacks, out);

	#else
	#error("createSurface() not support int this platform")
	#endif // SGE_OS_WINDOWS

	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createSwapchain(Vk_Swapchain** out, Vk_Surface* vkSurface, Vk_Device* vkDevice, 
								  const SwapchainInfo_Vk& info, const SwapchainAvailableInfo_Vk& avaInfo, const QueueFamilyIndices& queueFamilyIndices)
{
	u32 imageCount	= avaInfo.capabilities.minImageCount + 1;
	imageCount		= math::clamp(imageCount, imageCount, avaInfo.capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface			= vkSurface;
	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= info.surfaceFormat.format;
	createInfo.imageColorSpace	= info.surfaceFormat.colorSpace;
	createInfo.imageExtent		= info.extent;
	createInfo.imageArrayLayers = 1;		// For non-stereoscopic-3D applications, this value is 1.
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (queueFamilyIndices.graphics != queueFamilyIndices.present)
	{
		Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> idxData;
		idxData.emplace_back(queueFamilyIndices.graphics.value());
		idxData.emplace_back(queueFamilyIndices.present.value());

		createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= sCast<u32>(idxData.size());
		createInfo.pQueueFamilyIndices		= idxData.data();
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;		// Optional
		createInfo.pQueueFamilyIndices = nullptr;	// Optional
	}

	createInfo.preTransform		= avaInfo.capabilities.currentTransform;
	createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// bending with other window system
	createInfo.presentMode		= info.presentMode;
	createInfo.clipped			= VK_TRUE;
	createInfo.oldSwapchain		= VK_NULL_HANDLE;	// useful when it is invalid

	auto ret = vkCreateSwapchainKHR(vkDevice, &createInfo, Renderer_Vk::instance()->allocCallbacks(), out);
	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createImageView(Vk_ImageView** out, Vk_Image* vkImage, Vk_Device* vkDevice, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image								= vkImage;
	viewInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format								= format;

	viewInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;

	viewInfo.subresourceRange.aspectMask		= aspectFlags;
	viewInfo.subresourceRange.baseMipLevel		= 0;
	viewInfo.subresourceRange.levelCount		= mipLevels;
	viewInfo.subresourceRange.baseArrayLayer	= 0;
	viewInfo.subresourceRange.layerCount		= 1;

	VkResult ret = vkCreateImageView(vkDevice, &viewInfo, Renderer_Vk::instance()->allocCallbacks(), out);
	throwIfError(ret);
}

void
RenderApiUtil_Vk::getPhyDevicePropertiesTo(RenderAdapterInfo& info, Vk_PhysicalDevice* phyDevice)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(phyDevice, &deviceProperties);

	info.adapterName = deviceProperties.deviceName;

	info.feature.isDiscreteGPU = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	VkPhysicalDeviceMemoryProperties deviceMemProperties;
	vkGetPhysicalDeviceMemoryProperties(phyDevice, &deviceMemProperties);

	info.memorySize = deviceMemProperties.memoryHeaps[0].size;
}

void
RenderApiUtil_Vk::getPhyDeviceFeaturesTo(RenderAdapterInfo& info, Vk_PhysicalDevice* phyDevice)
{
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

	RenderAdapterInfo::Feature tmp;
	tmp.shaderHasFloat64	= deviceFeatures.shaderFloat64;
	tmp.hasGeometryShader	= deviceFeatures.geometryShader;

	info.feature = tmp;
}

void RenderApiUtil_Vk::getVkPhyDeviceFeaturesTo(VkPhysicalDeviceFeatures& out, const RenderAdapterInfo& info)
{
	out = {};
	RDS_LOG("TODO: getVkPhyDeviceFeaturesTo()");
	//info.feature.
}

bool
RenderApiUtil_Vk::getSwapchainAvailableInfoTo(SwapchainAvailableInfo_Vk& out, Vk_PhysicalDevice* vkPhydevice, Vk_Surface* vkSurface)
{
	auto& swInfo = out;

	auto ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhydevice, vkSurface, &swInfo.capabilities); (void)ret;
	throwIfError(ret);

	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhydevice, vkSurface, &formatCount, nullptr);
	if (formatCount == 0)
		return false;
	swInfo.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhydevice, vkSurface, &formatCount, swInfo.formats.data());

	u32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhydevice, vkSurface, &presentModeCount, nullptr);
	if (presentModeCount == 0)
		return false;
	swInfo.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhydevice, vkSurface, &presentModeCount, swInfo.presentModes.data());

	return true;
}

#endif


#if 0
#pragma mark --- rdsExtensionInfo_Vk-Impl ---
#endif // 0
#if 1

u32
ExtensionInfo_Vk::getAvailableValidationLayersTo(Vector<VkLayerProperties, s_kLocalSize>& out)
{
	out.clear();

	u32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (!layerCount)
		return 0;

	out.resize(layerCount);
	auto ret = vkEnumerateInstanceLayerProperties(&layerCount, out.data());
	Util::throwIfError(ret);

	return layerCount;
}

u32
ExtensionInfo_Vk::getAvailableExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, bool logExtension)
{
	out.clear();

	u32 extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (!extensionCount)
		return 0;

	out.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, out.data());

	if (logExtension)
	{
		RDS_LOG("vulkan available extensions:");
		for (const auto& e : out) 
		{
			RDS_LOG("\t {}", e.extensionName);
		}
	}
	return extensionCount;
}

u32
ExtensionInfo_Vk::getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, Vk_PhysicalDevice* phyDevice)
{
	auto& availableExtensions = out;
	availableExtensions.clear();

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);

	if (extensionCount == 0)
		return extensionCount;

	availableExtensions.resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, availableExtensions.data());

	return extensionCount;
}

void
ExtensionInfo_Vk::createExtensions(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension)
{
	getAvailableExtensionsTo(_availableExts, logAvaliableExtension);
	auto& out = _exts;
	out.clear();

	out.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
	if (adapterInfo.isDebug) {
		out.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	#if RDS_OS_WINDOWS
	out.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#else
	#error("getRequiredExtensions() not support int this platform")
	#endif // SGE_OS_WINDOWS
}

void
ExtensionInfo_Vk::createValidationLayers(const RenderAdapterInfo& adapterInfo)
{
	getAvailableValidationLayersTo(_availableLayers);
	if (!adapterInfo.isDebug)
	{
		return;
	}

	auto& out = _validationLayers;
	out.clear();
	out.emplace_back("VK_LAYER_KHRONOS_validation");

	RDS_DEBUG_CALL(checkValidationLayersExist());
}

void
ExtensionInfo_Vk::createPhyDeviceExtensions(const RenderAdapterInfo& adapterInfo, const Renderer_CreateDesc& cDesc, Vk_PhysicalDevice* phyDevice)
{
	getAvailablePhyDeviceExtensionsTo(_availablePhyDeviceExts, phyDevice);

	auto& o = _phyDeviceExts;
	o.clear();

	// TODO: check extension exist
	if (cDesc.isPresent)
		o.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

PFN_vkVoidFunction 
ExtensionInfo_Vk::getExtFunction(const char* funcName) const
{
	PFN_vkVoidFunction func = vkGetInstanceProcAddr(Renderer_Vk::instance()->vkInstance(), funcName);
	if (!func)
		return nullptr;
	constCast<ExtensionInfo_Vk&>(*this)._extFuncTable[funcName] = func;
	return func;
}

bool 
ExtensionInfo_Vk::isSupportValidationLayer(const char* validationLayerName) const
{
	RDS_CORE_ASSERT(validationLayers().size() > 0);

	bool islayerFound = false;
	for (const auto& layerProperties : availableLayers()) {
		if (StrUtil::isSame(validationLayerName, layerProperties.layerName)) 
		{
			islayerFound = true;
			break;
		}
	}

	return islayerFound;
}

void 
ExtensionInfo_Vk::checkValidationLayersExist()
{
	for (const auto& layerName : validationLayers())
	{
		throwIf(!isSupportValidationLayer(layerName), "not supported validation layer {}", layerName);
	}
}


#endif

}

#endif // RDS_RENDER_HAS_VULKAN