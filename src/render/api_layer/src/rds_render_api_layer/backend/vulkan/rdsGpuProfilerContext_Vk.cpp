#include "rds_render_api_layer-pch.h"
#include "rdsGpuProfilerContext_Vk.h"

#include "rdsRenderContext_Vk.h"
#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsGpuProfilerContext_Vk-Impl ---
#endif // 0
#if 1

void 
GpuProfilerContext_Vk::onCreate(const CreateDesc& cDesc)
{
	auto* rdCtxVk = sCast<RenderContext_Vk*>(cDesc.rdCtx);
	auto* rdr = rdCtxVk->renderer();
	auto vkFuncExtCtd	= (rdr->extInfo().getInstanceExtFunction<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>("vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
	auto vkFuncExtCt	= (rdr->extInfo().getInstanceExtFunction<PFN_vkGetCalibratedTimestampsEXT>					("vkGetCalibratedTimestampsEXT"));

	auto* vkPhyDev			= rdr->vkPhysicalDevice();		RDS_UNUSED(vkPhyDev			);
	auto* vkDev				= rdr->vkDevice();				RDS_UNUSED(vkDev			);
	auto* vkGraphicsQueue	= rdCtxVk->vkGraphicsQueue();	RDS_UNUSED(vkGraphicsQueue	);
	auto* vkCmdBuf			= rdCtxVk->vkCommandBuffer();	RDS_UNUSED(vkCmdBuf			);

	RDS_ASSERT(vkFuncExtCtd && vkFuncExtCt || !vkFuncExtCtd && !vkFuncExtCt, "must be both exist or both not exist");
	if (!vkFuncExtCtd && !vkFuncExtCt)
	{
		_ctx = RDS_PROFILE_GPU_CREATE_CTX_VK_IMPL(vkPhyDev, vkDev, vkGraphicsQueue->hnd(), vkCmdBuf);
	}
	else
	{
		_ctx = RDS_PROFILE_GPU_CREATE_CTX_CALIBRATED_VK_IMPL(vkPhyDev, vkDev, vkGraphicsQueue->hnd(), vkCmdBuf, vkFuncExtCtd, vkFuncExtCt);
	}

	setName(cDesc.name);
}

void 
GpuProfilerContext_Vk::onDestroy()
{
	if (_ctx)
	{
		RDS_PROFILE_GPU_DESTROY_CTX_VK_IMPL(_ctx);
		_ctx = nullptr;
	}
}

void 
GpuProfilerContext_Vk::zone(Vk_CommandBuffer_T* vkCmdBuf, const char* name)
{
	RDS_PROFILE_GPU_ZONET_VK_IMPL(_ctx, vkCmdBuf, name);
}

void 
GpuProfilerContext_Vk::collect(Vk_CommandBuffer_T* vkCmdBuf)
{
	RDS_PROFILE_GPU_COLLECT_VK_IMPL(_ctx, vkCmdBuf);
}

void 
GpuProfilerContext_Vk::setName(const char* name)
{
	RDS_PROFILE_GPU_CTX_NAME_VK_IMPL(_ctx, name);
}

#endif

}

#endif