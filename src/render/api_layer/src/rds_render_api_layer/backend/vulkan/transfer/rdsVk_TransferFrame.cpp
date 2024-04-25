#include "rds_render_api_layer-pch.h"
#include "rdsVk_TransferFrame.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

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
	//_stagingAlloc.create(rdDevVk);
	_linearStagingBuf.create(rdDevVk);

	_setDebugName();
}

void 
Vk_TransferFrame::destroy()
{
	clear();

	auto* rdDevVk = renderDeviceVk();

	_graphicsVkCmdPool.destroy(rdDevVk);
	_transferVkCmdPool.destroy(rdDevVk);

	_inFlightVkFnc.destroy(rdDevVk);
	_completedVkSmp.destroy(rdDevVk);

	_graphicsInFlightVkFnc.destroy(rdDevVk);
	_graphicsCompletedVkSmp.destroy(rdDevVk);
}

void 
Vk_TransferFrame::clear()
{
	_linearStagingBuf.reset();

	_hasTransferedGraphicsResoures	= false;
	_hasTransferedComputeResoures	= false;

	auto* rdDevVk = renderDeviceVk();
	_graphicsVkCmdPool.reset(rdDevVk);
	_transferVkCmdPool.reset(rdDevVk);
}

void
Vk_TransferFrame::requestStagingHandle(StagingHandle& out, VkDeviceSize size)
{
	out = _linearStagingBuf.alloc(size);
}

void 
Vk_TransferFrame::uploadToStagingBuf(StagingHandle& out, ByteSpan data, SizeType offset)
{
	VkDeviceSize bytes = sCast<VkDeviceSize>(data.size());
	requestStagingHandle(out, bytes);
	auto* dst = _linearStagingBuf.mappedData<u8*>(out);
	memory_copy(dst, data.data(), bytes);
}

void*	
Vk_TransferFrame::mappedStagingBufData(StagingHandle hnd)
{
	auto* dst = _linearStagingBuf.mappedData<u8*>(hnd);
	return dst;
}

Vk_Buffer_T* 
Vk_TransferFrame::getVkStagingBufHnd(StagingHandle hnd)
{
	return _linearStagingBuf.vkStagingBufHnd(hnd);
}

void 
Vk_TransferFrame::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_inFlightVkFnc);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_completedVkSmp);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_transferVkCmdPool);

	RDS_VK_SET_DEBUG_NAME_SRCLOC(_graphicsInFlightVkFnc);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_graphicsCompletedVkSmp);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_graphicsVkCmdPool);
}

RenderDevice_Vk* Vk_TransferFrame::renderDeviceVk() { return _tsfCtxVk->renderDeviceVk(); }

#endif

#if 0
#pragma mark --- rdsVk_LinearStagingBuffer-Impl ---
#endif // 0
#if 1

Vk_LinearStagingBuffer::Chunks::Chunks()
{
	_chunkSize	= 128 * 1024 * 1024;
}

Vk_LinearStagingBuffer::Chunks::~Chunks()
{

}


#endif

}


#endif