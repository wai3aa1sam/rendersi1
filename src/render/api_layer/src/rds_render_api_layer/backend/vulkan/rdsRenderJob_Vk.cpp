#include "rds_render_api_layer-pch.h"
#include "rdsRenderJob_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderFrame.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderJob_Vk-Impl ---
#endif // 0
#if 1

RenderJob_Vk::RenderJob_Vk()
{
	
}

RenderJob_Vk::~RenderJob_Vk()
{
	destroy();
}

void 
RenderJob_Vk::reset(RenderDevice_Vk* rdDevVk)
{
	//_vk_rdFrame.inFlightFence()->wait(rdCtxVk->renderDeviceVk());

	_vk_rdFrame.reset(rdDevVk);
	_pendingGfxVkCmdbufHnds.clear();
}

bool 
RenderJob_Vk::onCheckUploadCompleted()
{
	auto& vkTsfFrame	= vkTransferFrame();
	auto& vkQueueData	= vkTsfFrame.getVkQueueData(QueueTypeFlags::Transfer);
	return vkQueueData.inFlightVkFence.isSignaled(renderDeviceVk());
}

bool 
RenderJob_Vk::onCheckRenderCompleted()
{
	return _vk_rdFrame.inFlightFence()->isSignaled(renderDeviceVk());
}

void 
RenderJob_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	_vk_rdFrame.create(sCast<RenderDevice_Vk*>(cDesc.renderDevice));
}

void 
RenderJob_Vk::onDestroy()
{
	Base::onDestroy();
}

void
RenderJob_Vk::onReset(RenderContext* rdCtx)
{
	Base::onReset(rdCtx);
	//_vkLinearStagingBuf.reset();
}

//void 
//RenderJob_Vk::onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd)
//{
//
//}

Vk_CommandBuffer* 
RenderJob_Vk::requestCommandBuffer(Vk_Queue& vk_queue, VkCommandBufferLevel bufLevel, StrView debugName)
{
	using SRC = rds::QueueTypeFlags;
	auto&	vkRdFrame	= vkRenderFrame();
	
	auto* o = vkRdFrame.requestCommandBuffer(vk_queue.queueType(), bufLevel, debugName);
	if (BitUtil::has(vk_queue.queueType(), SRC::Graphics))
	{
		_pendingGfxVkCmdbufHnds.emplace_back(o->hnd());
	}
	o->reset(&vk_queue);
	return o;
}

#endif


}


#endif