#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"
#include "rdsRenderApi_Include_Vk.h"
#include "rdsVkPtr.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

#include "rds_render_api_layer/pass/rdsRenderTarget.h"
#include "rds_render_api_layer/command/rdsRenderCommand.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class RenderDevice_Vk;

class Vk_CommandBuffer;
class Vk_CommandPool;

class RenderGpuBuffer_Vk;

struct RenderDevice_CreateDesc;

struct Vk_RenderApiUtil;

struct Texture_Desc;

#if 1	

struct QueueFamilyIndices 
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kQueueTypeCount = 4;

	QueueFamilyIndices()
	{
		RDS_TODO("rework or modify");
	}

	void clear() 
	{ 
		graphics.reset();
		present.reset();
		transfer.reset();
	}

	u32 getFamilyIdx(QueueTypeFlags flags) const 
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

struct Vk_SwapchainAvailableInfo 
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
struct Vk_SwapchainInfo
{
	Vk_SwapchainInfo() : rect2f() {}
	~Vk_SwapchainInfo() = default;

	//VkSurfaceCapabilitiesKHR	capabilities;
	VkSurfaceFormatKHR			surfaceFormat;
	VkFormat					depthFormat;
	VkPresentModeKHR			presentMode;
	//VkExtent2D					extent;
	Rect2f						rect2f;
	u32							imageCount = 0;
};

#endif


#if 1

struct Vk_CDesc_Base
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Util = Vk_RenderApiUtil;

public:
	Vk_CDesc_Base()		= default;
	~Vk_CDesc_Base()	= default;
};

struct Vk_StageAccess
{
	VkPipelineStageFlags	srcStage	= {};
	VkPipelineStageFlags	dstStage	= {};
	VkAccessFlags			srcAccess	= {};
	VkAccessFlags			dstAccess	= {};
};

#endif // 1


#if 0
#pragma mark --- rdsVk_RenderApiUtil-Decl ---
#endif // 0
#if 1


struct Vk_RenderApiUtil : public RenderApiUtil
{
public:
	friend class RenderDevice_Vk;

public:
	using Base					= RenderApiUtil;
	using Result				= VkResult;
	//using SwapchainAvailInfo	= SwapchainAvailInfo_Vk;

public:
	Vk_RenderApiUtil() = delete;

public:
	static bool isSuccess		(Result ret);
	static void throwIfError	(Result ret);
	static bool assertIfError	(Result ret);
	static void reportError		(Result ret);

	static String getStrFromVkResult(Result ret);

public:
	template<class T> static VkDeviceSize toVkDeviceSize(T v);

	static VkOffset2D toVkOffset2D(const Rect2f& rect2);
	static VkExtent2D toVkExtent2D(const Rect2f& rect2);
	static VkExtent2D toVkExtent2D(const Vec2f&  vec2);
	static Rect2f	  toRect2f	  (const VkExtent2D&  ext2d);

	static VkFormat	toVkFormat(RenderDataType	v);
	static VkFormat	toVkFormat(ColorType		v);
	static VkFormat	toVkFormat_ShaderTexture(VkFormat v);		// to UNORM
	static VkFormat	toVkFormat_srgb			(VkFormat v);

	static VkBufferUsageFlagBits toVkBufferUsage (RenderGpuBufferTypeFlags v);
	static VkBufferUsageFlagBits toVkBufferUsages(RenderGpuBufferTypeFlags v);

	static VkMemoryPropertyFlags toVkMemoryPropFlags(RenderMemoryUsage memUsage);

	static VkIndexType toVkIndexType(RenderDataType idxType);

