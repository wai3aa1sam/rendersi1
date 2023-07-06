#include "rds_render_api_layer-pch.h"
#include "rdsRenderer_Vk.h"
#include "rdsRenderContext_Vk.h"

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
	createVkPhyDevice(cDesc);
	createVkDevice();
}

void
Renderer_Vk::onDestroy()
{

}

void
Renderer_Vk::createVkInstance()
{
	_extInfo.createValidationLayers(_adapterInfo);
	_extInfo.createInstanceExtensions(_adapterInfo);

	VkApplicationInfo appInfo = {};
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext				= nullptr;
	appInfo.pApplicationName	= RenderApiLayerTraits::s_appName;
	appInfo.pEngineName			= RenderApiLayerTraits::s_engineName;
	// specific application for driver optimization
	//appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	//appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo			= &appInfo;
	createInfo.enabledExtensionCount	= sCast<u32>(_extInfo.instanceExts().size());
	createInfo.ppEnabledExtensionNames	= _extInfo.instanceExts().data();
	createInfo.enabledLayerCount		= sCast<u32>(_extInfo.validationLayers().size());
	createInfo.ppEnabledLayerNames		= _extInfo.validationLayers().data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};	// don't destroy in scope before vkCreateInstance()
	if (_adapterInfo.isDebug)
	{
		Util::createDebugMessengerInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}

	VkResult ret = vkCreateInstance(&createInfo, allocCallbacks(), _vkInstance.ptrForInit());

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

	if (_adapterInfo.isDebug)
	{
		createVkDebugMessenger();
	}
}

void 
Renderer_Vk::createVkDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	Util::createDebugMessengerInfo(createInfo);

	auto func	= reinCast<PFN_vkCreateDebugUtilsMessengerEXT>(_extInfo.getInstanceExtFunction("vkCreateDebugUtilsMessengerEXT"));
	auto ret	= func(_vkInstance, &createInfo, allocCallbacks(), _vkDebugMessenger.ptrForInit());
	Util::throwIfError(ret);
}

void
Renderer_Vk::createVkPhyDevice(const CreateDesc& cDesc)
{
	static constexpr SizeType s_kLocalSize = 8;
	Vector<Vk_PhysicalDevice*, s_kLocalSize> phyDevices;
	Util::getAvailableGPUDevicesTo(phyDevices, _vkInstance);
	throwIf(phyDevices.is_empty(), "cannot find any available gpu");

	NativeUIWindow tmpWindow;
	Util::createTempWindow(tmpWindow);
	VkPtr<Vk_Surface> tmpSurface;
	Util::createSurface(tmpSurface.ptrForInit(), _vkInstance, allocCallbacks(), &tmpWindow);

	i64 largestScore		= NumLimit<i64>::min();
	i64 largestScoreIndex	= NumLimit<i64>::min();
	for (u32 i = 0; i < phyDevices.size(); i++)
	{
		auto* e = phyDevices[i];
		auto score = _rateAndInitVkPhyDevice(_adapterInfo, cDesc, e, tmpSurface);
		if (score > largestScore)
		{
			largestScore = score;
			largestScoreIndex = i;
		}
	}

	_vkPhysicalDevice.reset(phyDevices[largestScoreIndex]);
	_rateAndInitVkPhyDevice(_adapterInfo, cDesc, _vkPhysicalDevice.ptr(), tmpSurface);
	Util::getPhyDevicePropertiesTo(_adapterInfo, _vkPhysicalDevice);
}

void 
Renderer_Vk::createVkDevice()
{
	Vector<VkDeviceQueueCreateInfo, QueueFamilyIndices::s_kQueueTypeCount>	queueCreateInfos;
	float	queuePriority		= 1.0f;
	u32		queueCount			= 1;
	//u32		graphicsQueueIdx	= 0;

	//for (auto queueFamily : queueFamilyIndices) 
	{
		VkDeviceQueueCreateInfo				queueCreateInfo{};
		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= _queueFamilyIndices.graphics.value();
		queueCreateInfo.queueCount			= queueCount;
		queueCreateInfo.pQueuePriorities	= &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);

		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= _queueFamilyIndices.present.value();
		queueCreateInfo.queueCount			= queueCount;
		queueCreateInfo.pQueuePriorities	= &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures phyDeviceFeatures = {};
	Util::getVkPhyDeviceFeaturesTo(phyDeviceFeatures, _adapterInfo);

	VkPhysicalDeviceVulkan13Features phyDeviceFeature13 = {};
	phyDeviceFeature13.sType			= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	phyDeviceFeature13.synchronization2 = true;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount		= sCast<u32>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos		= queueCreateInfos.data();
	createInfo.pEnabledFeatures			= &phyDeviceFeatures;
	createInfo.enabledExtensionCount	= sCast<u32>(_extInfo.phyDeviceExts().size());
	createInfo.ppEnabledExtensionNames	= _extInfo.phyDeviceExts().data();
	createInfo.enabledLayerCount		= _adapterInfo.isDebug ? sCast<u32>(_extInfo.validationLayers().size()) : 0;	// ignored in new vk impl
	createInfo.ppEnabledLayerNames		= _extInfo.validationLayers().data();											// ignored in new vk impl
	createInfo.pNext					= &phyDeviceFeature13;

	auto ret = vkCreateDevice(_vkPhysicalDevice, &createInfo, allocCallbacks(), _vkDevice.ptrForInit());
	Util::throwIfError(ret);

	/*vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.graphics.value(),	graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.present.value(),	graphicsQueueIdx, _vkPresentQueue.ptrForInit());*/
}

i64	Renderer_Vk::_rateAndInitVkPhyDevice(RenderAdapterInfo& out, const CreateDesc& cDesc, Vk_PhysicalDevice* vkPhyDevice, Vk_Surface* vkSurface)
{
	auto* e = vkPhyDevice;
	_extInfo.createPhyDeviceExtensions(out, cDesc, e);
	Util::getPhyDeviceFeaturesTo(out, e);
	Util::getQueueFaimlyPropertiesTo(_queueFamilyProperties, vkPhyDevice);
	Util::getSwapchainAvailableInfoTo(_swapchainAvaliableInfo, e, vkSurface);

	for (u32 i = 0; i < _queueFamilyProperties.size(); ++i) 
	{
		auto prop = _queueFamilyProperties[i];

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkPhyDevice, i, vkSurface, &presentSupport);

		if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			_queueFamilyIndices.graphics = i;
		}

		if (presentSupport)
		{
			_queueFamilyIndices.present = i;
		}

		i++;
	}

	return _rateVkPhyDevice(out);
}

i64 Renderer_Vk::_rateVkPhyDevice(const RenderAdapterInfo& info)
{
	throwIf(!_queueFamilyIndices.graphics.has_value(),	"no graphics queue family");
	throwIf(!_queueFamilyIndices.present.has_value(),	"no graphics queue family");
	return 0;
}


#endif


}

#endif // RDS_RENDER_HAS_VULKAN