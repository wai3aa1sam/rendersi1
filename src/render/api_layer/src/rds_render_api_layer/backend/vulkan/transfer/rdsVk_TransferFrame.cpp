#include "rds_render_api_layer-pch.h"
#include "rdsVk_TransferFrame.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

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

	auto& queueFamily = rdDevVk->queueFamilyIndices();
	u32 transferQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Transfer);
	u32 graphicsQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Graphics);
	//u32 computeQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Compute);

	 _inFlightVkFnc.create(rdDevVk);
	_completedVkSmp.create(rdDevVk);
	_transferVkCmdPool.create(transferQueueFamilyIdx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, rdDevVk);

	RDS_TODO("remove? maybe put to main render graphics queue");
	 _graphicsInFlightVkFnc.create(rdDevVk);
	_graphicsCompletedVkSmp.create(rdDevVk);
	_graphicsVkCmdPool.create(graphicsQueueFamilyIdx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, rdDevVk);

	// thread-safe
	_stagingAlloc.create(rdDevVk);

	_setDebugName();
}

void 
Vk_TransferFrame::destroy()
{
	clear();

	auto* rdDevVk = renderDeviceVk();

	_graphicsVkCmdPool.destroy(rdDevVk);
	_transferVkCmdPool.destroy(rdDevVk);

	_stagingAlloc.destroy();

	 _inFlightVkFnc.destroy(rdDevVk);
	_completedVkSmp.destroy(rdDevVk);

	 _graphicsInFlightVkFnc.destroy(rdDevVk);
	_graphicsCompletedVkSmp.destroy(rdDevVk);
}

void 
Vk_TransferFrame::clear()
{
	{
		auto lockedData = _stagingBufs.scopedULock();
		lockedData->clear();
	}

	_hasTransferedGraphicsResoures	= false;
	_hasTransferedComputeResoures	= false;
}

Vk_Buffer* 
Vk_TransferFrame::requestStagingBuffer(u32& outIdx, VkDeviceSize size, RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(size > 0, "request size must not be 0");
	RDS_TODO("MutexProtected with a ptr element should be faster, but need a allocator (linear is ok), otherwise it need allocate everytime");

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::CpuToGpu;

	Vk_Buffer* staging = nullptr;
	{
		auto lockedData = _stagingBufs.scopedULock();
		auto& stagingBufs = *lockedData;

		outIdx = sCast<u32>(stagingBufs.size());
		staging = stagingBufs.emplace_back(makeUPtr<Vk_Buffer>()).ptr();
	}
	staging->create(rdDevVk, &_stagingAlloc, &allocInfo, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Transfer);

	return staging;
}

Vk_Buffer_T* 
Vk_TransferFrame::getVkStagingBufHnd(u32 idx)
{
	auto lockedData = _stagingBufs.scopedULock();
	auto& stagingBufs = *lockedData;

	RDS_CORE_ASSERT(idx < stagingBufs.size(), "invalid vk staging buffer index");
	return stagingBufs[idx]->hnd();
}

void 
Vk_TransferFrame::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME(_inFlightVkFnc);
	RDS_VK_SET_DEBUG_NAME(_completedVkSmp);
	RDS_VK_SET_DEBUG_NAME(_transferVkCmdPool);

	RDS_VK_SET_DEBUG_NAME(_graphicsInFlightVkFnc);
	RDS_VK_SET_DEBUG_NAME(_graphicsCompletedVkSmp);
	RDS_VK_SET_DEBUG_NAME(_graphicsVkCmdPool);
}

RenderDevice_Vk* Vk_TransferFrame::renderDeviceVk() { return _tsfCtxVk->renderDeviceVk(); }

}


#endif