	static bool isDepthFormat(VkFormat format);
	static bool isDepthOnlyFormat(VkFormat format);
	static bool hasStencilComponent(VkFormat format);
	static bool isVkFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features, RenderDevice_Vk* rdDevVk);

	static VkAttachmentLoadOp	toVkAttachmentLoadOp (RenderTargetLoadOp	v);
	static VkAttachmentLoadOp	toVkAttachmentLoadOp (RenderTargetOp		v);
	static VkAttachmentStoreOp	toVkAttachmentStoreOp(RenderTargetStoreOp	v);
	static VkAttachmentStoreOp	toVkAttachmentStoreOp(RenderTargetOp		v);

	static VkClearValue toVkClearValue(const Color4f& color);
	static VkClearValue toVkClearValue(float depth, u32 stencil);

	static VkShaderStageFlagBits	toVkShaderStageBit	(ShaderStageFlag v);
	static VkShaderStageFlagBits	toVkShaderStageBits	(ShaderStageFlag v);
	static StrView					toShaderStageProfile(ShaderStageFlag v);
	static VkDescriptorType			toVkDescriptorType	(ShaderResourceType v);

	static VkImageSubresourceRange	toVkImageSubresourceRange(const Texture_Desc& desc, u32 baseMip, u32 baseLayer);
	static VkImageSubresourceRange	toVkImageSubresourceRange(const Texture_Desc& desc);

	static VkImageSubresourceLayers	toVkImageSubresourceLayers(const Texture_Desc& desc, u32 mipLevel, u32 baseLayer, u32 layerCount);

	static VkFilter					toVkFilter				(SamplerFilter v);
	static VkSamplerAddressMode		toVkSamplerAddressMode	(SamplerWrap v);
	static VkImageType				toVkImageType			(RenderDataType v);
	static VkImageViewType			toVkImageViewType		(RenderDataType v);
	static VkSampleCountFlagBits	toVkSampleCountFlagBits	(u8 v);

	static Vk_AccessFlags toVkAccessFlag(RenderAccess v);

	static Vk_StageAccess toVkStageAccess(VkImageLayout srcLayout, VkImageLayout dstLayout);
	static Vk_StageAccess toVkStageAccess(RenderGpuBufferTypeFlags srcUsage, RenderGpuBufferTypeFlags dstUsage, RenderAccess srcAccess, RenderAccess dstAccess);

	static VkPipelineStageFlags toVkPipelineStageFlag(RenderGpuBufferTypeFlags usage);
	static VkAccessFlags		toVkAccessFlag(RenderGpuBufferTypeFlags usage, RenderAccess access);


	//static Vk_PipelineStageFlags toVkPipelineStageFlag	(RenderGpuBufferTypeFlags v/*, ShaderStageFlag stage = ShaderStageFlag::All*/);

	static VkCullModeFlagBits	toVkCullMode			(RenderState_Cull v);
	static VkPrimitiveTopology	toVkPrimitiveTopology	(RenderPrimitiveType v);
	static VkCompareOp			toVkCompareOp			(RenderState_DepthTestOp v);
	static VkBlendFactor		toVkBlendFactor			(RenderState_BlendFactor v);
	static VkBlendOp			toVkBlendOp				(RenderState_BlendOp v);

	static VkImageLayout		toVkImageLayout(TextureUsageFlags v);
	static VkImageLayout		toVkImageLayout(TextureUsageFlags v, RenderAccess access);
	static VkImageLayout		toVkImageLayout(TextureUsageFlags v, RenderAccess access, RenderTargetLoadOp  op);
	static VkImageLayout		toVkImageLayout(TextureUsageFlags v, RenderAccess access, RenderTargetStoreOp op);

	template<size_t N> static void getVkClearValuesTo(Vector<VkClearValue, N>& out, const RenderCommand_ClearFramebuffers* value, SizeType colorCount, bool hasDepth);
	
	/*template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& out, T** vkData, u32 n);
	template<class T, size_t N> static void convertToVkPtrs(Vector<VkPtr<T>, N>& dst, const Vector<T*, N>& src);*/

	template<class T, size_t N> static void convertToHnds(Vector<typename T::HndType, N>& dst, Span<T> src);
	template<class T, size_t N> static void convertToHnds(Vector<typename T::HndType, N>& dst, Span<T*> src);
	template<class T, size_t N> static void convertToHnds(Vector<typename T::HndType, N>& dst, const Vector<T, N>& src);
	
