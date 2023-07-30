#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderApi_Common.h"

#include "rdsRenderer_Vk.h"
#include "rdsVk_Allocator.h"

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
	RDS_CORE_LOG_ERROR("VkResult(0x{:0X}) {}", sCast<u32>(ret), str);
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
	return ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR;
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
	
	if (pCallbackData->messageIdNumber == 0x822806fa) return VK_FALSE; // "UNASSIGNED-BestPractices-vkCreateInstance-specialuse-extension-debugging"
	
	RDS_CORE_LOG_ERROR("Vulkan validation layer: {}\n", pCallbackData->pMessage);
	return VK_FALSE;
}

VkExtent2D 
RenderApiUtil_Vk::toVkExtent2D(const Rect2f& rect2)
{
	VkExtent2D vkExtent = 
	{
		sCast<u32>(rect2.w),
		sCast<u32>(rect2.h)
	};
	return vkExtent;
}

VkExtent2D 
RenderApiUtil_Vk::toVkExtent2D(const Vec2f& vec2)
{
	VkExtent2D vkExtent = 
	{
		sCast<u32>(vec2.x),
		sCast<u32>(vec2.y)
	};
	return vkExtent;
}

VkFormat
RenderApiUtil_Vk::toVkFormat(RenderDataType v)
{
	using SRC = RenderDataType;
	switch (v) 
	{
		case SRC::Int8:			{ return VK_FORMAT_R8_SINT;				} break;
		case SRC::Int8x2:		{ return VK_FORMAT_R8G8_SINT;			} break;
		case SRC::Int8x3:		{ return VK_FORMAT_R8G8B8_SINT;			} break; 
		case SRC::Int8x4:		{ return VK_FORMAT_R8G8B8A8_SINT;		} break;

		case SRC::UInt8:		{ return VK_FORMAT_R8_UINT;				} break;
		case SRC::UInt8x2:		{ return VK_FORMAT_R8G8_UINT;			} break;
		case SRC::UInt8x3:		{ return VK_FORMAT_R8G8B8_UINT;			} break; 
		case SRC::UInt8x4:		{ return VK_FORMAT_R8G8B8A8_UINT;		} break;

		case SRC::Norm8:		{ return VK_FORMAT_R8_SNORM;			} break;
		case SRC::Norm8x2:		{ return VK_FORMAT_R8G8_SNORM;			} break;
		case SRC::Norm8x3:		{ return VK_FORMAT_R8G8B8_SNORM;		} break; 
		case SRC::Norm8x4:		{ return VK_FORMAT_R8G8B8A8_SNORM;		} break;

		case SRC::UNorm8:		{ return VK_FORMAT_R8_UNORM;			} break;
		case SRC::UNorm8x2:		{ return VK_FORMAT_R8G8_UNORM;			} break;
		case SRC::UNorm8x3:		{ return VK_FORMAT_R8G8B8_UNORM;		} break; 
		case SRC::UNorm8x4:		{ return VK_FORMAT_R8G8B8A8_UNORM;		} break;

		case SRC::Int16:		{ return VK_FORMAT_R16_SINT;			} break;
		case SRC::Int16x2:		{ return VK_FORMAT_R16G16_SINT;			} break;
		case SRC::Int16x3:		{ return VK_FORMAT_R16G16B16_SINT;		} break; 
		case SRC::Int16x4:		{ return VK_FORMAT_R16G16B16A16_SINT;	} break;

		case SRC::UInt16:		{ return VK_FORMAT_R16_UINT;			} break;
		case SRC::UInt16x2:		{ return VK_FORMAT_R16G16_UINT;			} break;
		case SRC::UInt16x3:		{ return VK_FORMAT_R16G16B16_UINT;		} break; 
		case SRC::UInt16x4:		{ return VK_FORMAT_R16G16B16A16_UINT;	} break;

		case SRC::Norm16:		{ return VK_FORMAT_R16_SNORM;			} break;
		case SRC::Norm16x2:		{ return VK_FORMAT_R16G16_SNORM;		} break;
		case SRC::Norm16x3:		{ return VK_FORMAT_R16G16B16_SNORM;		} break; 
		case SRC::Norm16x4:		{ return VK_FORMAT_R16G16B16A16_SNORM;	} break;

		case SRC::UNorm16:		{ return VK_FORMAT_R16_UNORM;			} break;
		case SRC::UNorm16x2:	{ return VK_FORMAT_R16G16_UNORM;		} break;
		case SRC::UNorm16x3:	{ return VK_FORMAT_R16G16B16_UNORM;		} break; 
		case SRC::UNorm16x4:	{ return VK_FORMAT_R16G16B16A16_UNORM;	} break;

		case SRC::Int32:		{ return VK_FORMAT_R32_SINT;			} break;
		case SRC::Int32x2:		{ return VK_FORMAT_R32G32_SINT;			} break;
		case SRC::Int32x3:		{ return VK_FORMAT_R32G32B32_SINT;		} break; 
		case SRC::Int32x4:		{ return VK_FORMAT_R32G32B32A32_SINT;	} break;

		case SRC::UInt32:		{ return VK_FORMAT_R32_UINT;			} break;
		case SRC::UInt32x2:		{ return VK_FORMAT_R32G32_UINT;			} break;
		case SRC::UInt32x3:		{ return VK_FORMAT_R32G32B32_UINT;		} break; 
		case SRC::UInt32x4:		{ return VK_FORMAT_R32G32B32A32_UINT;	} break;

			//		case SRC::Norm32:	{ return VK_FORMAT_R32_SNORM;			} break;
			//		case SRC::Norm32x2:	{ return VK_FORMAT_R32G32_SNORM;		} break;
			//		case SRC::Norm32x3:	{ return VK_FORMAT_R32G32B32_SNORM;		} break; 
			//		case SRC::Norm32x4:	{ return VK_FORMAT_R32G32B32A32_SNORM;	} break;

			//case SRC::UNorm32:	{ return VK_FORMAT_R32_UNORM;			} break;
			//case SRC::UNorm32x2:	{ return VK_FORMAT_R32G32_UNORM;		} break;
			//case SRC::UNorm32x3:	{ return VK_FORMAT_R32G32B32_UNORM;		} break; 
			//case SRC::UNorm32x4:	{ return VK_FORMAT_R32G32B32A32_UNORM;	} break;
			// 
			//--
		case SRC::Float16:		{ return VK_FORMAT_R16_SFLOAT;			} break;
		case SRC::Float16x2:	{ return VK_FORMAT_R16G16_SFLOAT;		} break;
		case SRC::Float16x3:	{ return VK_FORMAT_R16G16B16_SFLOAT;	} break; 
		case SRC::Float16x4:	{ return VK_FORMAT_R16G16B16A16_SFLOAT;	} break;
			//---
		case SRC::Float32:		{ return VK_FORMAT_R32_SFLOAT;			} break;
		case SRC::Float32x2:	{ return VK_FORMAT_R32G32_SFLOAT;		} break;
		case SRC::Float32x3:	{ return VK_FORMAT_R32G32B32_SFLOAT;	} break;
		case SRC::Float32x4:	{ return VK_FORMAT_R32G32B32A32_SFLOAT;	} break;
			//---
		default: { throw RDS_ERROR("unsupported RenderDataType"); } break;
	}
}

