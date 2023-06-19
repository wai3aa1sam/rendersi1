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

#endif