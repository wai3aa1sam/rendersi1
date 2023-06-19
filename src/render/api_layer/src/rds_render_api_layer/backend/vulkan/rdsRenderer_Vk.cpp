#include "rds_render_api_layer-pch.h"
#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderer_Vk-Impl ---
#endif // 0
#if 1


Renderer_Vk::Renderer_Vk()
	: Base()
{
}

Renderer_Vk::~Renderer_Vk()
{
	destroy();
}

void 
Renderer_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	createVkInstance();
}

void 
Renderer_Vk::onDestroy()
{

}

void 
Renderer_Vk::createVkInstance()
{
	_extInfo.createValidationLayers(_adapterInfo);
	_extInfo.createExtensions(_adapterInfo);

	VkApplicationInfo appInfo = {};
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext				= nullptr;
	appInfo.pApplicationName	= RenderApiLayerTraits::s_appName;
	appInfo.pEngineName			= RenderApiLayerTraits::s_engineName;
	// specific application for driver optimization
	//appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	//appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion			= VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo			= &appInfo;
	createInfo.enabledExtensionCount	= sCast<u32>(_extInfo.exts().size());
	createInfo.ppEnabledExtensionNames	= _extInfo.exts().data();
	createInfo.enabledLayerCount		= sCast<u32>(_extInfo.validationLayers().size());
	createInfo.ppEnabledLayerNames		= _extInfo.validationLayers().data();
	createInfo.enabledExtensionCount	= 0;
	createInfo.ppEnabledExtensionNames	= nullptr;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};	// don't destroy in scope before vkCreateInstance()
	if (_adapterInfo.isDebug)
	{
		debugCreateInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity	= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = Util::debugCallback;
		debugCreateInfo.pUserData		= nullptr; // Optional

		createInfo.pNext				= (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}

	VkResult ret = vkCreateInstance(&createInfo, _memoryContext.allocationCallbacks(), _vkInstance.ptrForInit());

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

#endif
}

#endif // RDS_RENDER_HAS_VULKAN