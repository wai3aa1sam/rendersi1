#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"
#include "rdsRenderApi_Include_Vk.h"
#include "rdsVkPtr.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Decl ---
#endif // 0
#if 1


struct RenderApiUtil_Vk : public RenderApiUtil
{
public:
	friend class Renderer_Vk;

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

private:
	static bool _checkError(Result ret);

	// return VK_TRUE, then abort with VK_ERROR_VALIDATION_FAILED_EXT error
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*										pUserData);
};


#if 0
#pragma mark --- rdsExtensionInfo_Vk-Decl ---
#endif // 0
#if 1
struct ExtensionInfo_Vk
{
public:
	using Util		= RenderApiUtil_Vk;
	using SizeType	= RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 12;

public:
	static u32 getAvailableValidationLayersTo	(Vector<VkLayerProperties,		s_kLocalSize>& out);
	static u32 getAvailableExtensionsTo			(Vector<VkExtensionProperties,	s_kLocalSize>& out);

public:
	void createExtensions		(const RenderAdapterInfo& adapterInfo);
	void createValidationLayers	(const RenderAdapterInfo& adapterInfo);

	bool isSupportValidationLayer(const char* validationLayerName);

public:
	const Vector<const char*,			s_kLocalSize>&	exts()					const;
	const Vector<const char*,			s_kLocalSize>&	validationLayers()		const;

	const Vector<VkLayerProperties,		s_kLocalSize>&	availableLayers()		const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availableExts()			const;
	const Vector<VkExtensionProperties,	s_kLocalSize>&	availableDeviceExts()	const;

private:
	void checkValidationLayersExist();

private:
	Vector<const char*,				s_kLocalSize>	_exts;
	Vector<const char*,				s_kLocalSize>	_validationLayers;
	Vector<const char*,				s_kLocalSize>	_deviceExts;
	StringMap<PFN_vkVoidFunction>					_extFuncTable;

	Vector<VkLayerProperties,		s_kLocalSize>	_availableLayers;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availableExts;
	Vector<VkExtensionProperties,	s_kLocalSize>	_availableDeviceExts;
};

inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::exts()				const { return _exts; }
inline const Vector<const char*,			ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::validationLayers()	const { return _validationLayers; }
inline const Vector<VkLayerProperties,		ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availableLayers()	const { return _availableLayers; };
inline const Vector<VkExtensionProperties,	ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availableExts()		const { return _availableExts; };
inline const Vector<VkExtensionProperties,	ExtensionInfo_Vk::s_kLocalSize>& ExtensionInfo_Vk::availableDeviceExts()const { return _availableDeviceExts; };

#endif


#endif


#if 0
#pragma mark --- rdsRenderApiUtil_Vk-Impl ---
#endif // 0
#if 1



#endif

}

#endif // RDS_RENDER_HAS_VULKAN


