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
	RDS_CORE_ASSERT(!_vkCommandPool, "have not call destroy()");
}

void 
Vk_TransferFrame::create(TransferContext_Vk* ctx)
{
	RDS_CORE_ASSERT(ctx, "");

	_completedVkSmp.create();
	_inFlightVkFence.create();

	auto& queueFamily = ctx->renderer()->queueFamilyIndices();
	_vkCommandPool.create(queueFamily.getFamilyIdx(QueueTypeFlags::Transfer));

	// thread-safe
	_stagingAlloc.create(ctx->device());
}

void 
Vk_TransferFrame::destroy(TransferContext_Vk* ctx)
{
	_stagingBufs.clear();
	_vkCommandPool.destroy();
	_stagingAlloc.destroy();
}

void 
Vk_TransferFrame::clear()
{
	_stagingBufs.clear();
}

Vk_Buffer* 
Vk_TransferFrame::requestStagingBuffer(u32& outIdx, VkDeviceSize size)
{
	RDS_CORE_ASSERT(size > 0, "request size == 0");

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::CpuToGpu;

	outIdx = sCast<u32>(_stagingBufs.size());
	auto& staging = _stagingBufs.emplace_back();
	staging.create(&_stagingAlloc, &allocInfo, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Transfer | QueueTypeFlags::Graphics);

	return &_stagingBufs.back();
}

Vk_Buffer_T* 
Vk_TransferFrame::getVkStagingBufHnd(u32 idx)
{
	RDS_CORE_ASSERT(idx < _stagingBufs.size(), "invalid vk staging buffer index");
	return _stagingBufs[idx].hnd();
}


}


#endif