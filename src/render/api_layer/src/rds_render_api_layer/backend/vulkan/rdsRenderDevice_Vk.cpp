#include "rds_render_api_layer-pch.h"
#include "rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderDevice_Vk-Impl ---
#endif // 0
#if 1


RenderDevice_Vk::RenderDevice_Vk()
	: Base()
{
}

RenderDevice_Vk::~RenderDevice_Vk()
{
	destroy();
	RDS_LOG_DEBUG("~RenderDevice_Vk()");
}

void
RenderDevice_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	createVkInstance();

	Vk_PhysicalDeviceFeatures phyDevFeats;
	createVkPhyDevice(&phyDevFeats, cDesc);
	createVkDevice(phyDevFeats);
	
	loadVkInstFn(_vkExtInfo);
	loadVkDevFn(_vkExtInfo);

	_vkMemoryContext.create(vkDevice(), vkPhysicalDevice(), vkInstance());

	{
		auto tsfCtxCDesc = TransferContext::makeCDesc();
		tsfCtxCDesc._internal_create(this);
		_transferCtxVk.create(tsfCtxCDesc);
		_tsfCtx = &_transferCtxVk;
	}

	{
		auto bindlessRscVkcDesc = BindlessResources_Vk::makeCDesc();
		bindlessRscVkcDesc._internal_create(this);
		_bindlessRscsVk.create(bindlessRscVkcDesc);
		_bindlessRscs = &_bindlessRscsVk;
	}

	_setDebugName();
}

void
RenderDevice_Vk::onDestroy()
{
	waitIdle();

	_bindlessRscsVk.destroy();

	_tsfCtx->destroy();

	_rdFrames.clear();
	_tsfFrames.clear();

	_vkMemoryContext.destroy();

	RDS_TODO("since VkAllocCallbacks is inside _vkMemoryContext, destroyed will cause VkDevice, ... cannot destroy, or they just use nullptr for VkAllocCallbacks");;

	_vkDevice.destroy(this);
	_vkPhysicalDevice.destroy();
	_vkDebugMessenger.destroy(this);
	_vkInstance.destroy(this);

	Base::onDestroy();
}

void 
RenderDevice_Vk::onNextFrame()
{
	Base::onNextFrame();
	_transferCtxVk.vkTransferFrame().clear();
}

void 
RenderDevice_Vk::waitIdle()
{
	vkDeviceWaitIdle(vkDevice());
}

void
RenderDevice_Vk::createVkInstance()
{
	_vkExtInfo.createValidationLayers(_adapterInfo);
	_vkExtInfo.createInstanceExtensions(_adapterInfo);

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
	createInfo.enabledExtensionCount	= sCast<u32>(_vkExtInfo.instanceExts().size());
	createInfo.ppEnabledExtensionNames	= _vkExtInfo.instanceExts().data();
	createInfo.enabledLayerCount		= sCast<u32>(_vkExtInfo.validationLayers().size());
	createInfo.ppEnabledLayerNames		= _vkExtInfo.validationLayers().data();

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

	_vkInstance.create(&createInfo, allocCallbacks(), this);
	_vkExtInfo.create(this);

	if (_adapterInfo.isDebug)
	{
		_vkDebugMessenger.create(this);
	}
}

void
RenderDevice_Vk::createVkPhyDevice(Vk_PhysicalDeviceFeatures* oVkPhyDevFeats, const CreateDesc& cDesc)
{
	static constexpr SizeType s_kLocalSize = 8;
	Vector<Vk_PhysicalDevice_T*, s_kLocalSize> phyDevices;
	Util::getAvailableGPUDevicesTo(phyDevices, vkInstance());
	throwIf(phyDevices.is_empty(), "cannot find any available gpu");

	NativeUIWindow tmpWindow;
	Util::createTempWindow(tmpWindow);
	Vk_Surface tempSurface;
	tempSurface.create(&tmpWindow, this);
	auto leaveScopeAction = makeLeaveScopeAction(
		[&]()
		{
			tempSurface.destroy(nullptr, this);
		}
	);

	i64 largestScore		= NumLimit<i64>::min();
	i64 largestScoreIndex	= NumLimit<i64>::min();
	for (u32 i = 0; i < phyDevices.size(); i++)
	{
		Vk_PhysicalDeviceFeatures tempVkPhyDevFeats;

		auto* e = phyDevices[i];
		auto score = _rateAndInitVkPhyDevice(&tempVkPhyDevFeats, &_adapterInfo, cDesc, e, tempSurface.hnd());
		if (score > largestScore)
		{
			largestScore = score;
			largestScoreIndex = i;
			*oVkPhyDevFeats = tempVkPhyDevFeats;
		}
	}

	_vkPhysicalDevice.create(phyDevices[largestScoreIndex]);
	_rateAndInitVkPhyDevice(oVkPhyDevFeats, &_adapterInfo, cDesc, _vkPhysicalDevice.hnd(), tempSurface.hnd(), true);
}