public:
	static u32	getMemoryTypeIdx(u32 memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties, RenderDevice_Vk* rdDevVk);
	static void setDebugUtilObjectName(Vk_Device_T* vkDevHnd, VkObjectType vkObjT, const String& name, const void* vkHnd);

public:
	static Vk_Buffer*	toVkBuf		(		RenderGpuBuffer* rdGpuBuf);
	static Vk_Buffer*	toVkBuf		(const	RenderGpuBuffer* rdGpuBuf);

	static Vk_Buffer_T*	toVkBufHnd	(		RenderGpuBuffer* rdGpuBuf);
	static Vk_Buffer_T*	toVkBufHnd	(const  RenderGpuBuffer* rdGpuBuf);

	// for create vk objects
public:
	static void createDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& out);
	static void createSwapchain(Vk_Swapchain_T** out, Vk_Surface_T* vkSurface, Vk_Device_T* vkDevice, const Vk_SwapchainInfo& info, const Vk_SwapchainAvailableInfo& avaInfo, RenderDevice_Vk* rdDevVk);
	//static void createImageView(Vk_ImageView_T** out, Vk_Image_T* vkImage, Vk_Device* vkDevice, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	//template<size_t N> static void createImageViews(Vector<VkPtr<Vk_ImageView>, N>& out, const Vector<VkPtr<Vk_Image>, N>& vkImages, Vk_Device* vkDevice, 
	//												VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);

	//static void createBuffer(Vk_Buffer_T** outBuf, Vk_DeviceMemory** outBufMem, VkDeviceSize size
	//						, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, QueueTypeFlags queueTypeFlags);

	static void createBuffer(Vk_Buffer& outBuf, RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags);

public:
	static void copyBuffer				(Vk_Buffer* dstBuffer, Vk_Buffer* srcBuffer, VkDeviceSize size, Vk_CommandPool_T* vkCmdPool, Vk_Queue* vkTransferQueue, RenderDevice_Vk* rdDevVk);
	static void transitionImageLayout	(Vk_Image* image, VkFormat vkFormat, VkImageLayout dstLayout, VkImageLayout srcLayout, Vk_Queue* dstQueue, Vk_Queue* srcQueue, Vk_CommandBuffer* vkCmdBuf);
	static void copyBufferToImage		(Vk_Image* dstImage, Vk_Buffer* srcBuf, u32 width, u32 height, Vk_Queue* vkQueue, Vk_CommandBuffer* vkCmdBuf);

public:
	template<size_t N> static u32 getAvailableGPUDevicesTo	(Vector<Vk_PhysicalDevice_T*,	 N>& out, Vk_Instance_T* vkInstance);
	template<size_t N> static u32 getQueueFaimlyPropertiesTo(Vector<VkQueueFamilyProperties, N>& out, Vk_PhysicalDevice_T* vkPhyDevice);

	static void getPhyDevicePropertiesTo	(RenderAdapterInfo& outInfo, Vk_PhysicalDevice_T* phyDevice, bool isLogResult = false);
	static bool getSwapchainAvailableInfoTo	(Vk_SwapchainAvailableInfo& out, Vk_PhysicalDevice_T* vkPhydevice, Vk_Surface_T* vkSurface);


private:
	static bool _checkError(Result ret);

	// return VK_TRUE, then abort with VK_ERROR_VALIDATION_FAILED_EXT error
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*										pUserData);

	//static void _getPhyDeviceFeaturesTo(RenderAdapterInfo* outInfo, const VkPhysicalDeviceFeatures& devFeats);
};

