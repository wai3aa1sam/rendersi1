#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderApi_Include_Vk.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApiPrimitive.h"

#define RDS_ENABLE_GPU_PROFILER 1
#if RDS_RENDER_HAS_VULKAN && RDS_ENABLE_PROFILER && RDS_ENABLE_GPU_PROFILER

#include <tracy/TracyVulkan.hpp>

using Vk_GpuProfilerContext		= TracyVkCtx;
using VkProfileScope			= tracy::VkCtxScope;

// ---
// TracyVkZoneTransientS
//#define RDS_PROFILE_GPU_ZONET_VK_IMPL(CTX, CMDBUF, NAME)				TracyVkZoneTransientS(T_GP_CTX, RDS_CONCAT(_internal_tvkz_, RDS_LINE), CMDBUF, NAME, RDS_PROFILE_CALLSTACK_DEPTH, true)
//#define RDS_VK_PROFILE_GPU_ZONE(CTX, CMDBUF, NAME)					tracy::VkCtxScope ( CTX, RDS_LINE, RDS_FILE, ::strlen( RDS_FILE ), RDS_FUNC_NAME_SZ, strlen( RDS_FUNC_NAME_SZ ), NAME, strlen( NAME ), CMDBUF, RDS_PROFILE_CALLSTACK_DEPTH, true );

#define RDS_VK_PROFILE_GPU_DYNAMIC_ZONE(PROFILER, CMDBUF, NAME)		auto RDS_VAR_NAME(_scope) = PROFILER.makeProfileScope(RDS_SRCLOC, CMDBUF, NAME)
//#define RDS_VK_PROFILE_GPU_DYNAMIC_FMT(PROFILER, CMDBUF, FMT, ...)	RDS_FMT_STATIC_VAR(tstr, TempString, FMT, __VA_ARGS__); RDS_FMT_TO(RDS_VAR_NAME(tstr), FMT, __VA_ARGS__); RDS_VK_PROFILE_GPU_DYNAMIC_ZONE(PROFILER, CMDBUF, RDS_VAR_NAME(cstr))

#if RDS_PROFILER_ENABLE_DYNAMIC
	#define RDS_VK_PROFILE_GPU_DYNAMIC_FMT(PROFILER, CMDBUF, FMT, ...)	RDS_FMT_VAR(tstr, TempString, FMT, __VA_ARGS__); RDS_VK_PROFILE_GPU_DYNAMIC_ZONE(PROFILER, CMDBUF, RDS_VAR_NAME(tstr).c_str())
#else
	#define RDS_VK_PROFILE_GPU_DYNAMIC_FMT(PROFILER, CMDBUF, FMT, ...)	// RDS_FMT_VAR(tstr, TempString, FMT, __VA_ARGS__); static constexpr tracy::SourceLocationData RDS_VAR_NAME(__tracy_gpu_source_location) { FMT, TracyFunction,  TracyFile, (uint32_t)TracyLine, 0 }; auto RDS_VAR_NAME(_scope) = PROFILER.makeProfileScope(&RDS_VAR_NAME(__tracy_gpu_source_location), CMDBUF, FMT)
#endif // RDS_PROFILER_ENABLE_DYNAMIC

#else


using Vk_GpuProfilerContext		= int*;
using VkProfileScope			= int;

#define RDS_VK_PROFILE_GPU_DYNAMIC_ZONE(CTX, CMDBUF, NAME)				
#define RDS_VK_PROFILE_GPU_DYNAMIC_FMT(PROFILER, CMDBUF, FMT, ...)

#endif

#if RDS_RENDER_HAS_VULKAN


#if 0
#pragma mark --- rdsVk_GpuProfiler-Impl ---
#endif // 0
#if 1

namespace rds
{

class RenderContext;
class RenderContext_Vk;
class RenderDevice_Vk;

struct Vk_GpuProfiler_CreateDesc
{
	Vk_GpuProfiler_CreateDesc() = default;
	Vk_GpuProfiler_CreateDesc(const char* name_, RenderContext* rdCtx_)
	{
		name  = name_;
		rdCtx = rdCtx_;
	}
	const char*		name		= nullptr;
	RenderContext*	rdCtx		= nullptr;
	//Vk_Queue*		vkQueue		= nullptr;
	//QueueTypeFlags	queueType	= QueueTypeFlags::Graphics;
};

class Vk_GpuProfiler
{
public:
	using CreateDesc	= Vk_GpuProfiler_CreateDesc;

public:
	Vk_GpuProfiler();
	~Vk_GpuProfiler();

	void create(const CreateDesc& cDesc);
	void destroy();

	RDS_NODISCARD VkProfileScope makeProfileScope(const SrcLoc& srcLoc, Vk_CommandBuffer* vkCmdBuf, const char* name);
	RDS_NODISCARD VkProfileScope makeProfileScope(const tracy::SourceLocationData* srcLoc, Vk_CommandBuffer* vkCmdBuf, const char* name);
	
	void commit();

	void setName(const char* name);

public:
	RenderContext_Vk*	renderContexVk();
	RenderDevice_Vk*	renderDeviceVk();

protected:
	void onCreate(const CreateDesc& cDesc);
	void onDestroy();

private:
	void _createProfilerContext(Vk_GpuProfilerContext& ctx, const CreateDesc& cDesc, QueueTypeFlags queueType, Vk_Queue* vkQueue);
	void _destroyProfilerContext(Vk_GpuProfilerContext& ctx);
	void _commit(Vk_GpuProfilerContext& ctx, QueueTypeFlags queuType);

	void _setName(Vk_GpuProfilerContext& ctx, const char* name);

private:
	RenderContext*			_rdCtx		= nullptr;
	Vk_GpuProfilerContext	_gfxCtx		= nullptr;
	Vk_GpuProfilerContext	_computeCtx	= nullptr;
};

}


#endif

#endif