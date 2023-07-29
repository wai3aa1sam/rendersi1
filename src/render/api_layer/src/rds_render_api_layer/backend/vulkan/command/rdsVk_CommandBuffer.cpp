#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandBuffer.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_CommandBuffer-Impl ---
#endif // 0
#if 1


Vk_CommandBuffer::Vk_CommandBuffer()
{

}

Vk_CommandBuffer::~Vk_CommandBuffer()
{

}

void Vk_CommandBuffer::create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level)
{
	_vkCommandPool = vkCommandPool;
	_level = level;

	auto* vkDevice = Renderer_Vk::instance()->vkDevice();

	VkCommandBufferAllocateInfo cInfo = {};
	cInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cInfo.commandPool			= _vkCommandPool->hnd();
	cInfo.level					= level;
	cInfo.commandBufferCount	= 1;

	auto ret = vkAllocateCommandBuffers(vkDevice, &cInfo, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::destroy()
{
	auto* vkDev = Renderer_Vk::instance()->vkDevice();
	Vk_CommandBuffer_T* vkCmdBufs[] = { hnd()};
	vkFreeCommandBuffers(vkDev, _vkCommandPool->hnd(), ArraySize<decltype(vkCmdBufs)>, vkCmdBufs);
}

void 
Vk_CommandBuffer::reset()
{
	_vkQueue = nullptr;
}

void 
Vk_CommandBuffer::beginRecord(Vk_Queue* vkQueue, VkCommandBufferUsageFlags usageFlags)
{
	_vkQueue = vkQueue;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags				= usageFlags;		// Optional
	beginInfo.pInheritanceInfo	= nullptr;			// Optional
	auto ret = vkBeginCommandBuffer(hnd(), &beginInfo);
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::endRecord()
{
	auto ret = vkEndCommandBuffer(hnd());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::submit(Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, VkPipelineStageFlags2 waitStage, Vk_Semaphore* signalVkSmp, VkPipelineStageFlags2 signalStage)
{
	VkSubmitInfo2KHR submitInfo = {};

	VkSemaphoreSubmitInfoKHR waitSmpInfo = {};
	waitSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitSmpInfo.semaphore	= waitVkSmp;
	waitSmpInfo.stageMask	= waitStage;

	VkSemaphoreSubmitInfoKHR signalVkSmpInfo = {};
	signalVkSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalVkSmpInfo.semaphore	= signalVkSmp;
	signalVkSmpInfo.stageMask	= signalStage;

	VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
	cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdBufSubmitInfo.commandBuffer	= hnd();

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.waitSemaphoreInfoCount	= 1;
	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.commandBufferInfoCount	= 1;
	submitInfo.pWaitSemaphoreInfos		= &waitSmpInfo;
	submitInfo.pSignalSemaphoreInfos	= &signalVkSmpInfo;
	submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

	//PFN_vkQueueSubmit2KHR vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)renderer()->extInfo().getDeviceExtFunction("vkQueueSubmit2KHR");
	auto ret = vkQueueSubmit2(_vkQueue->hnd(), 1, &submitInfo, signalFence);
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::submit()
{
	VkSubmitInfo2KHR submitInfo = {};

	VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
	cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdBufSubmitInfo.commandBuffer	= hnd();

	submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.commandBufferInfoCount	= 1;
	submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

	auto ret = vkQueueSubmit2(_vkQueue->hnd(), 1, &submitInfo, VK_NULL_HANDLE);
	Util::throwIfError(ret);

	//Renderer_Vk::instance()->extInfo().getDeviceExtFunction<PFN_vkQueueSubmit2KHR>("vkQueueSubmit2KHR");
}

void 
Vk_CommandBuffer::waitIdle()
{
	RDS_CORE_ASSERT(_vkQueue, "{}", RDS_SRCLOC);
	vkQueueWaitIdle(_vkQueue->hnd());
}

void 
Vk_CommandBuffer::cmd_CopyBuffer(Vk_Buffer* dst, Vk_Buffer* src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset)
{
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset	= srcOffset; // Optional
	copyRegion.dstOffset	= dstOffset; // Optional
	copyRegion.size			= size;
	vkCmdCopyBuffer(hnd(), src->hnd(), dst->hnd(), 1, &copyRegion);
}


#endif


}
#endif