template<class T> inline VkDeviceSize Vk_RenderApiUtil::toVkDeviceSize(T v) { RDS_S_ASSERT(IsIntegral<T>); return sCast<VkDeviceSize>(v); }

#endif

#if 0
#pragma mark --- rdsVk_ExtensionInfo-Decl ---
#endif // 0
#if 1

struct Vk_ExtensionInfo
{
public:
	using Util		= Vk_RenderApiUtil;
	using SizeType	= RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 12;

public:
	static u32 getAvailableValidationLayersTo	(Vector<VkLayerProperties,		s_kLocalSize>& out, bool logAvaliable = false);
	static u32 getAvailableInstanceExtensionsTo	(Vector<VkExtensionProperties,	s_kLocalSize>& out, bool logAvaliable = false);
	static u32 getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties,	s_kLocalSize>& out, Vk_PhysicalDevice_T* phyDevice, bool logAvaliable = false);

public:
	void create(RenderDevice_Vk* rdDevVk);

	void createInstanceExtensions	(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension = false);
	void createValidationLayers		(const RenderAdapterInfo& adapterInfo);
	void createPhyDeviceExtensions	(const RenderAdapterInfo& adapterInfo, const RenderDevice_CreateDesc& rdDevCDesc, Vk_PhysicalDevice_T* phyDevice, bool isLogResult = false);

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
	RenderDevice_Vk* _rdDevVk = nullptr;

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
Vk_ExtensionInfo::getInstanceExtFunction(const char* funcName) const
{
	auto& table = _deviceExtFuncTable;
	auto it = table.find(funcName);
	if (it == table.end())
	{
		PFN_vkVoidFunction func = vkGetInstanceProcAddr(_rdDevVk->vkInstance(), funcName);
		if (!func)
			return nullptr;
		constCast<StringMap<PFN_vkVoidFunction>&>(table)[funcName] = func;
		return reinCast<T>(func);
	}
	return reinCast<T>(it->second);
}

template<class T> inline 
T
Vk_ExtensionInfo::getDeviceExtFunction(const char* funcName) const
{
	auto& table = _deviceExtFuncTable;
	auto it = table.find(funcName);
	if (it == table.end())
	{
		PFN_vkVoidFunction func = vkGetDeviceProcAddr(_rdDevVk->vkDevice(), funcName);
		if (!func)
			return nullptr;
		constCast<StringMap<PFN_vkVoidFunction>&>(table)[funcName] = func;
		return reinCast<T>(func);
	}
	return reinCast<T>(it->second);
}

inline const Vector<const char*,			Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::instanceExts()		const		{ return _instanceExts; }
inline const Vector<const char*,			Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::validationLayers()	const		{ return _validationLayers; }
inline const Vector<const char*,			Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::phyDeviceExts()		const		{ return _phyDeviceExts; }

inline const StringMap<PFN_vkVoidFunction>&									 Vk_ExtensionInfo::instanceExtFuncTable()	const		{ return _instanceExtFuncTable; }
inline		 StringMap<PFN_vkVoidFunction>&									 Vk_ExtensionInfo::instanceExtFuncTable()				{ return _instanceExtFuncTable; }
inline const StringMap<PFN_vkVoidFunction>&									 Vk_ExtensionInfo::deviceExtFuncTable()		const		{ return _deviceExtFuncTable; }
inline		 StringMap<PFN_vkVoidFunction>&									 Vk_ExtensionInfo::deviceExtFuncTable()					{ return _deviceExtFuncTable; }

inline const Vector<VkLayerProperties,		Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::availableLayers()	const		{ return _availableLayers; };
inline const Vector<VkExtensionProperties,	Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::availableExts()		const		{ return _availableExts; };
inline const Vector<VkExtensionProperties,	Vk_ExtensionInfo::s_kLocalSize>& Vk_ExtensionInfo::availablePhyDeviceExts()const	{ return _availablePhyDeviceExts; };

#endif

