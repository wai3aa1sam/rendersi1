#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#if RDS_RENDER_HAS_VULKAN

#if 0
#pragma mark --- rdsVulkan_Include-Decl ---
#endif // 0
#if 1

#if RDS_OS_WINDOWS
	#define VK_PROTOTYPES
	#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif // RDS_OS_WINDOWS

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#if RDS_OS_WINDOWS
	#include <vulkan/vulkan_win32.h>
#else

#endif // RDS_OS_WINDOWS

#endif


RDS_DISABLE_ALL_WARNINGS();

	#if RDS_DEVELOPMENT

		#define VMA_DEBUG_MARGIN 16
		#define VMA_DEBUG_DETECT_CORRUPTION 1
		#define RDS_VMA_ENABLE_DEBUG_LOG 0

		// enable debug log, but need modify src code, maybe pull request
		#if RDS_VMA_ENABLE_DEBUG_LOG
		#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
			printf((format), __VA_ARGS__); \
			printf("\n"); \
			} while(false)
		#endif // 0

	#endif // RDS_DEBUG

	#include <vk_mem_alloc.h>

RDS_RESTORE_ALL_WARNINGS();

#if VK_VERSION_1_3
	#define RDS_VK_VER_1_3 1
#elif VK_VERSION_1_2
	#define RDS_VK_VER_1_2 1
#endif // 1

#if !RDS_VK_VER_1_3
	#error "please download Vulkan 1.3 sdk from https://www.lunarg.com/vulkan-sdk/"
	#error "delete CMakeCache.txt and also make sure the environment variable [VULKAN_SDK] is set to version 1.3"
	#error "or pass a full vulkan sdk path to customSdkRoot in cmake function Vulkan_findPackage()"
#endif

#if 0
#pragma mark --- rdsVkInstFn-Decl ---
#endif // 0
#if 1

#if RDS_VK_VER_1_2
inline PFN_vkQueueSubmit2KHR vkQueueSubmit2;
#endif

#endif // 1

#if 0
#pragma mark --- rdsVkInstFn-Decl ---
#endif // 0
#if 1

inline PFN_vkSetDebugUtilsObjectNameEXT		vkSetDebugUtilsObjectName;
inline PFN_vkSetDebugUtilsObjectTagEXT		vkSetDebugUtilsObjectTag;
//inline PFN_vkDebugMarkerSetObjectTagEXT		vkDebugMarkerSetObjectTag;		// deprecated
inline PFN_vkCmdBeginDebugUtilsLabelEXT		vkCmdBeginDebugUtilsLabel;
inline PFN_vkCmdEndDebugUtilsLabelEXT		vkCmdEndDebugUtilsLabel;
inline PFN_vkCmdInsertDebugUtilsLabelEXT	vkCmdInsertDebugUtilsLabel;

inline PFN_vkQueueBeginDebugUtilsLabelEXT	vkQueueBeginDebugUtilsLabel;
inline PFN_vkQueueEndDebugUtilsLabelEXT		vkQueueEndDebugUtilsLabel;
inline PFN_vkQueueInsertDebugUtilsLabelEXT	vkQueueInsertDebugUtilsLabel;


#endif // 1

#if 0
#pragma mark --- rdsVkSync2Fn-Decl ---
#endif // 0
#if 1

inline PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2_khr;

#endif

namespace rds
{
using Vk_AllocHnd	= VmaAllocation;
using Vk_Allocation = VmaAllocation;

}

#if 0
#pragma mark --- rdsVk_DebugMarco-Decl ---
#endif // 0
#if 1

#define RDS_VK_MAX_RENDER_TARGET_COUNT 8

#endif // 1


#endif