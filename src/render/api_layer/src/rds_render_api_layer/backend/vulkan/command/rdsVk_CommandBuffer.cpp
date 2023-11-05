#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandBuffer.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Swapchain.h"

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
Vk_CommandBuffer::beginRecord(Vk_Queue* vkQueue, VkCommandBufferUsageFlags usageFlags, const VkCommandBufferInheritanceInfo* inheriInfo)
{
	RDS_CORE_ASSERT(!inheriInfo || (inheriInfo && BitUtil::has(usageFlags, (VkFlags)VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT)), "");

	_vkQueue = vkQueue;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags				= usageFlags;		// Optional
	beginInfo.pInheritanceInfo	= inheriInfo;		// Optional

	if (inheriInfo)
		usageFlags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

	auto ret = vkBeginCommandBuffer(hnd(), &beginInfo);
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::beginSecondaryRecord(Vk_Queue* vkQueue, Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer, u32 subpassIdx, VkCommandBufferUsageFlags usageFlags)
{
	VkCommandBufferInheritanceInfo cmdBufferInheritanceInfo = {};
	cmdBufferInheritanceInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	cmdBufferInheritanceInfo.renderPass		= vkRenderPass->hnd();
	cmdBufferInheritanceInfo.framebuffer	= vkFramebuffer->hnd();
	cmdBufferInheritanceInfo.subpass		= subpassIdx;

	beginRecord(vkQueue, usageFlags, &cmdBufferInheritanceInfo);
}

void 
Vk_CommandBuffer::endRecord()
{
	auto ret = vkEndCommandBuffer(hnd());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::submit(Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, Vk_PipelineStageFlags waitStage, Vk_Semaphore* signalVkSmp, Vk_PipelineStageFlags signalStage)
{
	VkSubmitInfo2KHR submitInfo = {};

	VkSemaphoreSubmitInfoKHR waitSmpInfo = {};
	waitSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitSmpInfo.semaphore	= waitVkSmp->hnd();
	waitSmpInfo.stageMask	= waitStage;

	VkSemaphoreSubmitInfoKHR signalVkSmpInfo = {};
	signalVkSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalVkSmpInfo.semaphore	= signalVkSmp->hnd();
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
	auto ret = vkQueueSubmit2(_vkQueue->hnd(), 1, &submitInfo, signalFence->hnd());
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
Vk_CommandBuffer::executeSecondaryCmdBufs(Span<Vk_CommandBuffer*> cmdBufs)
{
	Vector<Vk_CommandBuffer_T*, 32> vkCmdBufs;
	auto n = sCast<u32>(cmdBufs.size());
	vkCmdBufs.resize(n);
	Util::convertToHnds(vkCmdBufs, cmdBufs);

	vkCmdExecuteCommands(hnd(), n, vkCmdBufs.data());
}

void 
Vk_CommandBuffer::waitIdle()
{
	RDS_CORE_ASSERT(_vkQueue, "{}", RDS_SRCLOC);
	vkQueueWaitIdle(_vkQueue->hnd());
}

VkResult
Vk_CommandBuffer::swapBuffers(Vk_Queue* vkPresentQueue, Vk_Swapchain* vkSwpachain, u32 imageIdx, Vk_Semaphore* vkWaitSmp)
{
	VkPresentInfoKHR presentInfo = {};

	Vk_Semaphore_T*	waitVkSmps[]   = { vkWaitSmp->hnd()};
	Vk_Swapchain_T*	vkSwapChains[] = { vkSwpachain->hnd()};
	u32				imageIndices[] = { imageIdx };

	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount	= ArraySize<decltype(waitVkSmps)>;
	presentInfo.swapchainCount		= ArraySize<decltype(vkSwapChains)>;
	presentInfo.pWaitSemaphores		= waitVkSmps;
	presentInfo.pSwapchains			= vkSwapChains;
	presentInfo.pImageIndices		= imageIndices;
	presentInfo.pResults			= nullptr; // Optional, allows to check for every individual swap chain if presentation was successful

	auto ret = vkQueuePresentKHR(vkPresentQueue->hnd(), &presentInfo);
	return ret;
	/*if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR)
	{
	createSwapchain();
	} */
	//Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::setViewport(const math::Rect2f& rect, float minDepth, float maxDepth)
{
	VkViewport viewport = {};
	viewport.x			= rect.x;
	viewport.y			= rect.y;
	viewport.width		= rect.w;
	viewport.height		= rect.h;
	viewport.minDepth	= minDepth;
	viewport.maxDepth	= maxDepth;
	vkCmdSetViewport(hnd(), 0, 1, &viewport);
}

void 
Vk_CommandBuffer::setScissor(const math::Rect2f& rect)
{
	VkRect2D scissor = {};
	scissor.offset = { sCast<int>(rect.x), sCast<int>(rect.y)};
	scissor.extent = Util::toVkExtent2D(rect);
	vkCmdSetScissor(hnd(), 0, 1, &scissor);
}

void 
Vk_CommandBuffer::beginRenderPass(Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer, const math::Rect2f& rect2, Span<VkClearValue> vkClearValues, VkSubpassContents subpassContents)
{
	throwIf(_vkRdPass, "render pass already begin");
	_vkRdPass = vkRenderPass;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass			= vkRenderPass->hnd();
	renderPassInfo.framebuffer			= vkFramebuffer->hnd();
	renderPassInfo.renderArea.offset	= Util::toVkOffset2D(rect2);
	renderPassInfo.renderArea.extent	= Util::toVkExtent2D(rect2);
	renderPassInfo.clearValueCount		= sCast<u32>(vkClearValues.size());
	renderPassInfo.pClearValues			= vkClearValues.data();	// for VK_ATTACHMENT_LOAD_OP_CLEAR flag

	//VkSubpassContents subpassContents = _level == VK_COMMAND_BUFFER_LEVEL_PRIMARY ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
	vkCmdBeginRenderPass(hnd(), &renderPassInfo, subpassContents);
}

void 
Vk_CommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass(hnd());
	_vkRdPass = nullptr;
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