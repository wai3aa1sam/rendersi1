#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderApi_Common.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/shader/rdsRenderState.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{
#if 0
#pragma mark --- rdsVk_RenderApiUtil-Impl ---
#endif // 0
#if 1

bool 
Vk_RenderApiUtil::isSuccess(Result ret)
{
	return !_checkError(ret);
}

void
Vk_RenderApiUtil::throwIfError(Result ret)
{
	if (_checkError(ret))
	{
		reportError(ret);
		RDS_THROW("VkResult = {}", enumInt(ret));
	}
}

bool
Vk_RenderApiUtil::assertIfError(Result ret)
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
Vk_RenderApiUtil::reportError(Result ret)
{
	RDS_ASSERT(_checkError(ret), "reportError(): not an error");
	auto str = getStrFromVkResult(ret);
	RDS_CORE_LOG_ERROR("VkResult(0x{:0X}) {}", sCast<u32>(ret), str);
}

String
Vk_RenderApiUtil::getStrFromVkResult(Result ret)
{
	const char* str = string_VkResult(ret);
	return String(str);
	//log_core_debug();
}

bool
Vk_RenderApiUtil::_checkError(Result ret)
{
	return ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
Vk_RenderApiUtil::debugCallback(
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
	
	if (pCallbackData->messageIdNumber == 0x822806fa) { RDS_WARN_ONCE("bypassed vulkan error: UNASSIGNED-BestPractices-vkCreateInstance-specialuse-extension-debugging"); return VK_FALSE; }
	if (pCallbackData->messageIdNumber == 0xfc68be96) { RDS_WARN_ONCE("bypassed vulkan error: SPIR-V Extension SPV_GOOGLE_hlsl_functionality1 was declared, but one of the following requirements is required (VK_GOOGLE_hlsl_functionality1)"); return VK_FALSE; } 

	RDS_CORE_LOG_ERROR("Vulkan validation layer: {}\n", pCallbackData->pMessage);
	return VK_FALSE;
}

VkOffset2D 
Vk_RenderApiUtil::toVkOffset2D(const Rect2f& rect2)
{
	VkOffset2D vkOffset = { sCast<int>(rect2.x), sCast<int>(rect2.y) };
	return vkOffset;
}

VkExtent2D 
Vk_RenderApiUtil::toVkExtent2D(const Rect2f& rect2)
{
	VkExtent2D vkExtent = 
	{
		sCast<u32>(rect2.w),
		sCast<u32>(rect2.h)
	};
	return vkExtent;
}

VkExtent2D 
Vk_RenderApiUtil::toVkExtent2D(const Vec2f& vec2)
{
	VkExtent2D vkExtent = 
	{
		sCast<u32>(vec2.x),
		sCast<u32>(vec2.y)
	};
	return vkExtent;
}

Rect2f
Vk_RenderApiUtil::toRect2f(const VkExtent2D& ext2d)
{
	return Rect2f{ 0, 0, sCast<float>(ext2d.width), sCast<float>(ext2d.height) };
}

VkFormat
Vk_RenderApiUtil::toVkFormat(RenderDataType v)
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
		//case SRC::Float24Norm8:	{ return VK_FORMAT_D24_SFLOAT_S8_UINT;	} break;
		case SRC::Float32Norm8:	{ return VK_FORMAT_D32_SFLOAT_S8_UINT;	} break;

		default: { throw RDS_ERROR("unsupported RenderDataType"); } break;
	}
}

VkFormat
Vk_RenderApiUtil::toVkFormat(ColorType v)
{
	using SRC = ColorType;
	switch (v) 
	{
		case SRC::Rb:		{ return VK_FORMAT_R8_UNORM;			} break;

		case SRC::RGBAb:	{ return VK_FORMAT_R8G8B8A8_UNORM;		} break;
		case SRC::RGBAs:	{ return VK_FORMAT_R16G16B16A16_UNORM;	} break;
		case SRC::RGBAh:	{ return VK_FORMAT_R16G16B16A16_SFLOAT;	} break;
		case SRC::RGBAf:	{ return VK_FORMAT_R32G32B32A32_SFLOAT;	} break;

		case SRC::Depth:	{ return VK_FORMAT_D32_SFLOAT_S8_UINT;	} break;

		default: { RDS_THROW("unsupport type {}", v); }	break;
	}
	//return VkFormat::VK_FORMAT_MAX_ENUM;
}

