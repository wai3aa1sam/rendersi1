#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN && RDS_ENABLE_PROFILER

#include <tracy/TracyVulkan.hpp>

// ---
#define RDS_PROFILE_GPU_CREATE_CTX_VK_IMPL(PHY_DEV, DEV, QUEUE, CMDBUF)											TracyVkContext(PHY_DEV, DEV, QUEUE, CMDBUF)
#define RDS_PROFILE_GPU_CREATE_CTX_CALIBRATED_VK_IMPL(PHY_DEV, DEV, QUEUE, CMDBUF, FUNC_EXT_CTD, FUNC_EXT_CT)	TracyVkContextCalibrated(PHY_DEV, DEV, QUEUE, CMDBUF, FUNC_EXT_CTD, FUNC_EXT_CT)
#define RDS_PROFILE_GPU_DESTROY_CTX_VK_IMPL(T_GP_CTX)															TracyVkDestroy(T_GP_CTX)

#define RDS_PROFILE_GPU_CTX_NAME_VK_IMPL(T_GP_CTX, NAME)	TracyVkContextName(T_GP_CTX, NAME, sCast<u16>(StrUtil::len(NAME) + 1))

#define RDS_PROFILE_GPU_ZONET_VK_IMPL(T_GP_CTX, CMDBUF, NAME)	TracyVkZoneTransient(T_GP_CTX, RDS_CONCAT(_internal_tvkz_, RDS_LINE), CMDBUF, NAME, true)
#define RDS_PROFILE_GPU_COLLECT_VK_IMPL(T_GP_CTX, CMDBUF)	TracyVkCollect(T_GP_CTX, CMDBUF)

// ---
#define RDS_PROFILE_GPU_CTX_VK(VAR_NAME)			::rds::GpuProfilerContext_Vk VAR_NAME
#define RDS_PROFILE_GPU_CTX_CREATE(GP_CTX, NAME)	GP_CTX.create({NAME, this})
#define RDS_PROFILE_GPU_CTX_DESTROY(GP_CTX)			GP_CTX.destroy()

#define RDS_PROFILE_GPU_CTX_SET_NAME(NAME)	VAR_NAME.setName(NAME)

#define  RDS_PROFILE_GPU_ZONE_VK(GP_CTX, CMDBUF, NAME)			TracyVkZone((GP_CTX).ctx(), CMDBUF, NAME)
#define RDS_PROFILE_GPU_ZONEC_VK(GP_CTX, CMDBUF, NAME, COLOR)	TracyVkZoneC((GP_CTX).ctx(), CMDBUF, NAME, COLOR)
#define RDS_PROFILE_GPU_ZONET_VK(GP_CTX, CMDBUF, NAME)			(GP_CTX).zone(CMDBUF, NAME)
#define RDS_PROFILE_GPU_COLLECT_VK(GP_CTX, CMDBUF)				(GP_CTX).collect(CMDBUF)

// useless, use if in base class instead
// #define GPU_PROFILE_ZONE(NAME)
// if (vulkan) { TracyVkZoneC(gpCtx->ctx(), vkCmdBuf, NAME, COLOR) }
#define M_NamedZone(PG_CTX, CMDBUF, NAME, COLOR) \
	static constexpr const char* RDS_CONCAT(s_name, RDS_LINE) = NAME; \
	struct RDS_CONCAT(GpuProfilerContextZoneNamed, RDS_LINE) \
	{ \
		void set(rds::GpuProfilerContext_Vk* gpCtx, rds::Vk_CommandBuffer* vkCmdBuf) \
		{ \
			TracyVkZoneC(gpCtx->ctx(), vkCmdBuf, RDS_CONCAT(s_name, RDS_LINE), COLOR); \
		} \
	}; \
	RDS_SRCLOC; \
	RDS_CONCAT(GpuProfilerContextZoneNamed, RDS_LINE) RDS_CONCAT(_SetName_Var, RDS_LINE); RDS_CONCAT(_SetName_Var, RDS_LINE).set((PG_CTX), CMDBUF)

#else

#endif

#if RDS_RENDER_HAS_VULKAN

#include "rdsRenderApi_Include_Vk.h"
#include "rdsRenderApiPrimitive_Vk.h"

#if 0
#pragma mark --- rdsGpuProfilerContext_Vk-Impl ---
#endif // 0
#if 1

namespace rds
{

// TODO: change GpuProfilerContext_Vk to virtual 
// RenderContext hold a GpuProfilerContextCtx*, RenderContext hold a GpuProfilerContext_Vk / GpuProfilerContextCtx_Dx12
// onCreate in Vk / Dx12 set GpuProfilerContextCtx = &GpuProfilerContext_Vk;
// RDS_GPU_PROFILE_ZONE(...) GpuProfilerContextCtx->zone(...);

class RenderContext;

struct GpuProfilerContext_CreateDesc
{
	GpuProfilerContext_CreateDesc() = default;
	GpuProfilerContext_CreateDesc(const char* name_, RenderContext* rdCtx_)
	{
		name  = name_;
		rdCtx = rdCtx_;
	}
	const char*		name;
	RenderContext*	rdCtx = nullptr;
};

class GpuProfilerContext_Vk
{
public:
	using CreateDesc  = GpuProfilerContext_CreateDesc;
	using ProfilerCtx = TracyVkCtx;

public:
	GpuProfilerContext_Vk() = default;
	/*GpuProfilerContext_Vk(Vk_PhysicalDevice* vkPhyDev, Vk_Device* vkDev, Vk_Queue* vkQueue, Vk_CommandBuffer* vkCmdBuf
		, PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkFuncExtCtd = nullptr, PFN_vkGetCalibratedTimestampsEXT vkFuncExtCt = nullptr);*/
	virtual ~GpuProfilerContext_Vk();

	void create(const CreateDesc& cDesc) { onCreate(cDesc); }
	void destroy() { onDestroy(); }

	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void zone(Vk_CommandBuffer* vkCmdBuf, const char* name);
	virtual void collect(Vk_CommandBuffer* vkCmdBuf);

	virtual void setName(const char* name);

	ProfilerCtx& ctx();

private:
	ProfilerCtx _ctx		= nullptr;
	RenderContext* _rdCtx	= nullptr;
};

inline
GpuProfilerContext_Vk::~GpuProfilerContext_Vk()
{
	destroy();
}

inline GpuProfilerContext_Vk::ProfilerCtx& GpuProfilerContext_Vk::ctx() { return _ctx; }

}


#endif

#endif