void 
RenderDevice_Vk::createVkDevice(const Vk_PhysicalDeviceFeatures& vkPhyDevFeats)
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

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount		= sCast<u32>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos		= queueCreateInfos.data();
	//createInfo.pEnabledFeatures			= &vkPhyDevFeats.vkPhyDevFeatures()->features;
	createInfo.enabledExtensionCount	= sCast<u32>(_vkExtInfo.phyDeviceExts().size());
	createInfo.ppEnabledExtensionNames	= _vkExtInfo.phyDeviceExts().data();
	createInfo.enabledLayerCount		= _adapterInfo.isDebug ? sCast<u32>(_vkExtInfo.validationLayers().size()) : 0;	// ignored in new vk impl
	createInfo.ppEnabledLayerNames		= _vkExtInfo.validationLayers().data();											// ignored in new vk impl
	createInfo.pNext					= vkPhyDevFeats.vkPhyDevFeatures();

	auto ret = vkCreateDevice(_vkPhysicalDevice.hnd(), &createInfo, allocCallbacks(), _vkDevice.hndForInit());
	Util::throwIfError(ret);

	/*vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.graphics.value(),	graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(_vkDevice, _queueFamilyIndices.present.value(),	graphicsQueueIdx, _vkPresentQueue.ptrForInit());*/
}