VkBufferUsageFlagBits 
RenderApiUtil_Vk::toVkBufferUsage(RenderGpuBufferTypeFlags typeFlags)
{
	using SRC = RenderGpuBufferTypeFlags;
	switch (typeFlags)
	{
		case SRC::Vertex:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; }	break;
		case SRC::Index:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT; }		break;
		case SRC::Const:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; }	break;
		case SRC::TransferSrc:	{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT; }		break;
		case SRC::TransferDst:	{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT; }		break;
	}

	RDS_CORE_ASSERT(false, "unsporrted buffer usage");
	return VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
}

VkMemoryPropertyFlags 
RenderApiUtil_Vk::toVkMemoryPropFlags(RenderMemoryUsage memUsage)
{
	VkMemoryPropertyFlags flags = {};
	//if (BitUtil::has(requiredProperties, sCast<VkFlags>(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))) { return RenderMemoryUsage::CpuToGpu;	}
	//if (BitUtil::has(requiredProperties, sCast<VkFlags>(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))) { return RenderMemoryUsage::GpuOnly;	}
	_notYetSupported();
	return flags;
}

u32
RenderApiUtil_Vk::getMemoryTypeIdx(u32 memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties)
{
	auto& vkMemProps = Vk_MemoryContext::instance()->vkMemoryProperties();
	const u32 memoryCount = vkMemProps.memoryTypeCount;
	for (u32 memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) 
	{
		const bool isRequiredMemoryType = BitUtil::hasBit(memoryTypeBitsRequirement, memoryIndex);

		const VkMemoryPropertyFlags properties	= vkMemProps.memoryTypes[memoryIndex].propertyFlags;
		const bool hasRequiredProperties		=  BitUtil::has(properties, requiredProperties);

		if (isRequiredMemoryType && hasRequiredProperties)
			return memoryIndex;
	}
	// failed to find memory type
	RDS_CORE_ASSERT(false, "getMemoryTypeIdx() failed");
	return (u32)(-1);
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
RenderApiUtil_Vk::createSurface(Vk_Surface** out, Vk_Instance_T* vkInstance, const VkAllocationCallbacks* allocCallbacks, NativeUIWindow* window)
{
	#if RDS_OS_WINDOWS

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd			= window->wndHnd();
	createInfo.hinstance	= ::GetModuleHandle(nullptr);		// get handle of current process

	auto ret = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, allocCallbacks, out);

	#else
	#error("createSurface() not support int this platform")
	#endif // RDS_OS_WINDOWS

	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createSwapchain(Vk_Swapchain** out, Vk_Surface* vkSurface, Vk_Device* vkDevice, 
								  const SwapchainInfo_Vk& info, const SwapchainAvailableInfo_Vk& avaInfo, const QueueFamilyIndices& queueFamilyIndices)
{
	//u32 imageCount	= avaInfo.capabilities.minImageCount + 1;
	u32 imageCount	= RenderApiLayerTraits::s_kFrameInFlightCount;
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

	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> idxData;
	if (queueFamilyIndices.graphics != queueFamilyIndices.present)
	{
		idxData.emplace_back(queueFamilyIndices.graphics.value());
		idxData.emplace_back(queueFamilyIndices.present.value());

		createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= sCast<u32>(idxData.size());
		createInfo.pQueueFamilyIndices		= idxData.data();
	}
	else
	{
		createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount	= 0;		// Optional
		createInfo.pQueueFamilyIndices		= nullptr;	// Optional
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
RenderApiUtil_Vk::createImageView(Vk_ImageView_T** out, Vk_Image_T* vkImage, Vk_Device* vkDevice, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
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
RenderApiUtil_Vk::createShaderModule(Vk_ShaderModule** out, StrView filename, Vk_Device* vkDevice)
{
	Vector<u8> bin;
	File::readFile(filename, bin);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize	= bin.size();
	createInfo.pCode	= reinterpret_cast<const u32*>(bin.data());

	auto ret = vkCreateShaderModule(vkDevice, &createInfo, Renderer_Vk::instance()->allocCallbacks(), out);
	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createSemaphore(Vk_Semaphore** out, Vk_Device* vkDevice)
{
	auto* vkAllocCallbacks = Renderer_Vk::instance()->allocCallbacks();

	VkSemaphoreCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	auto ret = vkCreateSemaphore(vkDevice, &cInfo, vkAllocCallbacks, out);	
	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createFence(Vk_Fence** out, Vk_Device* vkDevice)
{
	auto* vkAllocCallbacks = Renderer_Vk::instance()->allocCallbacks();

	VkFenceCreateInfo cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	cInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;			// create with signaled state

	auto ret = vkCreateFence(vkDevice, &cInfo, vkAllocCallbacks, out);			
	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createCommandPool(Vk_CommandPool_T** outVkCmdPool, u32 queueIdx, VkCommandPoolCreateFlags createFlags)
{
	auto* renderer = Renderer_Vk::instance();
	auto* vkDevice = renderer->vkDevice();
	auto* allocCallbacks = renderer->allocCallbacks();

	VkCommandPoolCreateInfo cInfo = {};
	cInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cInfo.flags				= createFlags;
	cInfo.queueFamilyIndex	= queueIdx;

	auto ret = vkCreateCommandPool(vkDevice, &cInfo, allocCallbacks, outVkCmdPool);
	throwIfError(ret);
}

void 
RenderApiUtil_Vk::createCommandBuffer(Vk_CommandBuffer_T** outVkCmdBuf, Vk_CommandPool_T* vkCmdPool, VkCommandBufferLevel vkBufLevel)
{
	auto* renderer = Renderer_Vk::instance();
	auto* vkDevice = renderer->vkDevice();
	//auto* allocCallbacks = renderer->allocCallbacks();

	//static constexpr auto s_kFrameInFlightCount = RenderApiLayerTraits::s_kFrameInFlightCount;
	static constexpr size_t count = 1;
	Vector<Vk_CommandBuffer_T*, count> tmp;
	tmp.resize(count);
	//_vkCommandBuffers.resize(s_kFrameInFlightCount);

	VkCommandBufferAllocateInfo cInfo = {};
	cInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cInfo.commandPool			= vkCmdPool;
	cInfo.level					= vkBufLevel;
	cInfo.commandBufferCount	= count;

	auto ret = vkAllocateCommandBuffers(vkDevice, &cInfo, tmp.data());
	throwIfError(ret);
	//Util::convertToVkPtrs(_vkCommandBuffers, tmp);
	outVkCmdBuf = tmp.data();
}

void 
RenderApiUtil_Vk::createBuffer(Vk_Buffer_T** outBuf, Vk_DeviceMemory** outBufMem, VkDeviceSize size
								, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, QueueTypeFlags queueTypeFlags)
{
	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
	auto* vkAllocCallbacks	= Renderer_Vk::instance()->allocCallbacks();
	auto& vkQueueIndices	= Renderer_Vk::instance()->queueFamilyIndices();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;

	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
	if (queueCount > 1)
	{
		bufferInfo.sharingMode				= VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount	= queueCount;
		bufferInfo.pQueueFamilyIndices		= queueIdices.data();
	}

	auto ret = vkCreateBuffer(vkDev, &bufferInfo, vkAllocCallbacks, outBuf);
	throwIfError(ret);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vkDev, *outBuf, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= getMemoryTypeIdx(memRequirements.memoryTypeBits, properties);

	ret = vkAllocateMemory(vkDev, &allocInfo, vkAllocCallbacks, outBufMem);

	VkDeviceSize offset = 0;
	RDS_CORE_ASSERT(offset % memRequirements.alignment == 0, "not aligned");
	vkBindBufferMemory(vkDev, *outBuf, *outBufMem, offset);
}

void 
RenderApiUtil_Vk::copyBuffer(Vk_Buffer* dstBuffer, Vk_Buffer* srcBuffer, VkDeviceSize size, Vk_CommandPool_T* vkCmdPool, Vk_Queue* vkTransferQueue)
{
	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
	//auto* vkAllocCallbacks	= Renderer_Vk::instance()->allocCallbacks();

	VkResult ret;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool			= vkCmdPool;
	allocInfo.commandBufferCount	= 1;

	Vk_CommandBuffer_T* vkCmdBuf;
	ret = vkAllocateCommandBuffers(vkDev, &allocInfo, &vkCmdBuf);
	throwIfError(ret);
	auto cmdBufLsa = makeLeaveScopeAction(
		[&vkCmdBuf, vkCmdPool, &vkDev]()
		{
			Vk_CommandBuffer_T* vkCmdBufs[] = { vkCmdBuf };
			vkFreeCommandBuffers(vkDev, vkCmdPool, ArraySize<decltype(vkCmdBufs)>, vkCmdBufs);
		}
	);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(vkCmdBuf, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset	= 0; // Optional
	copyRegion.dstOffset	= 0; // Optional
	copyRegion.size			= size;
	vkCmdCopyBuffer(vkCmdBuf, srcBuffer->hnd(), dstBuffer->hnd(), 1, &copyRegion);

	vkEndCommandBuffer(vkCmdBuf);

	{
		VkSubmitInfo2KHR submitInfo = {};

		VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
		cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdBufSubmitInfo.commandBuffer	= vkCmdBuf;

		submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.commandBufferInfoCount	= 1;
		submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

		RDS_LOG_WARN("TODO: add smp for transfer and graphics queue");
		ret = vkQueueSubmit2(vkTransferQueue->hnd(), 1, &submitInfo, VK_NULL_HANDLE);
		throwIfError(ret);
		vkQueueWaitIdle(vkTransferQueue->hnd());
	}
}

void 
RenderApiUtil_Vk::transitionImageLayout(Vk_Image* image, VkFormat vkFormat, VkImageLayout dstLayout, VkImageLayout srcLayout, Vk_Queue* dstQueue, Vk_Queue* srcQueue, Vk_CommandBuffer* vkCmdBuf)
{
	vkCmdBuf->beginRecord(srcQueue);

	VkPipelineStageFlags	srcStage		= {};
	VkPipelineStageFlags	dstStage		= {};
	VkAccessFlags			srcAccessMask	= {};
	VkAccessFlags			dstAccessMask	= {};

	if (dstLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_UNDEFINED ) 
	{
		srcStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;		// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		dstStage			= VK_PIPELINE_STAGE_TRANSFER_BIT;
		srcAccessMask		= 0;
		dstAccessMask		= VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		srcStage		= VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage		= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
		dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_UNDEFINED ) 
	{
		srcStage		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;			// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		dstStage		= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		srcAccessMask	= 0;
		dstAccessMask	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else 
	{
		throwIf(true, "");
	}

	VkImageAspectFlags aspectMask = dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	//if (hasStencilComponent(format)) 
	//	aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	#if 1

	VkImageMemoryBarrier barrier = {};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout						= srcLayout;
	barrier.newLayout						= dstLayout;
	barrier.srcQueueFamilyIndex				= dstQueue ? srcQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= dstQueue ? dstQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= image->hnd();

	barrier.subresourceRange.aspectMask		= aspectMask;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.layerCount		= 1;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	vkCmdPipelineBarrier(
		vkCmdBuf->hnd(),
		srcStage , dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	#else

	VkImageMemoryBarrier2 barrier;
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.oldLayout						= srcLayout;
	barrier.newLayout						= dstLayout;
	barrier.srcQueueFamilyIndex				= dstQueue ? srcQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= dstQueue ? dstQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= image->hnd();

	barrier.subresourceRange.aspectMask		= aspectMask;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.layerCount		= 1;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.srcStageMask  = srcStage;
	barrier.dstStageMask  = dstStage;

	VkDependencyInfo depInfo = {};
	depInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers	= &barrier;
	
	auto vkCmdPipelineBarrier23 = Renderer_Vk::instance()->extInfo().getDeviceExtFunction<PFN_vkCmdPipelineBarrier2>("vkCmdPipelineBarrier2");
	vkCmdPipelineBarrier23(vkCmdBuf->hnd(), &depInfo);

	#endif // 0

	vkCmdBuf->endRecord();
	vkCmdBuf->submit();
	vkCmdBuf->waitIdle();
}

void 
RenderApiUtil_Vk::copyBufferToImage(Vk_Image* dstImage, Vk_Buffer* srcBuf, u32 width, u32 height, Vk_Queue* vkQueue, Vk_CommandBuffer* vkCmdBuf)
{
	vkCmdBuf->beginRecord(vkQueue);

	VkBufferImageCopy region = {};
	region.bufferOffset			= 0;
	region.bufferRowLength		= 0;
	region.bufferImageHeight	= 0;

	region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel		= 0;
	region.imageSubresource.baseArrayLayer	= 0;
	region.imageSubresource.layerCount		= 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
		vkCmdBuf->hnd(),
		srcBuf->hnd(),
		dstImage->hnd(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	vkCmdBuf->endRecord();
	vkCmdBuf->submit();
	vkCmdBuf->waitIdle();
}

void 
RenderApiUtil_Vk::createBuffer(Vk_Buffer& outBuf, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags)
{
	outBuf.create(vkAlloc, allocInfo, size, usage, queueTypeFlags);
}

void 
RenderApiUtil_Vk::createBuffer(Vk_Buffer_T** outBuf, Vk_AllocHnd* allocHnd, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags)
{
	auto& vkQueueIndices	= Renderer_Vk::instance()->queueFamilyIndices();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
	
	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
	if (queueCount > 1)
	{
		bufferInfo.sharingMode				= VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount	= queueCount;
		bufferInfo.pQueueFamilyIndices		= queueIdices.data();
	}

	auto ret = vkAlloc->allocBuf(outBuf, allocHnd, &bufferInfo, allocInfo);
	throwIfError(ret);
}

void
RenderApiUtil_Vk::getPhyDevicePropertiesTo(RenderAdapterInfo& outInfo, Vk_PhysicalDevice* phyDevice)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(phyDevice, &deviceProperties);

	outInfo.adapterName				= deviceProperties.deviceName;
	outInfo.feature.isDiscreteGPU	= deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	VkPhysicalDeviceMemoryProperties deviceMemProperties;
	vkGetPhysicalDeviceMemoryProperties(phyDevice, &deviceMemProperties);

	outInfo.memorySize = deviceMemProperties.memoryHeaps[0].size;

	RDS_LOG("Vulkan:");
	RDS_LOG("\t apiVersion: {}",		deviceProperties.apiVersion);
	RDS_LOG("\t adapterName: {}",		outInfo.adapterName);

	RDS_LOG("\t memoryHeapCount: {}",	deviceMemProperties.memoryHeapCount);
	for (size_t i = 0; i < deviceMemProperties.memoryHeapCount; i++)
	{
		RDS_LOG("\t memorySize: {}",		 deviceMemProperties.memoryHeaps[i].size);
	}

	outInfo.limit.maxSamplerAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
}

void
RenderApiUtil_Vk::getPhyDeviceFeaturesTo(RenderAdapterInfo& outInfo, Vk_PhysicalDevice* phyDevice)
{
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

	RenderAdapterInfo::Feature temp;
	temp.shaderHasFloat64		= deviceFeatures.shaderFloat64;
	temp.hasGeometryShader		= deviceFeatures.geometryShader;
	temp.hasSamplerAnisotropy	= deviceFeatures.samplerAnisotropy;

	outInfo.feature = temp;
}

void RenderApiUtil_Vk::getVkPhyDeviceFeaturesTo(VkPhysicalDeviceFeatures& out, const RenderAdapterInfo& info)
{
	out = {};

	out.samplerAnisotropy = VK_TRUE;

	RDS_CORE_LOG_WARN("TODO: getVkPhyDeviceFeaturesTo()");
	//info.feature.
}

bool
RenderApiUtil_Vk::getSwapchainAvailableInfoTo(SwapchainAvailableInfo_Vk& out, Vk_PhysicalDevice* vkPhyDevice, Vk_Surface* vkSurface)
{
	auto& swInfo = out;

	auto ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhyDevice, vkSurface, &swInfo.capabilities); (void)ret;
	throwIfError(ret);

	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhyDevice, vkSurface, &formatCount, nullptr);
	if (formatCount == 0)
		return false;
	swInfo.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhyDevice, vkSurface, &formatCount, swInfo.formats.data());

	u32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhyDevice, vkSurface, &presentModeCount, nullptr);
	if (presentModeCount == 0)
		return false;
	swInfo.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhyDevice, vkSurface, &presentModeCount, swInfo.presentModes.data());

	return true;
}

#endif


#if 0
#pragma mark --- rdsExtensionInfo_Vk-Impl ---
#endif // 0
#if 1

u32
ExtensionInfo_Vk::getAvailableValidationLayersTo(Vector<VkLayerProperties, s_kLocalSize>& out, bool logAvaliable)
{
	out.clear();

	u32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (!layerCount)
		return 0;

	out.resize(layerCount);
	auto ret = vkEnumerateInstanceLayerProperties(&layerCount, out.data());
	Util::throwIfError(ret);

	if (logAvaliable)
	{
		RDS_LOG("vulkan available InstanceLayer extensions:");
		for (const auto& e : out) 
		{
			RDS_LOG("\t ver: {}, {}", e.specVersion, e.layerName);
		}
	}

	return layerCount;
}

u32
ExtensionInfo_Vk::getAvailableInstanceExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, bool logAvaliable)
{
	out.clear();

	u32 extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (!extensionCount)
		return 0;

	out.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, out.data());

	if (logAvaliable)
	{
		RDS_LOG("vulkan available Instance extensions:");
		for (const auto& e : out) 
		{
			RDS_LOG("\t {}", e.extensionName);
		}
	}
	return extensionCount;
}

u32
ExtensionInfo_Vk::getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, Vk_PhysicalDevice* phyDevice, bool logAvaliable)
{
	auto& availableExtensions = out;
	availableExtensions.clear();

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);

	if (extensionCount == 0)
		return extensionCount;

	availableExtensions.resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, availableExtensions.data());

	if (logAvaliable)
	{
		RDS_LOG("vulkan available Device extensions:");
		for (const auto& e : availableExtensions) 
		{
			RDS_LOG("\t {}", e.extensionName);
		}
	}
	return extensionCount;
}

void
ExtensionInfo_Vk::createInstanceExtensions(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension)
{
	getAvailableInstanceExtensionsTo(_availableExts, logAvaliableExtension);
	auto& out = _instanceExts;
	out.clear();

	out.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
	//out.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME); // deprecated

	if (adapterInfo.isDebug) {
		out.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		// out.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME); // deprecated
	}
	#if RDS_OS_WINDOWS
	out.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#else
	#error("getRequiredExtensions() not support int this platform")
	#endif // RDS_OS_WINDOWS
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
	//out.emplace_back("VK_LAYER_KHRONOS_synchronization2"); // deprecated
	
	//RDS_DEBUG_CALL(checkValidationLayersExist());
}

void
ExtensionInfo_Vk::createPhyDeviceExtensions(const RenderAdapterInfo& adapterInfo, const Renderer_CreateDesc& cDesc, Vk_PhysicalDevice* phyDevice)
{
	getAvailablePhyDeviceExtensionsTo(_availablePhyDeviceExts, phyDevice);

	auto& o = _phyDeviceExts;
	o.clear();

	// TODO: check extension exist
	//o.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME); // deprecated
	o.emplace_back(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
	if (cDesc.isPresent)
	{
		o.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
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