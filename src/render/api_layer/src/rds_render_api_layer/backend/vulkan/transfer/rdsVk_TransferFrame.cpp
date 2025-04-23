#include "rds_render_api_layer-pch.h"
#include "rdsVk_TransferFrame.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/rdsRenderer.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_QueueData-Impl ---
#endif // 0
#if 1

void 
Vk_QueueData::create(QueueTypeFlags qeueuType, RenderDevice_Vk* rdDevVk, VkCommandPoolCreateFlags vkCmdPool)
{
	auto& queueFamily = rdDevVk->queueFamilyIndices();
	u32 transferQueueFamilyIdx	= queueFamily.getFamilyIdx(qeueuType);

	inFlightVkFence.create(rdDevVk);
	completedVkSemaphore.create(rdDevVk);
	vkCommandPool.create(transferQueueFamilyIdx, vkCmdPool, rdDevVk);
}

void 
Vk_QueueData::destroy()
{
	auto* rdDevVk = renderDeviceVk();
	inFlightVkFence.destroy(rdDevVk);
	completedVkSemaphore.destroy(rdDevVk);
	vkCommandPool.destroy(rdDevVk);
}

void 
Vk_QueueData::setDebugName(const SrcLoc& srcLoc, StrView name)
{
	//auto* rdDevVk = renderDeviceVk();
	RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC(inFlightVkFence,		/*rdDevVk,*/ srcLoc, "{}::{}", name, "inFlightVkFence");
	RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC(completedVkSemaphore,	/*rdDevVk,*/ srcLoc, "{}::{}", name, "completedVkSemaphore");
	RDS_VK_SET_DEBUG_NAME_FMT_SRCLOC(vkCommandPool,			/*rdDevVk,*/ srcLoc, "{}::{}", name, "vkCommandPool");
}

RenderDevice_Vk* Vk_QueueData::renderDeviceVk() { return vkCommandPool.renderDeviceVk(); }

#endif

#if 0
#pragma mark --- rdsVk_TransferFrame-Impl ---
#endif // 0
#if 1

Vk_TransferFrame::Vk_TransferFrame()
{

}

Vk_TransferFrame::~Vk_TransferFrame()
{
	//RDS_CORE_ASSERT(!_transferVkCmdPool, "have not call destroy()");
	destroy();
}

void 
Vk_TransferFrame::create(TransferContext_Vk* tsfCtxVk)
{
	RDS_CORE_ASSERT(tsfCtxVk, "");

	_tsfCtxVk = tsfCtxVk;
	auto* rdDevVk = tsfCtxVk->renderDeviceVk();

	// auto& queueFamily = rdDevVk->queueFamilyIndices();
	// u32 transferQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Transfer);
	// u32 graphicsQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Graphics);
	//u32 computeQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Compute);

	_tsfVkQueueData.create(QueueTypeFlags::Transfer, rdDevVk);
	_gfxVkQueueData.create(QueueTypeFlags::Graphics, rdDevVk);

	// thread-safe
	//_stagingAlloc.create(rdDevVk);
	//_linearStagingBuf.create(rdDevVk);

	_setDebugName();
}

void 
Vk_TransferFrame::destroy()
{
	clear();

	//auto* rdDevVk = renderDeviceVk();
	_gfxVkQueueData.destroy();
	_tsfVkQueueData.destroy();
}

void 
Vk_TransferFrame::clear()
{
	//_linearStagingBuf.reset();
	//_hasTransferedGraphicsResoures	= false;
	//_hasTransferedComputeResoures	= false;
}

void 
Vk_TransferFrame::waitAndResetQueueData(QueueTypeFlags type)
{
	waitQueueData(type);
	resetQueueData(type);
}

void 
Vk_TransferFrame::resetQueueData(QueueTypeFlags type)
{
	auto* rdDevVk = renderDeviceVk();

	//if (hasTransferedResoures(type))
	{
		auto& queData = getVkQueueData(type);
		queData.vkCommandPool.reset(rdDevVk);
		queData.inFlightVkFence.reset(rdDevVk);
	}
}

void 
Vk_TransferFrame::waitQueueData(QueueTypeFlags type)
{
	auto* rdDevVk = renderDeviceVk();
	// if (auto* v = getInFlightVkFence(type)) 
	// 	v->wait(rdDevVk);
	auto& queData = getVkQueueData(type);
	queData.inFlightVkFence.wait(rdDevVk);
}

void 
Vk_TransferFrame::_setDebugName()
{
	_tsfVkQueueData.setDebugName(RDS_SRCLOC, ":_tsfVkQueueData");
	_gfxVkQueueData.setDebugName(RDS_SRCLOC, ":_gfxVkQueueData");
}

RenderDevice_Vk* Vk_TransferFrame::renderDeviceVk() { return _tsfCtxVk->renderDeviceVk(); }

Vk_QueueData& 
Vk_TransferFrame::getVkQueueData(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return _gfxVkQueueData; } break;
		//case SRC::Compute:	{ return _gfxVkQueueData; } break;
		case SRC::Transfer: { return _tsfVkQueueData; } break;
		default: { RDS_THROW(""); }
	}
}

Vk_Fence* 
Vk_TransferFrame::getInFlightVkFence(QueueTypeFlags type)
{
	return hasTransferedResoures(type) ? &getVkQueueData(type).inFlightVkFence : nullptr;
}

Vk_Semaphore* 
Vk_TransferFrame::getCompletedVkSemaphore(QueueTypeFlags type)
{
	return hasTransferedResoures(type) ? &getVkQueueData(type).completedVkSemaphore : nullptr;
}

Vk_CommandBuffer* 
Vk_TransferFrame::requestCommandBuffer(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return requestGraphicsCommandBuffer(level, debugName); } break;
			//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		case SRC::Transfer: { return requestTransferCommandBuffer(level, debugName); } break;
		default: { RDS_THROW(""); }
	}
}

void 
Vk_TransferFrame::setTransferedResoures(QueueTypeFlags type, bool val)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { _hasTransferedGraphicsResoures	= val; } break;
		case SRC::Compute:	{ _hasTransferedComputeResoures		= val; } break;
		case SRC::Transfer: { ; } break;
		default: { RDS_THROW(""); }
	}
}

bool 
Vk_TransferFrame::hasTransferedResoures(QueueTypeFlags type) const
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return hasTransferedGraphicsResoures(); } break;
		//case SRC::Compute:	{ return hasTransferedComputeResoures()	; } break;
		case SRC::Transfer: { return true							; } break;
		default: { RDS_THROW(""); }
	}
}

#endif


}


#endif