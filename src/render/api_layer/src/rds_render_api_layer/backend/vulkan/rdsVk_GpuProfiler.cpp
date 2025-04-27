#include "rds_render_api_layer-pch.h"
#include "rdsVk_GpuProfiler.h"

#include "rdsRenderContext_Vk.h"
#include "rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_GpuProfiler-Impl ---
#endif // 0
#if 1

Vk_GpuProfiler::Vk_GpuProfiler()
{

}

Vk_GpuProfiler::~Vk_GpuProfiler()
{
	destroy();
}


void Vk_GpuProfiler::create(const CreateDesc& cDesc) 
{ 
	onCreate(cDesc); 
}

void Vk_GpuProfiler::destroy()
{
	onDestroy(); 
}

void 
Vk_GpuProfiler::onCreate(const CreateDesc& cDesc)
{
	_rdCtx		= cDesc.rdCtx;

	auto* rdCtxVk = renderContexVk();

	_createProfilerContext(_gfxCtx,		cDesc, QueueTypeFlags::Graphics,	rdCtxVk->vkGraphicsQueue());
	_createProfilerContext(_computeCtx,	cDesc, QueueTypeFlags::Compute,		rdCtxVk->vkComputeQueue());
}

void 
Vk_GpuProfiler::onDestroy()
{
	_destroyProfilerContext(_gfxCtx);
	_destroyProfilerContext(_computeCtx);
	_rdCtx = nullptr;
}

void			
Vk_GpuProfiler::commit()
{
	_commit(_gfxCtx,		QueueTypeFlags::Graphics);
	//_commit(_computeCtx,	QueueTypeFlags::Compute);
}

void 
Vk_GpuProfiler::setName(const char* name)
{
	_setName(_gfxCtx,		name);
	_setName(_computeCtx,	name);
}

#if RDS_ENABLE_GPU_PROFILER


VkProfileScope 
Vk_GpuProfiler::makeProfileScope(const SrcLocData* srcLocData, Vk_CommandBuffer* vkCmdBuf)
{
	if (!srcLocData)
		return tracy::VkCtxScope(_gfxCtx,		srcLocData, vkCmdBuf->hnd(), false);

	RDS_CORE_ASSERT(srcLocData, "srcLocData == nullptr");
	RDS_CORE_ASSERT(vkCmdBuf, "nullptr");
	auto queueTypeFlags = vkCmdBuf->queueTypeFlags();
	throwIf(BitUtil::has(queueTypeFlags, QueueTypeFlags::Transfer), "queue must support gfx or compute");
	//constexpr int callStackDepth = 1;

	switch (queueTypeFlags)
	{
		case QueueTypeFlags::Graphics:	{ return tracy::VkCtxScope(_gfxCtx,		srcLocData, vkCmdBuf->hnd(), true); } break;
		case QueueTypeFlags::Compute:	{ return tracy::VkCtxScope(_computeCtx, srcLocData, vkCmdBuf->hnd(), true); } break;
		default: { RDS_THROW(""); } break;
	}
}

VkProfileScope	
Vk_GpuProfiler::makeProfileScope(const SrcLoc& srcLoc, Vk_CommandBuffer* vkCmdBuf, const char* name)
{
	RDS_CORE_ASSERT(vkCmdBuf, "nullptr");
	auto queueTypeFlags = vkCmdBuf->queueTypeFlags();
	throwIf(BitUtil::has(queueTypeFlags, QueueTypeFlags::Transfer), "queue must support gfx or compute");
	//constexpr int callStackDepth = 1;

	switch (queueTypeFlags)
	{
		case QueueTypeFlags::Graphics:	{ return tracy::VkCtxScope(_gfxCtx,		srcLoc.line, srcLoc.file, ::strlen(srcLoc.file), srcLoc.func, strlen(srcLoc.func), name, strlen(name), vkCmdBuf->hnd(), true); } break;
		// case QueueTypeFlags::Graphics:	{ return tracy::VkCtxScope(_gfxCtx,		srcLoc.line, srcLoc.file, ::strlen(srcLoc.file), srcLoc.func, strlen(srcLoc.func), name, strlen(name), vkCmdBuf->hnd(), true); } break;
		// case QueueTypeFlags::Compute:	{ return tracy::VkCtxScope(_computeCtx, srcLoc.line, srcLoc.file, ::strlen(srcLoc.file), srcLoc.func, strlen(srcLoc.func), name, strlen(name), vkCmdBuf->hnd(), true); } break;
		default: { RDS_THROW("invalid QueueTypeFlags"); } break;
	}
}