VkFormat	
Vk_RenderApiUtil::toVkFormat_ShaderTexture(VkFormat v)
{
	using SRC = VkFormat;
	switch (v) 
	{
		case SRC::VK_FORMAT_R8_UNORM				:
		case SRC::VK_FORMAT_R8_UINT					: { return VK_FORMAT_R8_UNORM;	} break;

		case SRC::VK_FORMAT_R8G8B8A8_UNORM			:
		case SRC::VK_FORMAT_R8G8B8A8_USCALED		:
		case SRC::VK_FORMAT_R8G8B8A8_UINT			:
		case SRC::VK_FORMAT_R8G8B8A8_SRGB			: { return VK_FORMAT_R8G8B8A8_UNORM;	} break;

		case SRC::VK_FORMAT_R16G16B16A16_UNORM		:
		case SRC::VK_FORMAT_R16G16B16A16_USCALED	:
		case SRC::VK_FORMAT_R16G16B16A16_UINT		:
		case SRC::VK_FORMAT_R16G16B16A16_SFLOAT		: { return VK_FORMAT_R16G16B16A16_UNORM;	} break;

			//case SRC::VK_FORMAT_R32G32B32A32_UINT		:
			//case SRC::VK_FORMAT_R32G32B32A32_SINT		:
			//case SRC::VK_FORMAT_R32G32B32A32_SFLOAT		: { return VK_FORMAT_R32G32B32A32_SFLOAT;	} break;

		default: { RDS_THROW("unsupport type"); }	break;
	}
}

VkFormat	
Vk_RenderApiUtil::toVkFormat_srgb	(VkFormat v)
{
	using SRC = VkFormat;
	switch (v) 
	{
		case SRC::VK_FORMAT_R8_UNORM				:
		case SRC::VK_FORMAT_R8_UINT					: { return VK_FORMAT_R8_SRGB;	} break;

		case SRC::VK_FORMAT_R8G8B8A8_UNORM			:
		case SRC::VK_FORMAT_R8G8B8A8_SNORM			:
		case SRC::VK_FORMAT_R8G8B8A8_USCALED		:
		case SRC::VK_FORMAT_R8G8B8A8_SSCALED		:
		case SRC::VK_FORMAT_R8G8B8A8_UINT			:
		case SRC::VK_FORMAT_R8G8B8A8_SINT			:
		case SRC::VK_FORMAT_R8G8B8A8_SRGB			: { return VK_FORMAT_R8G8B8A8_SRGB;	} break;

		default: { RDS_THROW("unsupport type {}", RDS_SRCLOC); }	break;
	}
}

VkBufferUsageFlagBits 
Vk_RenderApiUtil::toVkBufferUsage(RenderGpuBufferTypeFlags v)
{
	using SRC = RenderGpuBufferTypeFlags;
	switch (v)
	{
		case SRC::Vertex:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; }	break;
		case SRC::Index:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT; }		break;
		case SRC::Const:		{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; }	break;
		case SRC::TransferSrc:	{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT; }		break;
		case SRC::TransferDst:	{ return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT; }		break;
		default: { RDS_THROW("unsupport type {}, {}", v, RDS_SRCLOC); }	break;
	}
	//return VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
}

VkMemoryPropertyFlags 
Vk_RenderApiUtil::toVkMemoryPropFlags(RenderMemoryUsage memUsage)
{
	VkMemoryPropertyFlags flags = {};
	//if (BitUtil::has(requiredProperties, sCast<VkFlags>(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))) { return RenderMemoryUsage::CpuToGpu;	}
	//if (BitUtil::has(requiredProperties, sCast<VkFlags>(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))) { return RenderMemoryUsage::GpuOnly;	}
	RDS_NOT_YET_SUPPORT();
	return flags;
}

VkIndexType 
Vk_RenderApiUtil::toVkIndexType(RenderDataType idxType)
{
	using SRC = RenderDataType;
	switch (idxType)
	{
		case SRC::UInt16: { return VK_INDEX_TYPE_UINT16; } break;
		case SRC::UInt32: { return VK_INDEX_TYPE_UINT32; } break;
		default: { return VK_INDEX_TYPE_MAX_ENUM; }
	}
}

