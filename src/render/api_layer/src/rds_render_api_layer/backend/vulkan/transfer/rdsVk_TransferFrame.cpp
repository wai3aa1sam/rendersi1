#include "rds_render_api_layer-pch.h"
#include "rdsVk_TransferFrame.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

Vk_TransferFrame::Vk_TransferFrame()
{

}

Vk_TransferFrame::~Vk_TransferFrame()
{
	RDS_CORE_ASSERT(!_transferVkCmdPool, "have not call destroy()");
}

void 
Vk_TransferFrame::create(TransferContext_Vk* ctx)
{
	RDS_CORE_ASSERT(ctx, "");

	auto& queueFamily = ctx->renderer()->queueFamilyIndices();
	u32 transferQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Transfer);
	u32 graphicsQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Graphics);
	//u32 computeQueueFamilyIdx	= queueFamily.getFamilyIdx(QueueTypeFlags::Compute);

	_inFlightVkFnc.create();
	_completedVkSmp.create();
	_transferVkCmdPool.create(transferQueueFamilyIdx);

	RDS_TODO("remove? maybe put to main render graphics queue");
	_graphicsInFlightVkFnc.create();
	_graphicsCompletedVkSmp.create();
	_graphicsVkCmdPool.create(graphicsQueueFamilyIdx);

	// thread-safe
	_stagingAlloc.create(ctx->device());
}

void 
Vk_TransferFrame::destroy(TransferContext_Vk* ctx)
{
	clear();

	_graphicsVkCmdPool.destroy();
	_transferVkCmdPool.destroy();

	_stagingAlloc.destroy();
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
Vk_TransferFrame::requestStagingBuffer(u32& outIdx, VkDeviceSize size)
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
	staging->create(&_stagingAlloc, &allocInfo, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Transfer);

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


}


#endif