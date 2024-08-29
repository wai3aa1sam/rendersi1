#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderApi_Include_Vk.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApiPrimitive.h"

#if RDS_RENDER_HAS_VULKAN && RDS_ENABLE_PROFILER

#include <tracy/TracyVulkan.hpp>

using Vk_GpuProfilerContext		= TracyVkCtx;
using VkProfileScope			= tracy::VkCtxScope;

// ---
// TracyVkZoneTransientS
//#define RDS_PROFILE_GPU_ZONET_VK_IMPL(CTX, CMDBUF, NAME)				TracyVkZoneTransientS(T_GP_CTX, RDS_CONCAT(_internal_tvkz_, RDS_LINE), CMDBUF, NAME, RDS_PROFILE_CALLSTACK_DEPTH, true)
//#define RDS_VK_PROFILE_GPU_ZONE(CTX, CMDBUF, NAME)					tracy::VkCtxScope ( CTX, RDS_LINE, RDS_FILE, ::strlen( RDS_FILE ), RDS_FUNC_NAME_SZ, strlen( RDS_FUNC_NAME_SZ ), NAME, strlen( NAME ), CMDBUF, RDS_PROFILE_CALLSTACK_DEPTH, true );
#define RDS_VK_PROFILE_GPU_TRANSIENT_ZONE(PROFILER, CMDBUF, NAME)		auto RDS_UNIQUE_VAR_NAME(VkProfileScope) = PROFILER.makeProfileScope(RDS_SRCLOC, CMDBUF, NAME)
#define RDS_VK_PROFILE_GPU_TRANSIENT_FMT(PROFILER, CMDBUF, FMT, ...)	RDS_VK_PROFILE_GPU_TRANSIENT_ZONE(PROFILER, CMDBUF, fmtAs_T<TempString>(FMT, __VA_ARGS__).c_str())

#else


using Vk_GpuProfilerContext		= int*;
using VkProfileScope			= int;

#define RDS_VK_PROFILE_GPU_TRANSIENT_ZONE(CTX, CMDBUF, NAME)				


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