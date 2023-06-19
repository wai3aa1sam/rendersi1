#include "rds_render_api_layer-pch.h"
#include "rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{
#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Impl ---
#endif // 0
#if 1

void RenderApiUtil_Vk::throwIfError(Result ret)
{
	if (_checkError(ret))
	{
		reportError(ret);
		RDS_THROW("VkResult = {}", enumInt(ret));
	}
}

bool RenderApiUtil_Vk::assertIfError(Result ret)
{
	if (_checkError(ret))
	{
		reportError(ret);
		RDS_ASSERT(false);
		return false;
	}
	return true;
}

void RenderApiUtil_Vk::reportError(Result ret)
{
	RDS_ASSERT(_checkError(ret), "reportError(): not an error");
	auto str = getStrFromVkResult(ret);
	RDS_LOG("VkResult(0x{:0X}) {}", sCast<u32>(ret), str);
}

String RenderApiUtil_Vk::getStrFromVkResult(Result ret)
{
	const char* str = string_VkResult(ret);
	return String(str);
	//log_core_debug();
}

bool RenderApiUtil_Vk::_checkError(Result ret)
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

#endif


#if 0
#pragma mark --- rdsExtensionInfo_Vk-Impl ---
#endif // 0
#if 1

u32 
ExtensionInfo_Vk::getAvailableValidationLayersTo	(Vector<VkLayerProperties,		s_kLocalSize>& out)
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
ExtensionInfo_Vk::getAvailableExtensionsTo			(Vector<VkExtensionProperties,	s_kLocalSize>& out)
{
	out.clear();

	u32 extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (!extensionCount)
		return 0;

	out.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, out.data());
	return extensionCount;

	//std::cout << "available extensions:\n";
	//for (const auto& e : out) {
	//	std::cout << '\t' << e.extensionName << '\n';
	//}
}

void 
ExtensionInfo_Vk::createExtensions(const RenderAdapterInfo& adapterInfo)
{
	getAvailableExtensionsTo(_availableExts);
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

bool 
ExtensionInfo_Vk::isSupportValidationLayer(const char* validationLayerName)
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