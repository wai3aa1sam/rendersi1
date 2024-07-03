#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandBuffer.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Swapchain.h"

#include "rds_render_api_layer/backend/vulkan/pass/rdsVk_RenderPassPool.h"
#include "rds_render_api_layer/backend/vulkan/pass/rdsVk_FramebufferPool.h"

#include "rds_render_api_layer/texture/rdsTexture.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_CommandBuffer_static-Impl ---
#endif // 0
#if 1

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Queue* vkQueue, Span<Vk_CommandBuffer_T*> vkCmdBudHnds, Span<Vk_SmpSubmitInfo> waitSmps, Span<Vk_SmpSubmitInfo> signalSmps)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Queue* vkQueue, Span<Vk_CommandBuffer_T*> vkCmdBudHnds, Vk_Fence* signalFence, Span<Vk_SmpSubmitInfo> waitSmps, Span<Vk_SmpSubmitInfo> signalSmps)
{
	auto n = sCast<VkDeviceSize>(vkCmdBudHnds.size());
	if (n == 0)
		return;

	RDS_CORE_ASSERT(vkQueue && signalFence, "vkQueue && signalFence");

	Vector<VkCommandBufferSubmitInfoKHR, 64> cmdBufSubmitInfos;
	cmdBufSubmitInfos.reserve(n);
	for (auto* hnd : vkCmdBudHnds)
	{
		auto& info = cmdBufSubmitInfos.emplace_back();
		info.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		info.commandBuffer	= hnd;
	}

	VkSubmitInfo2KHR submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.waitSemaphoreInfoCount	= sCast<u32>(waitSmps.size());
	submitInfo.signalSemaphoreInfoCount = sCast<u32>(signalSmps.size());
	submitInfo.commandBufferInfoCount	= sCast<u32>(n);
	submitInfo.pWaitSemaphoreInfos		= waitSmps.data();
	submitInfo.pSignalSemaphoreInfos	= signalSmps.data();;
	submitInfo.pCommandBufferInfos		= cmdBufSubmitInfos.data();
	
	//PFN_vkQueueSubmit2KHR vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)renderer()->extInfo().getDeviceExtFunction("vkQueueSubmit2KHR");
	vkQueue->submit(submitInfo, signalFence->hnd(), debugLabel);
}

#endif

#if 0
#pragma mark --- rdsVk_CommandBuffer-Impl ---
#endif // 0
#if 1

Vk_CommandBuffer::Vk_CommandBuffer()
{

}

Vk_CommandBuffer::~Vk_CommandBuffer()
{
	// when vk cmd pool desttroy, all cmd buf will destroy too
	Base::destroy();
}

