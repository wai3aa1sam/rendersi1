#include "rds_render_api_layer-pch.h"
#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

#if RDS_VK_VER_1_2
PFN_vkQueueSubmit2KHR vkQueueSubmit2;
#endif

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

	#if RDS_VK_VER_1_2
	vkQueueSubmit2 = extInfo().getDeviceExtFunction<PFN_vkQueueSubmit2KHR>("vkQueueSubmit2KHR");
	#endif

	_memoryContextVk.create(vkDevice(), vkPhysicalDevice(), vkInstance());

	_transferCtxVk.create();
	_tsfCtx = &_transferCtxVk;
}

void
Renderer_Vk::onDestroy()
{
	vkDeviceWaitIdle(vkDevice());
	_tsfCtx->destroy();

	_rdFrames.clear();
	_tsfFrames.clear();

	_memoryContextVk.destroy();
}

void 
Renderer_Vk::waitIdle()
{
	vkDeviceWaitIdle(vkDevice());
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
	appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	#if RDS_VK_VER_1_3
	appInfo.apiVersion = VK_API_VERSION_1_3;
	#elif RDS_VK_VER_1_2
	appInfo.apiVersion = VK_API_VERSION_1_2;
	#else
	#error "unsupported vulkan version";
	#endif // RDS_VK_VER_1_3


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
		VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
		VkValidationFeaturesEXT features = {};
		features.sType							= VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount	= 1;
		features.pEnabledValidationFeatures		= enables;
		
		Util::createDebugMessengerInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		//createInfo.pNext = &features;
	}

	_vkInstance.create(&createInfo, allocCallbacks());

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

	auto func	= _extInfo.getInstanceExtFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
	auto ret	= func(vkInstance(), &createInfo, allocCallbacks(), _vkDebugMessenger.ptrForInit());
	Util::throwIfError(ret);
}

void
Renderer_Vk::createVkPhyDevice(const CreateDesc& cDesc)
{
	static constexpr SizeType s_kLocalSize = 8;
	Vector<Vk_PhysicalDevice*, s_kLocalSize> phyDevices;
	Util::getAvailableGPUDevicesTo(phyDevices, vkInstance());
	throwIf(phyDevices.is_empty(), "cannot find any available gpu");

	NativeUIWindow tmpWindow;
	Util::createTempWindow(tmpWindow);
	Vk_Surface tempSurface;
	tempSurface.create(&tmpWindow);

	i64 largestScore		= NumLimit<i64>::min();
	i64 largestScoreIndex	= NumLimit<i64>::min();
	for (u32 i = 0; i < phyDevices.size(); i++)
	{
		auto* e = phyDevices[i];
		Util::getPhyDevicePropertiesTo(_adapterInfo, e);
		auto score = _rateAndInitVkPhyDevice(_adapterInfo, cDesc, e, &tempSurface);
		if (score > largestScore)
		{
			largestScore = score;
			largestScoreIndex = i;
		}
	}

	_vkPhysicalDevice.reset(phyDevices[largestScoreIndex]);
	_rateAndInitVkPhyDevice(_adapterInfo, cDesc, _vkPhysicalDevice.ptr(), &tempSurface);
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
		VkDeviceQueueCreateInfo	queueCreateInfo = {};
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

		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= _queueFamilyIndices.transfer.value();
		queueCreateInfo.queueCount			= queueCount;
		queueCreateInfo.pQueuePriorities	= &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures phyDeviceFeatures = {};
	Util::getVkPhyDeviceFeaturesTo(phyDeviceFeatures, _adapterInfo);

	// vulkan core feature base on its version, otherwise it is a extension
	#if RDS_VK_VER_1_3

	VkPhysicalDeviceVulkan13Features phyDeviceFeature = {};
	phyDeviceFeature.sType				= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	phyDeviceFeature.synchronization2	= true;

	#elif RDS_VK_VER_1_2

	VkPhysicalDeviceVulkan12Features phyDeviceFeature = {};
	phyDeviceFeature.sType				= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	//phyDeviceFeature.synchronization2	= true;

	#else
	#error "unsupport vulkan version";
	#endif // 0
	
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount		= sCast<u32>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos		= queueCreateInfos.data();
	createInfo.pEnabledFeatures			= &phyDeviceFeatures;
	createInfo.enabledExtensionCount	= sCast<u32>(_extInfo.phyDeviceExts().size());
	createInfo.ppEnabledExtensionNames	= _extInfo.phyDeviceExts().data();
	createInfo.enabledLayerCount		= _adapterInfo.isDebug ? sCast<u32>(_extInfo.validationLayers().size()) : 0;	// ignored in new vk impl
	createInfo.ppEnabledLayerNames		= _extInfo.validationLayers().data();											// ignored in new vk impl
	createInfo.pNext					= &phyDeviceFeature;

	auto ret = vkCreateDevice(_vkPhysicalDevice, &createInfo, allocCallbacks(), _vkDevice.ptrForInit());
	Util::throwIfError(ret);

	/*vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.graphics.value(),	graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.present.value(),	graphicsQueueIdx, _vkPresentQueue.ptrForInit());*/
}

i64	Renderer_Vk::_rateAndInitVkPhyDevice(RenderAdapterInfo& out, const CreateDesc& cDesc, Vk_PhysicalDevice* vkPhyDevice, Vk_Surface* vkSurface)
{
	auto* e = vkPhyDevice;
	_queueFamilyIndices.clear();
	_extInfo.createPhyDeviceExtensions(out, cDesc, e);
	Util::getPhyDeviceFeaturesTo(out, e);
	Util::getQueueFaimlyPropertiesTo(_queueFamilyProperties, vkPhyDevice);
	Util::getSwapchainAvailableInfoTo(_swapchainAvaliableInfo, e, vkSurface->hnd());

	u32 maxTry		= 10;
	u32 curTryIdx	= 0;
	while (curTryIdx < maxTry && !_queueFamilyIndices.isAllUnique())
	{
		for (u32 i = 0; i < _queueFamilyProperties.size(); ++i) 
		{
			auto& prop = _queueFamilyProperties[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhyDevice, i, vkSurface->hnd(), &presentSupport);

			//bool isFoundGraphics	= _queueFamilyIndices.graphics.has_value();
			//bool isFoundPresent		= _queueFamilyIndices.present.has_value();

			if (BitUtil::has(prop.queueFlags, sCast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT)) && !_queueFamilyIndices.isUniqueGraphics()) 
			{
				_queueFamilyIndices.graphics = i;
			}

			if (BitUtil::hasButNot(prop.queueFlags, sCast<VkQueueFlags>(VK_QUEUE_TRANSFER_BIT), sCast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT)) && !_queueFamilyIndices.isUniqueTransfer()) 
			{
				_queueFamilyIndices.transfer = i;
			}

			if (presentSupport && !_queueFamilyIndices.isUniquePresent())
			{
				_queueFamilyIndices.present = i;
			}
		}
		curTryIdx++;
	}

	return _rateVkPhyDevice(out);
}

i64 Renderer_Vk::_rateVkPhyDevice(const RenderAdapterInfo& info)
{
	i64 score = 0;

	if (_queueFamilyIndices.graphics.has_value())	score += 10000;
	if (_queueFamilyIndices.present.has_value())	score += 10000;
	if (_queueFamilyIndices.transfer.has_value())	score += 10000;

	if (info.feature.hasSamplerAnisotropy) score += 100;

	return score;
}


#endif


}

#endif // RDS_RENDER_HAS_VULKAN