i64	RenderDevice_Vk::_rateAndInitVkPhyDevice(Vk_PhysicalDeviceFeatures* oVkPhyDevFeats, RenderAdapterInfo* out, const CreateDesc& cDesc, Vk_PhysicalDevice_T* vkPhyDevHnd, Vk_Surface_T* vkSurfHnd, bool isLogResult)
{
	_queueFamilyIndices.clear();

	Util::getPhyDevicePropertiesTo(*out, vkPhyDevHnd, isLogResult);
	_vkExtInfo.createPhyDeviceExtensions(*out, cDesc, vkPhyDevHnd);
	oVkPhyDevFeats->create(out, vkPhyDevHnd);
	Util::getQueueFaimlyPropertiesTo(_vkQueueFamilyProps, vkPhyDevHnd);
	Util::getSwapchainAvailableInfoTo(_vkSwapchainAvaliableInfo, vkPhyDevHnd, vkSurfHnd);

	u32 maxTry		= 10;
	u32 curTryIdx	= 0;
	while (curTryIdx < maxTry && !_queueFamilyIndices.isAllUnique())
	{
		for (u32 i = 0; i < _vkQueueFamilyProps.size(); ++i) 
		{
			auto& prop = _vkQueueFamilyProps[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhyDevHnd, i, vkSurfHnd, &presentSupport);

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

	return _rateVkPhyDevice(*out);
}

i64 RenderDevice_Vk::_rateVkPhyDevice(const RenderAdapterInfo& info)
{
	i64 score = 0;

	if (_queueFamilyIndices.graphics.has_value())	score += 10000;
	if (_queueFamilyIndices.present.has_value())	score += 10000;
	if (_queueFamilyIndices.transfer.has_value())	score += 10000;

	if (info.feature.hasSamplerAnisotropy) score += 100;

	return score;
}

void 
RenderDevice_Vk::loadVkInstFn(Vk_ExtensionInfo& vkExtInfo)
{
	#define RDS_VK_LOAD_INST_FN_EXT(VAR, FALLBACK_FN) \
	VAR = vkExtInfo.getInstanceExtFunction<RDS_CONCAT(RDS_CONCAT(PFN_, VAR), EXT)>(RDS_CONCAT_TO_STR(VAR, EXT)); \
	if (VAR == nullptr) \
	{ \
		RDS_CORE_LOG_WARN("cannot load" #VAR); \
		VAR = FALLBACK_FN; \
	} \
	// ---

	#define RDS_VK_LOAD_INST_FN_KHR(var, fn) \
	var = vkExtInfo.getDeviceExtFunction<fn>(RDS_STRINGIFY(fn)); throwIf(!var, "cannot load vk fn: {}", RDS_STRINGIFY(fn)) \
	// ---
	
	RDS_VK_LOAD_INST_FN_EXT(vkSetDebugUtilsObjectName,	[](VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo) { return VK_SUCCESS; });
	RDS_VK_LOAD_INST_FN_EXT(vkDebugMarkerSetObjectTag,	[](VkDevice device, const VkDebugMarkerObjectTagInfoEXT* pTagInfo)	{ return VK_SUCCESS; });
	RDS_VK_LOAD_INST_FN_EXT(vkCmdBeginDebugUtilsLabel,	[](VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) {});
	RDS_VK_LOAD_INST_FN_EXT(vkCmdEndDebugUtilsLabel,	[](VkCommandBuffer commandBuffer) {});
	RDS_VK_LOAD_INST_FN_EXT(vkCmdInsertDebugUtilsLabel,	[](VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) {});

	//RDS_VK_LOAD_INST_FN_KHR(vkCmdPipelineBarrier2_khr, PFN_vkCmdPipelineBarrier2KHR);

	#undef RDS_VK_LOAD_INST_FN_EXT
	#undef RDS_VK_LOAD_INST_FN_KHR
}

void 
RenderDevice_Vk::loadVkDevFn(Vk_ExtensionInfo& vkExtInfo)
{
	#define RDS_VK_LOAD_DV_FN(VAR, FALLBACK_FN) \
	VAR = vkExtInfo.getDeviceExtFunction<RDS_CONCAT(RDS_CONCAT(PFN_, VAR), KHR)>(RDS_CONCAT_TO_STR(VAR, KHR)) \
	if (VAR == nullptr) \
	{ \
		VAR = FALLBACK_FN; \
	} \
	// ---

	#if RDS_VK_VER_1_2
	RDS_VK_LOAD_DV_FN(vkQueueSubmit2, [](VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence) { return VK_SUCCESS; });
	#endif

	#undef RDS_VK_LOAD_DEV_FN
}

void 
RenderDevice_Vk::_setDebugName()
{
	//RDS_VK_SET_DEBUG_NAME(_vkInstance);	// set debug name for instance will crash RenderDoc

	if (adapterInfo().isDebug)
	{
		RDS_VK_SET_DEBUG_NAME(_vkDebugMessenger, debugName());
	}
	RDS_VK_SET_DEBUG_NAME(_vkPhysicalDevice, debugName());
	RDS_VK_SET_DEBUG_NAME(_vkDevice, debugName());


}

#endif

#if 0
#pragma mark --- rdsVk_PhysicalDeviceFeatures-Impl ---
#endif // 0
#if 1

void 
Vk_PhysicalDeviceFeatures::create(RenderAdapterInfo* outInfo, Vk_PhysicalDevice_T* vkPhyDevHnd)
{
	outInfo->feature.bindless = true;
	_getRenderApaterFeaturesTo(outInfo);

	// vulkan core feature base on its version, otherwise it is a extension

	#if RDS_VK_VER_1_3

	VkPhysicalDeviceVulkan13Features& vkPhyDevVkFeats13 = _vkPhyDevVkFeats13;
	vkPhyDevVkFeats13 = {};
	vkPhyDevVkFeats13.sType				= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vkPhyDevVkFeats13.synchronization2	= VK_TRUE;
	vkPhyDevVkFeats13.pNext				= nullptr;

	#else
	#error "unsupport vulkan version";
	#endif // 0

	VkPhysicalDeviceVulkan12Features& vkPhyDevVkFeats12 = _vkPhyDevVkFeats12;
	vkPhyDevVkFeats12 = {};
	vkPhyDevVkFeats12.sType	= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vkPhyDevVkFeats12.bufferDeviceAddress									= VK_TRUE;
	vkPhyDevVkFeats12.timelineSemaphore										= VK_TRUE;

	vkPhyDevVkFeats12.descriptorBindingUniformBufferUpdateAfterBind			= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingSampledImageUpdateAfterBind			= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingStorageImageUpdateAfterBind			= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingStorageBufferUpdateAfterBind			= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingUniformTexelBufferUpdateAfterBind	= VK_TRUE;		
	vkPhyDevVkFeats12.descriptorBindingStorageTexelBufferUpdateAfterBind	= VK_TRUE;		
	vkPhyDevVkFeats12.descriptorBindingUpdateUnusedWhilePending				= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingPartiallyBound						= VK_TRUE;
	vkPhyDevVkFeats12.descriptorBindingVariableDescriptorCount				= VK_TRUE;
	vkPhyDevVkFeats12.runtimeDescriptorArray								= VK_TRUE;
	vkPhyDevVkFeats12.shaderSampledImageArrayNonUniformIndexing				= VK_TRUE;
	vkPhyDevVkFeats12.shaderStorageBufferArrayNonUniformIndexing			= VK_TRUE;
	vkPhyDevVkFeats12.shaderStorageImageArrayNonUniformIndexing				= VK_TRUE;

	vkPhyDevVkFeats12.pNext	= &vkPhyDevVkFeats13;

	//VkPhysicalDeviceVulkan11Features& vkPhyDevVkFeats11 = _vkPhyDevVkFeats11;
	//vkPhyDevVkFeats11.pNext	= &vkPhyDevVkFeats12;

	VkPhysicalDeviceDescriptorIndexingFeatures vkDescrIdxFeats = {};		// already include in VkPhysicalDeviceVulkan12Features, only for checking
	vkDescrIdxFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	vkDescrIdxFeats.pNext = nullptr;

	auto& vkPhyDevFeats = _vkPhyDevFeats2;
	vkPhyDevFeats = {};
	vkPhyDevFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	vkPhyDevFeats.pNext = &vkDescrIdxFeats;
	/*vkPhyDevFeats.features.shaderUniformBufferArrayDynamicIndexing	= VK_TRUE;
	vkPhyDevFeats.features.shaderSampledImageArrayDynamicIndexing	= VK_TRUE;
	vkPhyDevFeats.features.shaderStorageBufferArrayDynamicIndexing	= VK_TRUE;
	vkPhyDevFeats.features.shaderStorageImageArrayDynamicIndexing	= VK_TRUE;*/

	vkGetPhysicalDeviceFeatures2(vkPhyDevHnd, &vkPhyDevFeats);
	vkPhyDevFeats.pNext = &vkPhyDevVkFeats12;

	RDS_CORE_ASSERT(vkDescrIdxFeats.shaderSampledImageArrayNonUniformIndexing);
	RDS_CORE_ASSERT(vkDescrIdxFeats.descriptorBindingSampledImageUpdateAfterBind);
	RDS_CORE_ASSERT(vkDescrIdxFeats.shaderUniformBufferArrayNonUniformIndexing);
	RDS_CORE_ASSERT(vkDescrIdxFeats.descriptorBindingUniformBufferUpdateAfterBind);
	RDS_CORE_ASSERT(vkDescrIdxFeats.shaderStorageBufferArrayNonUniformIndexing);
	RDS_CORE_ASSERT(vkDescrIdxFeats.descriptorBindingStorageBufferUpdateAfterBind);
	RDS_CORE_ASSERT(vkDescrIdxFeats.runtimeDescriptorArray);
}

void 
Vk_PhysicalDeviceFeatures::_getRenderApaterFeaturesTo(RenderAdapterInfo* outInfo)
{
	const auto& devFeats = this->_vkPhyDevFeats2.features;

	RenderAdapterInfo::Feature temp;
	temp.shaderHasFloat64		= devFeats.shaderFloat64;
	temp.hasGeometryShader		= devFeats.geometryShader;
	temp.hasSamplerAnisotropy	= devFeats.samplerAnisotropy;
	temp.hasWireframe			= devFeats.fillModeNonSolid;

	outInfo->feature = temp;
}

VkPhysicalDeviceFeatures2*			Vk_PhysicalDeviceFeatures::vkPhyDevFeatures()	const { return &_vkPhyDevFeats2; }
Vk_PhysicalDeviceVulkanFeatures*	Vk_PhysicalDeviceFeatures::vkPhyDevVkFeatures() const 
{
	#if RDS_VK_VER_1_3
	return &_vkPhyDevVkFeats13;
	#elif
	return &_vkPhyDevVkFeats12;
	#endif
}

#endif

}

#endif // RDS_RENDER_HAS_VULKAN