void Vk_CommandBuffer::create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level, RenderDevice_Vk* rdDevVk)
{
	_vkCommandPool = vkCommandPool;
	_level = level;

	auto* vkDev = rdDevVk->vkDevice();

	VkCommandBufferAllocateInfo cInfo = {};
	cInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cInfo.commandPool			= _vkCommandPool->hnd();
	cInfo.level					= level;
	cInfo.commandBufferCount	= 1;

	auto ret = vkAllocateCommandBuffers(vkDev, &cInfo, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::destroy(RenderDevice_Vk* rdDevVk)
{
	// when vk cmd pool desttroy, all cmd buf will destroy too
	auto* vkDev = rdDevVk->vkDevice();
	Vk_CommandBuffer_T* vkCmdBufs[] = { hnd()};
	vkFreeCommandBuffers(vkDev, _vkCommandPool->hnd(), ArraySize<decltype(vkCmdBufs)>, vkCmdBufs);
	reset();
	Base::destroy();
}

void 
Vk_CommandBuffer::reset(Vk_Queue* vkQueue)
{
	_vkQueue = vkQueue;
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
	beginDebugLabel(debugName());
}

void 
Vk_CommandBuffer::beginRecord(VkCommandBufferUsageFlags usageFlags, const VkCommandBufferInheritanceInfo* inheriInfo)
{
	RDS_CORE_ASSERT(_vkQueue, "Vk_CommandBuffer has not reset to valid Vk_Queue yet");
	beginRecord(_vkQueue, usageFlags, inheriInfo);
}

void 
Vk_CommandBuffer::beginRecord(const char* debugName, RenderDevice_Vk* rdDevVk, VkCommandBufferUsageFlags usageFlags, const VkCommandBufferInheritanceInfo* inheriInfo)
{
	setDebugName(debugName, rdDevVk);
	beginRecord(_vkQueue, usageFlags, inheriInfo);
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
	endDebugLabel();
	auto ret = vkEndCommandBuffer(hnd());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Fence* signalFence, Vk_Semaphore* signalVkSmp, Vk_PipelineStageFlags signalStage)
{
	return submit(debugLabel, signalFence, nullptr, VK_PIPELINE_STAGE_2_NONE, signalVkSmp, signalStage);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, Vk_PipelineStageFlags waitStage, Vk_Semaphore* signalVkSmp, Vk_PipelineStageFlags signalStage)
{
	VkSubmitInfo2KHR submitInfo = {};

	VkSemaphoreSubmitInfoKHR waitSmpInfo = {};
	if (waitVkSmp)
	{
		waitSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitSmpInfo.semaphore	= waitVkSmp->hnd();
		waitSmpInfo.stageMask	= waitStage;
	}

	VkSemaphoreSubmitInfoKHR signalVkSmpInfo = {};
	if (signalVkSmp)
	{
		signalVkSmpInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalVkSmpInfo.semaphore	= signalVkSmp->hnd();
		signalVkSmpInfo.stageMask	= signalStage;
	}
	
	VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
	cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdBufSubmitInfo.commandBuffer	= hnd();

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.waitSemaphoreInfoCount	= waitVkSmp		? 1 : 0;
	submitInfo.signalSemaphoreInfoCount = signalVkSmp	? 1 : 0;
	submitInfo.commandBufferInfoCount	= 1;
	submitInfo.pWaitSemaphoreInfos		= &waitSmpInfo;
	submitInfo.pSignalSemaphoreInfos	= &signalVkSmpInfo;
	submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

	//PFN_vkQueueSubmit2KHR vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)renderer()->extInfo().getDeviceExtFunction("vkQueueSubmit2KHR");
	_vkQueue->submit(submitInfo, signalFence->hnd(), debugLabel);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Fence* signalFence, Span<Vk_SmpSubmitInfo> waitSmps, Span<Vk_SmpSubmitInfo> signalSmps)
{
	VkSubmitInfo2KHR submitInfo = {};

	VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
	cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdBufSubmitInfo.commandBuffer	= hnd();

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.waitSemaphoreInfoCount	= sCast<u32>(waitSmps.size());
	submitInfo.signalSemaphoreInfoCount = sCast<u32>(signalSmps.size());
	submitInfo.commandBufferInfoCount	= 1;
	submitInfo.pWaitSemaphoreInfos		= waitSmps.data();
	submitInfo.pSignalSemaphoreInfos	= signalSmps.data();;
	submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

	//PFN_vkQueueSubmit2KHR vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)renderer()->extInfo().getDeviceExtFunction("vkQueueSubmit2KHR");
	_vkQueue->submit(submitInfo, signalFence->hnd(), debugLabel);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel, Vk_Fence* signalFence)
{
	return submit(debugLabel, signalFence, nullptr, VK_PIPELINE_STAGE_2_NONE, nullptr, VK_PIPELINE_STAGE_2_NONE);
}

void 
Vk_CommandBuffer::submit(const RenderDebugLabel& debugLabel)
{
	VkSubmitInfo2KHR submitInfo = {};

	VkCommandBufferSubmitInfoKHR cmdBufSubmitInfo = {};
	cmdBufSubmitInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdBufSubmitInfo.commandBuffer	= hnd();

	submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.commandBufferInfoCount	= 1;
	submitInfo.pCommandBufferInfos		= &cmdBufSubmitInfo;

	_vkQueue->submit(submitInfo, VK_NULL_HANDLE, debugLabel);
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

void 
Vk_CommandBuffer::setViewport(const Rect2f& rect, float minDepth, float maxDepth)
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
Vk_CommandBuffer::setViewportReverse(const Rect2f& rect, float minDepth, float maxDepth)
{
	Rect2f viewport = {};
	viewport.x		= rect.x;
	viewport.y		= rect.y + rect.h;
	viewport.w		= rect.w;
	viewport.h		= -rect.h;

	setViewport(viewport, minDepth, maxDepth);
}

void 
Vk_CommandBuffer::setScissor(const Rect2f& rect)
{
	VkRect2D scissor = {};
	scissor.offset = { sCast<int>(rect.x), sCast<int>(rect.y)};
	scissor.extent = Util::toVkExtent2D(rect);
	vkCmdSetScissor(hnd(), 0, 1, &scissor);
}

//void 
//Vk_CommandBuffer::beginRenderPass(Vk_RenderPass_T* vkRdPassHnd, Vk_Framebuffer_T* vkFramebufHnd, const Rect2f& rect2, Span<VkClearValue> vkClearValues, VkSubpassContents subpassContents)
//{
//	throwIf(_vkRdPassHnd, "render pass already begin");
//	_vkRdPassHnd = vkRdPassHnd;
//
//	VkRenderPassBeginInfo renderPassInfo = {};
//	renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//	renderPassInfo.renderPass			= vkRdPassHnd;
//	renderPassInfo.framebuffer			= vkFramebufHnd;
//	renderPassInfo.renderArea.offset	= Util::toVkOffset2D(rect2);
//	renderPassInfo.renderArea.extent	= Util::toVkExtent2D(rect2);
//	renderPassInfo.clearValueCount		= sCast<u32>(vkClearValues.size());
//	renderPassInfo.pClearValues			= vkClearValues.data();	// for VK_ATTACHMENT_LOAD_OP_CLEAR flag
//
//	//VkSubpassContents subpassContents = _level == VK_COMMAND_BUFFER_LEVEL_PRIMARY ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
//	vkCmdBeginRenderPass(hnd(), &renderPassInfo, subpassContents);
//}

void 
Vk_CommandBuffer::beginRenderPass(Vk_RenderPass* vkRdPass, Vk_Framebuffer* vkFramebuf, const Rect2f& rect2, Span<VkClearValue> vkClearValues, VkSubpassContents subpassContents)
{
	RDS_CORE_ASSERT(vkRdPass && vkFramebuf, "");
	throwIf(_vkRdPass, "render pass already begin");
	_vkRdPass = vkRdPass;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass			= vkRdPass->hnd();
	renderPassInfo.framebuffer			= vkFramebuf->hnd();
	renderPassInfo.renderArea.offset	= Util::toVkOffset2D(rect2);
	renderPassInfo.renderArea.extent	= Util::toVkExtent2D(rect2);
	renderPassInfo.clearValueCount		= sCast<u32>(vkClearValues.size());
	renderPassInfo.pClearValues			= vkClearValues.data();	// for VK_ATTACHMENT_LOAD_OP_CLEAR flag

	//VkSubpassContents subpassContents = _level == VK_COMMAND_BUFFER_LEVEL_PRIMARY ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
	vkCmdBeginRenderPass(hnd(), &renderPassInfo, subpassContents);

	//beginRenderPass(vkRenderPass->hnd(), vkFramebuf->hnd(), rect2, vkClearValues, subpassContents);
}

void 
Vk_CommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass(hnd());
	_vkRdPass = nullptr;
}

#if 0
--#pragma mark --- rdsVk_CommandBuffer::cmd-Impl
#endif // 0
#if 1

void 
Vk_CommandBuffer::cmd_dispatch(u32 threadGrpX, u32 threadGrpY, u32 threadGrpZ)
{
	vkCmdDispatch(hnd(), threadGrpX, threadGrpY, threadGrpZ);
}

void 
Vk_CommandBuffer::cmd_copyBuffer(Vk_Buffer* dst, Vk_Buffer* src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset)
{
	RDS_CORE_ASSERT(dst, "dst == nullptr");
	RDS_CORE_ASSERT(src, "src == nullptr");

	cmd_copyBuffer(dst->hnd(), src->hnd(), size, dstOffset, srcOffset);
}

void 
Vk_CommandBuffer::cmd_copyBuffer(Vk_Buffer_T* dst, Vk_Buffer_T* src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset)
{
	RDS_CORE_ASSERT(dst, "dst == nullptr");
	RDS_CORE_ASSERT(src, "src == nullptr");

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset	= srcOffset; // Optional
	copyRegion.dstOffset	= dstOffset; // Optional
	copyRegion.size			= size;
	vkCmdCopyBuffer(hnd(), src, dst, 1, &copyRegion);
}

void 
Vk_CommandBuffer::cmd_copyBufferToImage(Vk_Image_T* dst, Vk_Buffer_T* src, VkImageLayout layout, Span<VkBufferImageCopy> copyDescs)
{
	vkCmdCopyBufferToImage(
		hnd(),
		src,
		dst,
		layout,
		sCast<u32>(copyDescs.size()),
		copyDescs.data()
	);
}

void 
Vk_CommandBuffer::cmd_copyImage(Vk_Image_T* dst, Vk_Image_T* src, VkImageLayout srcLayout, VkImageLayout dstLayout, const VkImageCopy& copyRegion)
{
	vkCmdCopyImage(hnd(), src, srcLayout, dst, dstLayout, 1, &copyRegion);
}

void 
Vk_CommandBuffer::cmd_copyImage(Vk_Image_T* dst, Vk_Image_T* src, VkImageLayout srcLayout, VkImageLayout dstLayout, Tuple3u extent, u32 srcBaseLayer, u32 dstBaseLayer, u32 srcMip, u32 dstMip)
{
	VkImageCopy copyRegion = {};
	copyRegion.extent = Util::toVkExtent3D(extent);
	copyRegion.srcSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.baseArrayLayer	= srcBaseLayer;
	copyRegion.srcSubresource.mipLevel			= srcMip;
	copyRegion.srcSubresource.layerCount		= 1;

	copyRegion.dstSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.baseArrayLayer	= dstBaseLayer;
	copyRegion.dstSubresource.mipLevel			= dstMip;
	copyRegion.dstSubresource.layerCount		= 1;

	cmd_copyImage(dst, src, srcLayout, dstLayout, copyRegion);
}

void 
Vk_CommandBuffer::cmd_copyBufferToImage(Vk_Image_T* dst, Vk_Buffer_T* src, VkImageLayout layout, u32 width, u32 height, u32 srcOffset)
{
	RDS_CORE_ASSERT(width != 0 || height != 0, "");

	VkBufferImageCopy region = {};
	region.bufferOffset			= srcOffset;
	region.bufferRowLength		= 0;
	region.bufferImageHeight	= 0;

	region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel		= 0;
	region.imageSubresource.baseArrayLayer	= 0;
	region.imageSubresource.layerCount		= 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
		hnd(),
		src,
		dst,
		layout,
		1,
		&region
	);
}

void 
Vk_CommandBuffer::cmd_addMemoryBarrier(Vk_StageAccess vkStageAccess)
{
	
	#if 0
	VkMemoryBarrier2KHR memoryBarrier = {};
	memoryBarrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	memoryBarrier.srcStageMask	= vkStageAccess.srcStage;
	memoryBarrier.dstStageMask	= vkStageAccess.dstStage;
	memoryBarrier.srcAccessMask	= vkStageAccess.srcAccess;
	memoryBarrier.dstAccessMask	= vkStageAccess.dstAccess; 

	VkDependencyInfoKHR dependencyInfo = {};
	dependencyInfo.memoryBarrierCount	= 1;
	dependencyInfo.pMemoryBarriers		= &memoryBarrier;

	vkCmdPipelineBarrier2KHR(hnd(), &dependencyInfo);

	#else

	VkMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = vkStageAccess.srcAccess;
	memoryBarrier.dstAccessMask = vkStageAccess.dstAccess;

	//vkCmdPipelineBarrier(hnd(), vkStageAccess.srcStage, vkStageAccess.dstStage, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
	//vkCmdPipelineBarrier(hnd(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, vkStageAccess.dstStage, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);

	#endif // 0
}

void 
Vk_CommandBuffer::cmd_addMemoryBarrier(VkPipelineStageFlags srcVkStage, VkPipelineStageFlags dstVkStage, VkAccessFlagBits srcAccess, VkAccessFlagBits dstAccess)
{
	Vk_StageAccess vkStageAccess = {};
	vkStageAccess.srcStage	= srcVkStage;
	vkStageAccess.dstStage	= dstVkStage;
	vkStageAccess.srcAccess = srcAccess;
	vkStageAccess.dstAccess = dstAccess;

	cmd_addMemoryBarrier(vkStageAccess);
}

void 
Vk_CommandBuffer::cmd_addMemoryBarrier(RenderGpuBufferTypeFlags srcVkStage, RenderGpuBufferTypeFlags dstVkStage, RenderAccess srcAccess, RenderAccess dstAccess)
{
	cmd_addMemoryBarrier(Util::toVkStageAccess(srcVkStage, dstVkStage, srcAccess, dstAccess));
}

void 
Vk_CommandBuffer::cmd_addMemoryBarrier(RenderResourceStateFlags srcState, RenderResourceStateFlags dstState)
{
	using StateUtil = RenderResourceStateFlagsUtil;
	cmd_addMemoryBarrier(Util::toVkStageAccess(StateUtil::getBufferUsageFlags(srcState), StateUtil::getBufferUsageFlags(dstState)
						, StateUtil::getRenderAccess(srcState), StateUtil::getRenderAccess(dstState)));
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(const Vk_Cmd_AddImageMemBarrierDesc& desc)
{
	auto vkStageAcess = Util::toVkStageAccess(desc.srcLayout, desc.dstLayout, desc.srcPipelineStage, desc.dstPipelineStage);

	bool isTransferOwnership = desc.srcQueueFamilyIdx != VK_QUEUE_FAMILY_IGNORED || desc.dstQueueFamilyIdx != VK_QUEUE_FAMILY_IGNORED;
	if (isTransferOwnership)
	{
		if (desc.isSrcQueueOwner)
		{
			vkStageAcess.dstStage	= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_NONE;
			vkStageAcess.dstAccess	= VkAccessFlagBits::VK_ACCESS_NONE;
		}
		else
		{
			vkStageAcess.srcStage	= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_NONE;
			vkStageAcess.srcAccess	= VkAccessFlagBits::VK_ACCESS_NONE;
		}
	}


	VkImageAspectFlags aspectMask = Util::isDepthFormat(desc.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	if (Util::hasStencilComponent(desc.format)) 
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	#if 1

	VkImageMemoryBarrier barrier = {};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout						= desc.srcLayout;
	barrier.newLayout						= desc.dstLayout;
	barrier.srcQueueFamilyIndex				= desc.srcQueueFamilyIdx;
	barrier.dstQueueFamilyIndex				= desc.dstQueueFamilyIdx;
	barrier.image							= desc.image;

	barrier.subresourceRange.aspectMask		= aspectMask;
	barrier.subresourceRange.baseMipLevel	= desc.baseMip;
	barrier.subresourceRange.baseArrayLayer	= desc.baseLayer;
	barrier.subresourceRange.levelCount		= desc.mipCount;
	barrier.subresourceRange.layerCount		= desc.layerCount;

	barrier.srcAccessMask = vkStageAcess.srcAccess;
	barrier.dstAccessMask = vkStageAcess.dstAccess;

	vkCmdPipelineBarrier(
		hnd(),
		vkStageAcess.srcStage , vkStageAcess.dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	#else

	VkImageMemoryBarrier2 barrier;
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.oldLayout						= srcLayout;
	barrier.newLayout						= dstLayout;
	barrier.srcQueueFamilyIndex				= dstQueue ? srcQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= dstQueue ? dstQueue->familyIdx() : VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= image->hnd();

	barrier.subresourceRange.aspectMask		= aspectMask;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.layerCount		= 1;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.srcStageMask  = srcStage;
	barrier.dstStageMask  = dstStage;

	VkDependencyInfo depInfo = {};
	depInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers	= &barrier;

	auto vkCmdPipelineBarrier23 = Renderer_Vk::instance()->extInfo().getDeviceExtFunction<PFN_vkCmdPipelineBarrier2>("vkCmdPipelineBarrier2");
	vkCmdPipelineBarrier23(vkCmdBuf->hnd(), &depInfo);

	#endif // 0
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout)
{
	cmd_addImageMemBarrier(image->hnd(), vkFormat, srcLayout, dstLayout);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout)
{
	Vk_Cmd_AddImageMemBarrierDesc desc = {};
	desc.image		= image;
	desc.format		= vkFormat;
	desc.srcLayout	= srcLayout;
	desc.dstLayout	= dstLayout;

	cmd_addImageMemBarrier(desc);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout, u32 baseMip, u32 mipCount, u32 baseLayer, u32 layerCount)
{
	Vk_Cmd_AddImageMemBarrierDesc desc = {};
	desc.image		= image;
	desc.format		= vkFormat;
	desc.srcLayout	= srcLayout;
	desc.dstLayout	= dstLayout;

	desc.baseMip	= sCast<u8>(baseMip);
	desc.mipCount	= sCast<u8>(mipCount);
	desc.baseLayer	= sCast<u16>(baseLayer);
	desc.layerCount	= sCast<u16>(layerCount);

	cmd_addImageMemBarrier(desc);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout, u32 srcQueueFamilyIdx, u32 dstQueueFamilyIdx, bool isSrcQueueOwner)
{
	Vk_Cmd_AddImageMemBarrierDesc desc = {};
	desc.image				= image;
	desc.format				= vkFormat;
	desc.srcLayout			= srcLayout;
	desc.dstLayout			= dstLayout;
	desc.srcQueueFamilyIdx	= srcQueueFamilyIdx;
	desc.dstQueueFamilyIdx	= dstQueueFamilyIdx;
	desc.isSrcQueueOwner	= isSrcQueueOwner;

	cmd_addImageMemBarrier(desc);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkImageLayout srcLayout, VkImageLayout dstLayout
										, VkPipelineStageFlags srcPipelineStage, VkPipelineStageFlags dstPipelineStage
										, const Texture_Desc& texDesc, u32 srcQueueFamilyIdx, u32 dstQueueFamilyIdx, bool isSrcQueueOwner)
{
	Vk_Cmd_AddImageMemBarrierDesc desc = {};
	desc.image				= image;
	desc.format				= Util::toVkFormat(texDesc.format);
	desc.srcLayout			= srcLayout;
	desc.dstLayout			= dstLayout;
	desc.srcPipelineStage	= srcPipelineStage;
	desc.dstPipelineStage	= dstPipelineStage;
	desc.srcQueueFamilyIdx	= srcQueueFamilyIdx;
	desc.dstQueueFamilyIdx	= dstQueueFamilyIdx;
	desc.isSrcQueueOwner	= isSrcQueueOwner;

	desc.baseMip			= 0;
	desc.mipCount			= texDesc.mipCount;
	desc.baseLayer			= 0;
	desc.layerCount			= texDesc.layerCount;

	cmd_addImageMemBarrier(desc);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkImageLayout srcLayout, VkImageLayout dstLayout, ShaderStageFlag srcShaderStage, ShaderStageFlag dstShaderStage, const Texture_Desc& texDesc)
{
	cmd_addImageMemBarrier(image, srcLayout, dstLayout
							, Util::toVkPipelineStageFlag(srcShaderStage), Util::toVkPipelineStageFlag(dstShaderStage)
							, texDesc, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, false);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkImageLayout srcLayout, VkImageLayout dstLayout, const Texture_Desc& texDesc)
{
	cmd_addImageMemBarrier(image, srcLayout, dstLayout
							, VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM, VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM
							, texDesc, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, false);
}

void 
Vk_CommandBuffer::cmd_addImageMemBarrier(Vk_Image_T* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout, u32 baseMip, u32 mipCount, u32 baseLayer, u32 layerCount, u32 srcQueueFamilyIdx, u32 dstQueueFamilyIdx, bool isSrcQueueOwner)
{
	Vk_Cmd_AddImageMemBarrierDesc desc = {};
	desc.image				= image;
	desc.format				= vkFormat;
	desc.srcLayout			= srcLayout;
	desc.dstLayout			= dstLayout;
	desc.srcQueueFamilyIdx	= srcQueueFamilyIdx;
	desc.dstQueueFamilyIdx	= dstQueueFamilyIdx;
	desc.isSrcQueueOwner	= isSrcQueueOwner;

	desc.baseMip			= sCast<u8>(baseMip);
	desc.mipCount			= sCast<u8>(mipCount);
	desc.baseLayer			= sCast<u16>(baseLayer);
	desc.layerCount			= sCast<u16>(layerCount);

	cmd_addImageMemBarrier(desc);
}


#endif // 1

void 
Vk_CommandBuffer::beginDebugLabel(const char* name, const Color4f& color)
{
	#if RDS_DEVELOPMENT
	VkDebugUtilsLabelEXT debugLabel = Util::toVkDebugUtilsLabel(name, color);
	vkCmdBeginDebugUtilsLabel(hnd(), &debugLabel);
	#endif // RDS_DEVELOP
}

void 
Vk_CommandBuffer::endDebugLabel	()
{
	#if RDS_DEVELOPMENT
	vkCmdEndDebugUtilsLabel(hnd());
	#endif
}

void 
Vk_CommandBuffer::insertDebugLabel(const char* name, const Color4f& color)
{
	#if RDS_DEVELOPMENT
	VkDebugUtilsLabelEXT debugLabel = Util::toVkDebugUtilsLabel(name, color);
	vkCmdInsertDebugUtilsLabel(hnd(), &debugLabel);
	#endif
}

//Renderer_Vk*		Vk_CommandBuffer::renderer()	{ return _vkCommandPool-> }
//RenderDevice_Vk*	Vk_CommandBuffer::device()		{ return _vkC }


#endif


}
#endif