bool 
Vk_RenderApiUtil::isDepthFormat(VkFormat format)
{
	return isDepthOnlyFormat(format) || hasStencilComponent(format);
}

bool 
Vk_RenderApiUtil::isDepthOnlyFormat(VkFormat format)
{
	return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT;
}

bool 
Vk_RenderApiUtil::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT;
}

bool 
Vk_RenderApiUtil::isVkFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features, RenderDevice_Vk* rdDevVk)
{
	auto* vkPhyDev = rdDevVk->vkPhysicalDevice();

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(vkPhyDev, format, &props);

	bool isLinear	= tiling == VK_IMAGE_TILING_LINEAR	&& BitUtil::has(props.linearTilingFeatures,  features);
	bool isOptimal	= tiling == VK_IMAGE_TILING_OPTIMAL	&& BitUtil::has(props.optimalTilingFeatures, features);

	return isLinear || isOptimal;
}

VkAttachmentLoadOp	
Vk_RenderApiUtil::toVkAttachmentLoadOp (RenderTargetLoadOp v)
{
	using SRC = RenderTargetLoadOp;
	switch (v)
	{
		case SRC::Load:		{ return VK_ATTACHMENT_LOAD_OP_LOAD; }		break;
		case SRC::Clear:	{ return VK_ATTACHMENT_LOAD_OP_CLEAR; }		break;
		case SRC::DontCare: { return VK_ATTACHMENT_LOAD_OP_DONT_CARE; } break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
}

VkAttachmentStoreOp	
Vk_RenderApiUtil::toVkAttachmentStoreOp(RenderTargetStoreOp v)
{
	using SRC = RenderTargetStoreOp;
	switch (v)
	{
		case SRC::None:		{ return VK_ATTACHMENT_STORE_OP_NONE; }		break;
		case SRC::Store:	{ return VK_ATTACHMENT_STORE_OP_STORE; }		break;
		case SRC::DontCare: { return VK_ATTACHMENT_STORE_OP_DONT_CARE; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
}

#if 0
VkAttachmentStoreOp	
Vk_RenderApiUtil::toVkAttachmentStoreOp(RenderTargetOp v)
{
	using SRC = RenderTargetOp;
	switch (v)
	{
		/*case SRC::Store:	{ return VK_ATTACHMENT_STORE_OP_STORE; }		break;
		case SRC::DontCare: { return VK_ATTACHMENT_STORE_OP_DONT_CARE; }	break;*/
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
}

VkAttachmentStoreOp	
Vk_RenderApiUtil::toVkAttachmentStoreOp(RenderTargetOp v)
{
	using SRC = RenderTargetOp;
	switch (v)
	{
		/*case SRC::Store:	{ return VK_ATTACHMENT_STORE_OP_STORE; }		break;
		case SRC::DontCare: { return VK_ATTACHMENT_STORE_OP_DONT_CARE; }	break;*/
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
}
#endif // 0


VkClearValue 
Vk_RenderApiUtil::toVkClearValue(const Color4f& color)
{
	VkClearValue o;
	auto& f32s = o.color.float32;
	f32s[0] = color.r;
	f32s[1] = color.g;
	f32s[2] = color.b;
	f32s[3] = color.a;
	return o;
}

VkClearValue 
Vk_RenderApiUtil::toVkClearValue(float depth, u32 stencil)
{
	VkClearValue o;
	o.depthStencil.depth = depth;
	o.depthStencil.stencil = stencil;
	return o;
}

VkShaderStageFlagBits	
Vk_RenderApiUtil::toVkShaderStageBit(ShaderStageFlag v)
{
	using SRC = ShaderStageFlag;

	switch (v)
	{
		case SRC::Vertex:	{ return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT; }	break;
		case SRC::Pixel:	{ return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT; } break;
		case SRC::Compute:	{ return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}

	//return VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}


VkShaderStageFlagBits	
Vk_RenderApiUtil::toVkShaderStageBits(ShaderStageFlag flag)
{
	using SRC = ShaderStageFlag;

	EnumInt<VkShaderStageFlagBits> o = 0;

	if (BitUtil::has(flag, SRC::Vertex))	{ o |= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT; }
	if (BitUtil::has(flag, SRC::Pixel))		{ o |= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT; }
	if (BitUtil::has(flag, SRC::Compute))	{ o |= VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT; }

	return sCast<VkShaderStageFlagBits>(o);
}


StrView 
Vk_RenderApiUtil::toShaderStageProfile(ShaderStageFlag v)
{
	using SRC = rds::ShaderStageFlag;
	switch (v)
	{
		case SRC::Vertex:		{ return "vs_1.1"; } break;
		case SRC::Pixel:		{ return "ps_1.1"; } break;
		case SRC::Compute:		{ return "cs_1.1"; } break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
}


VkDescriptorType			
Vk_RenderApiUtil::toVkDescriptorType(ShaderResourceType v)
{
	using SRC = rds::ShaderResourceType;
	switch (v)
	{
		case SRC::ConstantBuffer:	{ return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; } break;
		case SRC::Texture:			{ return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; }	break;
		case SRC::Sampler:			{ return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER; }		break;
		case SRC::StorageBuffer:	{ return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; } break;
		case SRC::StorageImage:		{ return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkFilter
Vk_RenderApiUtil::toVkFilter(SamplerFilter v)
{
	using SRC = rds::SamplerFilter;
	switch (v)
	{
		case SRC::Nearest:		{ return VkFilter::VK_FILTER_NEAREST; }		break;
		case SRC::Linear:		{ return VkFilter::VK_FILTER_LINEAR; }		break;
		case SRC::Bilinear:		{ return VkFilter::VK_FILTER_LINEAR; }		break;
		case SRC::Trilinear:	{ return VkFilter::VK_FILTER_CUBIC_IMG; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkFilter::VK_FILTER_MAX_ENUM;
}

VkSamplerAddressMode 
Vk_RenderApiUtil::toVkSamplerAddressMode(SamplerWrap v)
{
	using SRC = rds::SamplerWrap;
	switch (v)
	{
		case SRC::Repeat:			{ return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT; }			break;
		case SRC::Mirrored:			{ return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; }	break;
		case SRC::ClampToEdge:		{ return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; }		break;
		case SRC::ClampToBorder:	{ return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

VkImageViewType		
Vk_RenderApiUtil::toVkImageViewType(RenderDataType v)
{
	using SRC = rds::RenderDataType;
	switch (v)
	{
		case SRC::Texture1D:			{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D; }			break;
		case SRC::Texture2D:			{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D; }			break;
		case SRC::Texture3D:			{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D; }			break;
		case SRC::TextureCube:			{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE; }		break;
		case SRC::Texture1DArray:		{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY; }	break;
		case SRC::Texture2DArray:		{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY; }	break;
		case SRC::TextureCubeArray:		{ return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY; }	break;
		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkImageViewType::VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

Vk_StageAccess
Vk_RenderApiUtil::toVkStageAccess(VkImageLayout srcLayout, VkImageLayout dstLayout)
{
	Vk_StageAccess out;
	auto& srcStage	= out.srcStage;
	auto& dstStage	= out.dstStage;
	auto& srcAccess = out.srcAccess;
	auto& dstAccess	= out.dstAccess;

	if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED && dstLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		srcStage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;		// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		dstStage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
		srcAccess	= VK_PIPELINE_STAGE_NONE_KHR;
		dstAccess	= VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		srcStage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		srcAccess	= VK_ACCESS_TRANSFER_WRITE_BIT;
		dstAccess	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED && dstLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)						// write renderTarget
	{
		srcStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		srcAccess	= VK_PIPELINE_STAGE_NONE_KHR;
		dstAccess	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED && dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)				// write depthStencil
	{
		srcStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dstStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		srcAccess	= VK_PIPELINE_STAGE_NONE_KHR;
		dstAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // write depthStencil
	{
		srcStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dstStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		srcAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dstAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) // read depthStencil in shader
	{
		srcStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dstStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;		// pass a texture usage here could help
		srcAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dstAccess	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) // read depthStencil in shader
	{
		srcStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dstStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;		// pass a texture usage here could help
		srcAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dstAccess	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) // read depthStencil in attachment
	{
		srcStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dstStage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		srcAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dstAccess	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) // read render target
	{
		srcStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		srcAccess	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dstAccess	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) // shader rsc -> rt
	{
		srcStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dstStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		srcAccess	= VK_ACCESS_NONE;
		dstAccess	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (srcLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && dstLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) // present
	{
		srcStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstStage	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		srcAccess	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dstAccess	= VK_PIPELINE_STAGE_NONE_KHR;
	}
	else 
	{
		throwError("");
	}

	return out;
}

VkCullModeFlagBits 
Vk_RenderApiUtil::toVkCullMode(RenderState_Cull v)
{
	using SRC = RenderState_Cull;
	switch (v)
	{
		case SRC::None:		{ return VkCullModeFlagBits::VK_CULL_MODE_NONE; }		break;
		case SRC::Back:		{ return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT; }	break;
		case SRC::Front:	{ return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT; }	break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkCullModeFlagBits::VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

VkPrimitiveTopology	
Vk_RenderApiUtil::toVkPrimitiveTopology(RenderPrimitiveType v)
{
	using SRC = RenderPrimitiveType;
	switch (v)
	{
		case SRC::Triangle: { return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; } break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkCompareOp			
Vk_RenderApiUtil::toVkCompareOp(RenderState_DepthTestOp v)
{
	using SRC = RenderState_DepthTestOp;
	switch (v)
	{
		case SRC::Never:		{ return VkCompareOp::VK_COMPARE_OP_NEVER; }			break;
		case SRC::Always:		{ return VkCompareOp::VK_COMPARE_OP_ALWAYS; }			break;
		case SRC::Less:			{ return VkCompareOp::VK_COMPARE_OP_LESS; }				break;
		case SRC::LessEqual:	{ return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL; }	break;
		case SRC::Greater:		{ return VkCompareOp::VK_COMPARE_OP_GREATER; }			break;
		case SRC::GreaterEqual: { return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL; } break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkCompareOp::VK_COMPARE_OP_MAX_ENUM;
}

VkBlendFactor	
Vk_RenderApiUtil::toVkBlendFactor(RenderState_BlendFactor v)
{
	using SRC = RenderState_BlendFactor;
	switch (v)
	{
		case SRC::Zero:					{ return VkBlendFactor::VK_BLEND_FACTOR_ZERO; }						break;
		case SRC::One:					{ return VkBlendFactor::VK_BLEND_FACTOR_ONE; }						break;
		case SRC::SrcColor:				{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }		break;
		case SRC::DstColor:				{ return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR; }				break;
		case SRC::SrcAlpha:				{ return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA; }				break;
		case SRC::ConstColor:			{ return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR; }			break;
		case SRC::ConstAlpha:			{ return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA; }			break;
		case SRC::OneMinusSrcColor:		{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }		break;
		case SRC::OneMinusDstColor:		{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }		break;
		case SRC::OneMinusSrcAlpha:		{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }		break;
		case SRC::OneMinusDstAlpha:		{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }		break;
		case SRC::OneMinusConstColor:	{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; } break;
		case SRC::OneMinusConstAlpha:	{ return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; } break;
		case SRC::SrcAlphaSaturate:		{ return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }		break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkBlendFactor::VK_BLEND_FACTOR_MAX_ENUM;
}

VkBlendOp		
Vk_RenderApiUtil::toVkBlendOp(RenderState_BlendOp v)
{
	using SRC = RenderState_BlendOp;
	switch (v)
	{
		case SRC::Disable:	{ return VkBlendOp::VK_BLEND_OP_ADD; }				break;
		case SRC::Add:		{ return VkBlendOp::VK_BLEND_OP_ADD; }				break;
		case SRC::Sub:		{ return VkBlendOp::VK_BLEND_OP_SUBTRACT; }			break;
		case SRC::RevSub:	{ return VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT; }	break;
		case SRC::Min:		{ return VkBlendOp::VK_BLEND_OP_MIN; }				break;
		case SRC::Max:		{ return VkBlendOp::VK_BLEND_OP_MAX; }				break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkBlendOp::VK_BLEND_OP_MAX_ENUM;
}

VkImageLayout		
Vk_RenderApiUtil::toVkImageLayout(TextureFlags v)
{
	using SRC = TextureFlags;
	switch (v)
	{
		case SRC::None:				{ return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; }						break;
		case SRC::ShaderResource:	{ return VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }			break;
		case SRC::UnorderedAccess:	{ return VkImageLayout::VK_IMAGE_LAYOUT_GENERAL; }							break;
		case SRC::RenderTarget:		{ return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; }			break;
		case SRC::DepthStencil:		{ return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; }	break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkBlendOp::VK_BLEND_OP_MAX_ENUM;
}

VkImageLayout		
Vk_RenderApiUtil::toVkImageLayout(TextureFlags v, RenderAccess access)
{
	using SRC = TextureFlags;
	switch (v)
	{
		case SRC::None:				{ RDS_CORE_ASSERT(access == RenderAccess::None); return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; }		break;
		case SRC::ShaderResource:	{ return VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }										break;
		case SRC::UnorderedAccess:	{ return VkImageLayout::VK_IMAGE_LAYOUT_GENERAL; }														break;
		case SRC::RenderTarget:		{ return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;}										break;
		case SRC::DepthStencil:		{ return access == RenderAccess::Write ? VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	: VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; }	break;

		default: { RDS_THROW("unsupport type {}", v); } break;
	}
	//return VkBlendOp::VK_BLEND_OP_MAX_ENUM;
}

u32
Vk_RenderApiUtil::getMemoryTypeIdx(u32 memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties, RenderDevice_Vk* rdDevVk)
{
	auto& vkMemProps = rdDevVk->memoryContext()->vkMemoryProperties();
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
Vk_RenderApiUtil::setDebugUtilObjectName(Vk_Device_T* vkDevHnd, VkObjectType vkObjT, const String& name, const void* vkHnd)
{
	RDS_CORE_ASSERT(vkHnd, "");

	VkDebugUtilsObjectNameInfoEXT nameInfo = {};
	nameInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType		= vkObjT;
	nameInfo.pObjectName	= name.c_str();
	nameInfo.objectHandle	= reinCast<u64>(vkHnd);
	nameInfo.pNext			= VK_NULL_HANDLE;

	auto ret = vkSetDebugUtilsObjectName(vkDevHnd, &nameInfo);
	throwIfError(ret);
}

Vk_Buffer*		Vk_RenderApiUtil::toVkBuf		(		RenderGpuBuffer* rdGpuBuf) { return sCast<RenderGpuBuffer_Vk*>(rdGpuBuf)->vkBuf(); }
Vk_Buffer*		Vk_RenderApiUtil::toVkBuf		(const	RenderGpuBuffer* rdGpuBuf) { return constCast(sCast<const RenderGpuBuffer_Vk*>(rdGpuBuf))->vkBuf(); }
Vk_Buffer_T*	Vk_RenderApiUtil::toVkBufHnd	(		RenderGpuBuffer* rdGpuBuf) { return toVkBuf(rdGpuBuf)->hnd(); }
Vk_Buffer_T*	Vk_RenderApiUtil::toVkBufHnd	(const	RenderGpuBuffer* rdGpuBuf) { return toVkBuf(rdGpuBuf)->hnd(); }

void 
Vk_RenderApiUtil::createDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& out)
{
	out = {};

	out.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	out.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	out.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	out.pfnUserCallback = debugCallback;
	out.pUserData		= nullptr; // Optional
}

void 
Vk_RenderApiUtil::createSwapchain(Vk_Swapchain_T** out, Vk_Surface_T* vkSurface, Vk_Device_T* vkDevice, 
								  const Vk_SwapchainInfo& info, const Vk_SwapchainAvailableInfo& avaInfo, RenderDevice_Vk* rdDevVk)
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
	createInfo.imageExtent		= toVkExtent2D(info.rect2f);
	createInfo.imageArrayLayers = 1;		// For non-stereoscopic-3D applications, this value is 1.
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const auto& queueFamilyIndices = rdDevVk->queueFamilyIndices();

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

	auto ret = vkCreateSwapchainKHR(vkDevice, &createInfo, rdDevVk->allocCallbacks(), out);
	throwIfError(ret);
}

//void 
//Vk_RenderApiUtil::createBuffer(Vk_Buffer_T** outBuf, Vk_DeviceMemory** outBufMem, VkDeviceSize size
//								, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, QueueTypeFlags queueTypeFlags)
//{
//	auto* vkDev				= Renderer_Vk::instance()->vkDevice();
//	auto* vkAllocCallbacks	= Renderer_Vk::instance()->allocCallbacks();
//	auto& vkQueueIndices	= Renderer_Vk::instance()->queueFamilyIndices();
//
//	VkBufferCreateInfo bufferInfo = {};
//	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	bufferInfo.size						= size;
//	bufferInfo.usage					= usage;
//	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
//
//	Vector<u32, QueueFamilyIndices::s_kQueueTypeCount> queueIdices;
//	auto queueCount = vkQueueIndices.get(queueIdices, queueTypeFlags);
//	if (queueCount > 1)
//	{
//		bufferInfo.sharingMode				= VK_SHARING_MODE_CONCURRENT;
//		bufferInfo.queueFamilyIndexCount	= queueCount;
//		bufferInfo.pQueueFamilyIndices		= queueIdices.data();
//	}
//
//	auto ret = vkCreateBuffer(vkDev, &bufferInfo, vkAllocCallbacks, outBuf);
//	throwIfError(ret);
//
//	VkMemoryRequirements memRequirements;
//	vkGetBufferMemoryRequirements(vkDev, *outBuf, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo = {};
//	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize	= memRequirements.size;
//	allocInfo.memoryTypeIndex	= getMemoryTypeIdx(memRequirements.memoryTypeBits, properties);
//
//	ret = vkAllocateMemory(vkDev, &allocInfo, vkAllocCallbacks, outBufMem);
//
//	VkDeviceSize offset = 0;
//	RDS_CORE_ASSERT(offset % memRequirements.alignment == 0, "not aligned");
//	vkBindBufferMemory(vkDev, *outBuf, *outBufMem, offset);
//}

void 
Vk_RenderApiUtil::copyBuffer(Vk_Buffer* dstBuffer, Vk_Buffer* srcBuffer, VkDeviceSize size, Vk_CommandPool_T* vkCmdPool, Vk_Queue* vkTransferQueue, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev	= rdDevVk->vkDevice();
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
		cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
		cmdBufSubmitInfo.commandBuffer	= vkCmdBuf;

		submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
		submitInfo.commandBufferInfoCount	= 1;
		submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

		RDS_LOG_WARN("TODO: add smp for transfer and graphics queue");
		ret = vkQueueSubmit2(vkTransferQueue->hnd(), 1, &submitInfo, VK_NULL_HANDLE);
		throwIfError(ret);
		vkQueueWaitIdle(vkTransferQueue->hnd());
	}
}

void 
Vk_RenderApiUtil::transitionImageLayout(Vk_Image* image, VkFormat vkFormat, VkImageLayout dstLayout, VkImageLayout srcLayout, Vk_Queue* dstQueue, Vk_Queue* srcQueue, Vk_CommandBuffer* vkCmdBuf)
{
	vkCmdBuf->beginRecord(srcQueue);

	VkPipelineStageFlags	srcStage		= {};
	VkPipelineStageFlags	dstStage		= {};
	VkAccessFlags			srcAccessMask	= {};
	VkAccessFlags			dstAccessMask	= {};

	if (dstLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_UNDEFINED ) 
	{
		srcStage		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;		// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		dstStage		= VK_PIPELINE_STAGE_TRANSFER_BIT;
		srcAccessMask	= 0;
		dstAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		srcStage		= VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage		= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
		dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;
	}
	else if (dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && srcLayout == VK_IMAGE_LAYOUT_UNDEFINED) 
	{
		srcStage		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;			// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		dstStage		= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		srcAccessMask	= 0;
		dstAccessMask	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else 
	{
		throwError("");
	}

	VkImageAspectFlags aspectMask = dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	if (hasStencilComponent(vkFormat)) 
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

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
Vk_RenderApiUtil::copyBufferToImage(Vk_Image* dstImage, Vk_Buffer* srcBuf, u32 width, u32 height, Vk_Queue* vkQueue, Vk_CommandBuffer* vkCmdBuf)
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
Vk_RenderApiUtil::createBuffer(Vk_Buffer& outBuf, RenderDevice_Vk* rdDevVk, Vk_Allocator* vkAlloc, Vk_AllocInfo* allocInfo, VkDeviceSize size, VkBufferUsageFlags usage, QueueTypeFlags queueTypeFlags)
{
	outBuf.create(rdDevVk, vkAlloc, allocInfo, size, usage, queueTypeFlags);
}

void
Vk_RenderApiUtil::getPhyDevicePropertiesTo(RenderAdapterInfo& outInfo, Vk_PhysicalDevice_T* phyDevice)
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
Vk_RenderApiUtil::getPhyDeviceFeaturesTo(RenderAdapterInfo& outInfo, Vk_PhysicalDevice_T* phyDevice)
{
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

	RenderAdapterInfo::Feature temp;
	temp.shaderHasFloat64		= deviceFeatures.shaderFloat64;
	temp.hasGeometryShader		= deviceFeatures.geometryShader;
	temp.hasSamplerAnisotropy	= deviceFeatures.samplerAnisotropy;

	outInfo.feature = temp;
}

void Vk_RenderApiUtil::getVkPhyDeviceFeaturesTo(VkPhysicalDeviceFeatures& out, const RenderAdapterInfo& info)
{
	out = {};

	out.samplerAnisotropy = VK_TRUE;

	RDS_CORE_LOG_WARN("TODO: getVkPhyDeviceFeaturesTo()");
	//info.feature.
}

bool
Vk_RenderApiUtil::getSwapchainAvailableInfoTo(Vk_SwapchainAvailableInfo& out, Vk_PhysicalDevice_T* vkPhyDevice, Vk_Surface_T* vkSurface)
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
#pragma mark --- rdsVk_ExtensionInfo-Impl ---
#endif // 0
#if 1

u32
Vk_ExtensionInfo::getAvailableValidationLayersTo(Vector<VkLayerProperties, s_kLocalSize>& out, bool logAvaliable)
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
Vk_ExtensionInfo::getAvailableInstanceExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, bool logAvaliable)
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
Vk_ExtensionInfo::getAvailablePhyDeviceExtensionsTo(Vector<VkExtensionProperties, s_kLocalSize>& out, Vk_PhysicalDevice_T* phyDevice, bool logAvaliable)
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
Vk_ExtensionInfo::create(RenderDevice_Vk* rdDevVk)
{
	_rdDevVk = rdDevVk;
}

void
Vk_ExtensionInfo::createInstanceExtensions(const RenderAdapterInfo& adapterInfo, bool logAvaliableExtension)
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
Vk_ExtensionInfo::createValidationLayers(const RenderAdapterInfo& adapterInfo)
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
Vk_ExtensionInfo::createPhyDeviceExtensions(const RenderAdapterInfo& adapterInfo, const RenderDevice_CreateDesc& rdDevCDesc, Vk_PhysicalDevice_T* phyDevice)
{
	getAvailablePhyDeviceExtensionsTo(_availablePhyDeviceExts, phyDevice, false);
	auto& o = _phyDeviceExts;
	o.clear();

	auto emplaceIfExist = [adapterInfo](auto& dst, const char* ext, const auto& availPhyDevExts)
	{
		if (availPhyDevExts.findIf([ext](const auto& v) { return StrUtil::isSame(v.extensionName, ext); }) != availPhyDevExts.end())
		{
			dst.emplace_back(ext);
		}
		else
		{
			RDS_CORE_LOG("{} do not support vulkan extension: {}", adapterInfo.adapterName, ext);
		}
	};

	// TODO: check extension exist
	//o.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME); // deprecated, but only in 1.3?
	emplaceIfExist(o, VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,		availablePhyDeviceExts());
	emplaceIfExist(o, VK_GOOGLE_HLSL_FUNCTIONALITY_1_EXTENSION_NAME,	availablePhyDeviceExts());
	emplaceIfExist(o, VK_KHR_MAINTENANCE1_EXTENSION_NAME,				availablePhyDeviceExts());
	
	if (rdDevCDesc.isPresent)
	{
		o.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
}

bool 
Vk_ExtensionInfo::isSupportValidationLayer(const char* validationLayerName) const
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
Vk_ExtensionInfo::checkValidationLayersExist()
{
	for (const auto& layerName : validationLayers())
	{
		throwIf(!isSupportValidationLayer(layerName), "not supported validation layer {}", layerName);
	}
}


#endif

}

#endif // RDS_RENDER_HAS_VULKAN