void 
Vk_GpuProfiler::_createProfilerContext(Vk_GpuProfilerContext& ctx, const CreateDesc& cDesc, QueueTypeFlags queueType, Vk_Queue* vkQueue)
{
	auto* rdDevVk = renderDeviceVk();

	auto* vkPhyDev	= rdDevVk->vkPhysicalDevice();																				RDS_UNUSED(vkPhyDev);
	auto* vkDev		= rdDevVk->vkDevice();																						RDS_UNUSED(vkDev);

	auto vkFuncExtCtd	= (rdDevVk->extInfo().getInstanceExtFunction<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(	"vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
	auto vkFuncExtCt	= (rdDevVk->extInfo().getInstanceExtFunction<PFN_vkGetCalibratedTimestampsEXT>(						"vkGetCalibratedTimestampsEXT"));

	Vk_CommandPool pool;
	pool.create(vkQueue->familyIdx(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, rdDevVk);
	auto* vkCmdBuf = pool.requestCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, "createProfilerCtx", rdDevVk);

	RDS_ASSERT(vkFuncExtCtd && vkFuncExtCt || !vkFuncExtCtd && !vkFuncExtCt, "must be both exist or both not exist");
	if (!vkFuncExtCtd && !vkFuncExtCt)
	{
		ctx = TracyVkContext(vkPhyDev, vkDev, vkQueue->hnd(), vkCmdBuf->hnd());
	}
	else
	{
		ctx = TracyVkContextCalibrated(vkPhyDev, vkDev, vkQueue->hnd(), vkCmdBuf->hnd(), vkFuncExtCtd, vkFuncExtCt);
	}

	_setName(ctx, fmtAs_T<TempString>("{}-{}-{}", cDesc.name, queueType, vkQueue->familyIdx()).c_str());
}

void 
Vk_GpuProfiler::_destroyProfilerContext(Vk_GpuProfilerContext& ctx)
{
	if (ctx)
	{
		TracyVkDestroy(ctx);
		ctx = nullptr;
	}
}

void 
Vk_GpuProfiler::_commit(Vk_GpuProfilerContext& ctx, QueueTypeFlags queuType)
{
	auto* rdCtxVk	= renderContexVk();
	auto* vkCmdBuf	= rdCtxVk->requestCommandBuffer(queuType, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, RDS_FUNC_NAME_SZ);
	vkCmdBuf->beginRecord();
	TracyVkCollect(ctx, vkCmdBuf->hnd());
	vkCmdBuf->endRecord();
}

void 
Vk_GpuProfiler::_setName(Vk_GpuProfilerContext& ctx, const char* name)
{
	TracyVkContextName(ctx,	name, sCast<u16>(::strlen(name) + 1));
}

#else


VkProfileScope	
Vk_GpuProfiler::makeProfileScope(const SrcLoc& srcLoc, Vk_CommandBuffer* vkCmdBuf, const char* name)
{
	return {};
}

VkProfileScope 
Vk_GpuProfiler::makeProfileScope(const tracy::SourceLocationData* srcLoc, Vk_CommandBuffer* vkCmdBuf, const char* name)
{
	return {};
}

void 
Vk_GpuProfiler::_createProfilerContext(Vk_GpuProfilerContext& ctx, const CreateDesc& cDesc, QueueTypeFlags queueType, Vk_Queue* vkQueue)
{
	
}

void 
Vk_GpuProfiler::_destroyProfilerContext(Vk_GpuProfilerContext& ctx)
{
	
}

void 
Vk_GpuProfiler::_commit(Vk_GpuProfilerContext& ctx, QueueTypeFlags queuType)
{
	
}

void 
Vk_GpuProfiler::_setName(Vk_GpuProfilerContext& ctx, const char* name)
{
	
}

#endif // RDS_ENABLE_GPU_PROFILER

RenderContext_Vk*	
Vk_GpuProfiler::renderContexVk()
{
	return sCast<RenderContext_Vk*>(_rdCtx);
}

RenderDevice_Vk* 
Vk_GpuProfiler::renderDeviceVk()
{
	return renderContexVk()->renderDeviceVk();
}

#endif

}

#endif