#if 0
#pragma mark --- rdsVk_RenderApiUtil-Impl ---
#endif // 0
#if 1

template<size_t N> inline
void 
Vk_RenderApiUtil::getVkClearValuesTo(Vector<VkClearValue, N>& out, const RenderCommand_ClearFramebuffers* value, SizeType colorCount, bool hasDepth)
{
	RDS_TODO("default clear values");
	RenderCommand_ClearFramebuffers defaultValue;
	defaultValue.color			= Color4f{ 0.1f, 0.1f, 0.1f, 1.0f };
	defaultValue.depthStencil	= Pair<float, u32>{ { 1.0f }, { 0 } };

	auto* clearValue = value ? value : &defaultValue;
	RDS_CORE_ASSERT(clearValue, "");

	out.clear();

	for (SizeType i = 0; i < colorCount; i++)
	{
		auto& e = out.emplace_back();
		e = toVkClearValue(clearValue->color.value());
	}

	if (hasDepth)
	{
		auto& e = out.emplace_back();
		const auto& v = clearValue->depthStencil.value();
		e = toVkClearValue(v.first, v.second);
	}
}
//template<class T, size_t N> inline
//void
//Vk_RenderApiUtil::convertToVkPtrs(Vector<VkPtr<T>, N>& out, T** vkData, u32 n)
//{
//	out.clear();
//	out.resize(n);
//	for (size_t i = 0; i < n; i++)
//	{
//		out[i].reset(vkData[i]);
//	}
//}
//
//template<class T, size_t N> inline
//void 
//Vk_RenderApiUtil::convertToVkPtrs(Vector<VkPtr<T>, N>& dst, const Vector<T*, N>& src)
//{
//	convertToVkPtrs(dst, (T**)src.data(), sCast<u32>(src.size()));
//}

template<class T, size_t N> inline 
void 
Vk_RenderApiUtil::convertToHnds(Vector<typename T::HndType, N>& dst, Span<T> src)
{
	auto n = src.size();
	dst.clear();
	dst.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		dst.emplace_back(src[i].hnd());
	}
}

template<class T, size_t N> inline 
void 
Vk_RenderApiUtil::convertToHnds(Vector<typename T::HndType, N>& dst, Span<T*> src)
{
	auto n = src.size();
	dst.clear();
	dst.reserve(n);
	for (size_t i = 0; i < n; i++)
	{
		auto* hnd = src[i]->hnd();
		dst.emplace_back(hnd);
	}
}

template<class T, size_t N> inline
void 
Vk_RenderApiUtil::convertToHnds(Vector<typename T::HndType, N>& dst, const Vector<T, N>& src)
{
	convertToHnds(dst, src.span());
}


//template<size_t N> inline
//void 
//Vk_RenderApiUtil::createImageViews(Vector<VkPtr<Vk_ImageView>, N>& out, const Vector<VkPtr<Vk_Image>, N>& vkImages, Vk_Device_T* vkDevice,
//								   VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
//{
//	out.clear();
//	out.resize(vkImages.size());
//	for (size_t i = 0; i < vkImages.size(); ++i)
//	{
//		Vk_ImageView* p = nullptr;
//		createImageView(&p, (Vk_Image*)vkImages[i].ptr(), vkDevice, format, aspectFlags, mipLevels);
//		out[i].reset(p);
//	}
//}

template<size_t N> inline
u32
Vk_RenderApiUtil::getAvailableGPUDevicesTo(Vector<Vk_PhysicalDevice_T*, N>& out, Vk_Instance_T* vkInstance)
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
Vk_RenderApiUtil::getQueueFaimlyPropertiesTo(Vector<VkQueueFamilyProperties, N>& out,	Vk_PhysicalDevice_T* vkPhyDevice)
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
#pragma mark --- rdsVk_RenderApiUtil-Impl ---
#endif // 0
#if 1



#endif

}

#endif // RDS_RENDER_HAS_VULKAN


