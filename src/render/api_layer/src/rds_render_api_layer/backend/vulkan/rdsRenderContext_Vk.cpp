#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderer_Vk.h"

#include "rds_render_api_layer/command/rdsTransferRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

SPtr<RenderContext> Renderer_Vk::onCreateContext(const RenderContext_CreateDesc& cDesc)
{
	auto p = SPtr<RenderContext>(RDS_NEW(RenderContext_Vk)());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderContext_Vk-Impl ---
#endif // 0
#if 1

RenderContext_Vk::RenderContext_Vk()
	: Base()
{

}

RenderContext_Vk::~RenderContext_Vk()
{
	destroy();
}

void
RenderContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	_renderer = Renderer_Vk::instance();
	auto vkDevice = _renderer->vkDevice();

	Util::createSurface(_vkSurface.ptrForInit(), _renderer->vkInstance(), _renderer->allocCallbacks(), cDesc.window);

	_renderFrames.resize(s_kFrameInFlightCount);
	for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		_renderFrames[i].create();
	}

	u32	graphicsQueueIdx = 0;
	u32	presentQueueIdx = 0;
	u32	transferQueueIdx = 0;
	vkGetDeviceQueue(vkDevice, _renderer->queueFamilyIndices().graphics.value(), graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(vkDevice, _renderer->queueFamilyIndices().present.value(), presentQueueIdx, _vkPresentQueue.ptrForInit());
	vkGetDeviceQueue(vkDevice, _renderer->queueFamilyIndices().transfer.value(), transferQueueIdx, _vkTransferQueue.ptrForInit());

	createSwapchainInfo(_swapchainInfo, _renderer->swapchainAvailableInfo(), cDesc.window->clientRect());

	createSyncObjects();

	createTestDescriptorSetLayout();
	createTestUniformBuffer();
	createTestDescriptorPool();
	createTestDescriptorSets();
	updateTestUBO(_curImageIdx);

	createTestRenderPass();
	createTestGraphicsPipeline();
	createTestVertexBuffer();
	createTestIndexBuffer();

	createSwapchain(_swapchainInfo);

	_curGraphicsCmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	RDS_PROFILE_GPU_CTX_CREATE(_gpuProfilerCtx, "Main Window");
}

void
RenderContext_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void
RenderContext_Vk::onDestroy()
{
	Base::onDestroy();
	vkDeviceWaitIdle(renderer()->vkDevice());
}

void
RenderContext_Vk::onBeginRender()
{
	RDS_PROFILE_SCOPED();

	auto* vkDevice = renderer()->vkDevice();
	VkResult ret;

	//const auto* inFlightVkFence = reinCast<const VkFence*>(_inFlightVkFence.address());
	Vk_Fence* vkFences[] = { _inFlightVkFences[_curFrameIdx] };
	u32 vkFenceCount = ArraySize<decltype(vkFences)>;
	{
		RDS_PROFILE_SECTION("vkWaitForFences()");
		vkWaitForFences(vkDevice, vkFenceCount, vkFences, VK_TRUE, NumLimit<u64>::max());
	}
	//vkResetFences(vkDevice, vkFenceCount, vkFences);		// reset too early will cause deadlock, since the invalidate wii cause no work submitted (returned) and then no one will signal it

	{
		RDS_PROFILE_SECTION("vkAcquireNextImageKHR()");
		u32 imageIdx;
		ret = vkAcquireNextImageKHR(vkDevice, _vkSwapchain, NumLimit<u64>::max(), _imageAvailableVkSmps[_curFrameIdx], VK_NULL_HANDLE, &imageIdx);
		_curImageIdx = imageIdx;
	}

	#if 1

	if (ret == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return;
	}

	#else

	// VK_ERROR_OUT_OF_DATE_KHR		means window size is changed
	// VK_SUBOPTIMAL_KHR			means sucessfully presented but the surface properties are not matched
	if (ret == VK_ERROR_OUT_OF_DATE_KHR)
	{
		createSwapchain();
		return; // should not record if size is changed
	}
	else
	{
		throwIf(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR, "");	// VK_SUBOPTIMAL_KHR 
	}

	#endif // 1

	{
		_curGraphicsCmdBuf	= renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		auto* vkCmdBuf		= _curGraphicsCmdBuf->hnd();
		beginRecord(vkCmdBuf, _curImageIdx);

		//RDS_PROFILE_GPU_ZONE_VK(_gpuProfilerCtx, vkCmdBuf, "Test Draw Call");
		RDS_PROFILE_GPU_ZONET_VK(_gpuProfilerCtx, vkCmdBuf, "Test Draw Call");
		testDrawCall(vkCmdBuf, _curImageIdx);
	}
}

void
RenderContext_Vk::onEndRender()
{
	RDS_PROFILE_SCOPED();

	endRecord(_curGraphicsCmdBuf->hnd());

	{
		VkPresentInfoKHR presentInfo = {};

		Vk_Semaphore*	waitVkSmps[]   = { _renderCompletedVkSmps[_curFrameIdx] };
		Vk_Swapchain*	vkSwapChains[] = { _vkSwapchain };
		u32				imageIndices[] = { _curImageIdx };

		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = ArraySize<decltype(waitVkSmps)>;
		presentInfo.swapchainCount = ArraySize<decltype(vkSwapChains)>;
		presentInfo.pWaitSemaphores = waitVkSmps;
		presentInfo.pSwapchains = vkSwapChains;
		presentInfo.pImageIndices = imageIndices;
		presentInfo.pResults = nullptr; // Optional, allows to check for every individual swap chain if presentation was successful

		auto ret = vkQueuePresentKHR(_vkPresentQueue, &presentInfo);
		/*if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR)
		{
		createSwapchain();
		} */
		Util::throwIfError(ret);
	}

	// next frame idx
	_curFrameIdx = math::modPow2Val(_curFrameIdx + 1, s_kFrameInFlightCount);
}

void
RenderContext_Vk::onSetFramebufferSize(const Vec2f& newSize)
{
	if (newSize.x == 0 || newSize.y == 0)
		return;

	// fix validation layer false positive error: vkCreateSwapchainKHR imageExtent surface capabilities
	// https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/1340
	auto ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer()->vkPhysicalDevice(), _vkSurface, &renderer()->_swapchainAvaliableInfo.capabilities); (void)ret;

	_swapchainInfo.extent = Util::toVkExtent2D(newSize);
	createSwapchain(_swapchainInfo);
}

void 
RenderContext_Vk::onCommit(TransferCommandBuffer& transferBuf)
{
	Base::onCommit(transferBuf);

}

void 
RenderContext_Vk::onUploadBuffer(RenderFrameUploadBuffer& rdfUploadBuf)
{
	Base::onUploadBuffer(rdfUploadBuf);
	_onUploadBuffer_MemCopyMutex(rdfUploadBuf);
}

void 
RenderContext_Vk::_onUploadBuffer_MemCopyMutex(RenderFrameUploadBuffer& rdfUploadBuf)
{
	auto* allocVk	= Renderer_Vk::instance()->memoryContext()->allocVk();

	auto totalSize					= rdfUploadBuf.totalDataSize();
	const auto* inlineUploadBuffer	= rdfUploadBuf.getInlineUploadBuffer();

	if (totalSize == 0)
	{
		return;
	}

	Vk_Buffer vkStageBuf;

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage  = RenderMemoryUsage::CpuOnly;
	allocInfo.flags |= RenderAllocFlags::HostWrite;
	Util::createBuffer(vkStageBuf, allocVk, &allocInfo, totalSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

	{
		const auto& chunks	= inlineUploadBuffer->bufData.chunks();
		auto chunkCount		= chunks.size();

		u8* mappedData = nullptr;
		Vk_ScopedMemMap mm = { &mappedData, &vkStageBuf };

		RDS_CORE_ASSERT(chunkCount == 1, "not yet handle > 1");
		for (size_t i = 0; i < chunkCount; i++)
		{
			auto& e = chunks[i];
			memory_copy(mappedData, e->data(), e->size());
			mappedData += e->size();
		}
	}

	auto* transferCmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	transferCmdBuf->beginRecord(_vkTransferQueue, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	
	auto CmdCount = inlineUploadBuffer->uploadBufCmds.size();
	for (size_t iCmd = 0; iCmd < CmdCount; iCmd++)
	{
		auto& cmd = inlineUploadBuffer->uploadBufCmds[iCmd];
		auto* dstBuf = sCast<RenderGpuBuffer_Vk*>(cmd->dst.ptr());
		transferCmdBuf->cmd_CopyBuffer(dstBuf->vkBuf(), &vkStageBuf, cmd->data.size(), 0, inlineUploadBuffer->bufOffsets[iCmd]);		// dst offset already handled in RenderGpuBuffer::onUpload()
		RDS_ASSERT(!BitUtil::has(cmd->queueTypeflags, QueueTypeFlags::Compute), "not yet support");
	}
	
	{
		static bool v = false;
		if (!v)
		{
			RDS_LOG_WARN("TODO: add smp for transfer and graphics queue");
			v = true;
		}
	}

	transferCmdBuf->endRecord();
	transferCmdBuf->submit();

	transferCmdBuf->waitIdle(); // TODO: remove

	// TODO: delay rotate
	auto& rotates = constCast(inlineUploadBuffer->parents);
	for (auto& e : rotates)
	{
		e->rotate();
	}
}

void
RenderContext_Vk::beginRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();

	renderFrame().reset();

	updateTestUBO(imageIdx);
	_curGraphicsCmdBuf->beginRecord(vkGraphicsQueue(), VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

void
RenderContext_Vk::endRecord(Vk_CommandBuffer_T* vkCmdBuf)
{
	RDS_PROFILE_SCOPED();

	VkResult ret;
	auto* vkDevice = renderer()->vkDevice();

	RDS_PROFILE_GPU_COLLECT_VK(_gpuProfilerCtx, vkCmdBuf);

	_curGraphicsCmdBuf->endRecord();

	{
		Vk_Fence* vkFences[] = { _inFlightVkFences[_curFrameIdx] };
		u32 vkFenceCount = ArraySize<decltype(vkFences)>;
		ret = vkResetFences(vkDevice, vkFenceCount, vkFences);		// should handle it to signaled if the function throw?
		Util::throwIfError(ret);
	}

	_curGraphicsCmdBuf->submit(_inFlightVkFences[_curFrameIdx]
								, _imageAvailableVkSmps[_curFrameIdx],	VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
								, _renderCompletedVkSmps[_curFrameIdx], VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void
RenderContext_Vk::bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline)
{
	RDS_PROFILE_SCOPED();

	vkCmdBindPipeline(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	// since we set viewport and scissor state is dynamic
	VkViewport			viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = sCast<float>(_swapchainInfo.extent.width);
	viewport.height = sCast<float>(_swapchainInfo.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkCmdBuf, 0, 1, &viewport);

	VkRect2D		scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = _swapchainInfo.extent;
	vkCmdSetScissor(vkCmdBuf, 0, 1, &scissor);

}

void
RenderContext_Vk::testDrawCall(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();

	{
		VkClearValue clearValues[] = { VkClearValue(), VkClearValue() };
		clearValues[0].color = { { 0.8f, 0.6f, 0.4f, 1.0f } };
		//clearValues[0].color		= {{ 0.0f, 0.0f, 0.0f, 1.0f }};
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _testVkRenderPass;
		renderPassInfo.framebuffer = _vkSwapchainFramebuffers[imageIdx];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _swapchainInfo.extent;
		renderPassInfo.clearValueCount = ArraySize<decltype(clearValues)>;
		renderPassInfo.pClearValues = clearValues;	// for VK_ATTACHMENT_LOAD_OP_CLEAR flag

		vkCmdBeginRenderPass(vkCmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	bindPipeline(vkCmdBuf, _testVkPipeline);

	{
		Vk_Buffer_T* vertexBuffers[]	= { _testVkVtxBuffer.hnd() };
		VkDeviceSize offsets[]			= { 0 };
		vkCmdBindVertexBuffers(vkCmdBuf, 0, 1, vertexBuffers, offsets);
	}

	{
		vkCmdBindIndexBuffer(vkCmdBuf, _testVkIdxBuffer.hnd(), 0, VK_INDEX_TYPE_UINT16);
	}

	{
		Vk_DescriptorSet_T* descSets[] = { _testVkDescriptorSets[imageIdx].hnd() };
		vkCmdBindDescriptorSets(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _testVkPipelineLayout, 0, 1, descSets, 0, nullptr);
	}
	
	//u32 vtxCount = TestVertex::s_kVtxCount;
	u32 idxCount = TestVertex::s_kIdxCount;
	vkCmdDrawIndexed(vkCmdBuf, idxCount, 1, 0, 0, 0);

	{
		vkCmdEndRenderPass(vkCmdBuf);
	}
}

void
RenderContext_Vk::createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const math::Rect2f& rect2)
{
	RDS_CORE_ASSERT(!info.formats.is_empty() || !info.presentModes.is_empty(), "info is not yet init");
	{
		for (const auto& availableFormat : info.formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				out.surfaceFormat = availableFormat;
			}
		}
	}

	{
		for (const auto& availablePresentMode : info.presentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			{
				out.presentMode = availablePresentMode;
			}
		}
	}

	{
		if (info.capabilities.currentExtent.width != math::inf<u32>())
		{
			out.extent = info.capabilities.currentExtent;
		}
		else
		{
			const auto& caps = info.capabilities;
			auto& extent = out.extent;
			extent = Util::toVkExtent2D(rect2);
			extent.width = math::clamp(extent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
			extent.height = math::clamp(extent.height, caps.minImageExtent.height, caps.maxImageExtent.height);
		}
	}
}

void
RenderContext_Vk::createSwapchain(const SwapchainInfo_Vk& swapchainInfo)
{
	RDS_PROFILE_SCOPED();

	destroySwapchain();

	auto vkDevice = renderer()->vkDevice();

	if (_framebufferSize.x != 0 && _framebufferSize.y != 0)
	{
		_swapchainInfo.extent = Util::toVkExtent2D(_framebufferSize);
	}

	Util::createSwapchain(_vkSwapchain.ptrForInit(), _vkSurface, vkDevice, swapchainInfo, _renderer->swapchainAvailableInfo(), _renderer->queueFamilyIndices());
	Util::createSwapchainImages(_vkSwapchainImages, _vkSwapchain, vkDevice);
	Util::createSwapchainImageViews(_vkSwapchainImageViews, _vkSwapchainImages, vkDevice, swapchainInfo.surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	createSwapchainFramebuffers();
}

void
RenderContext_Vk::destroySwapchain()
{
	if (!_vkSwapchain)
		return;

	vkDeviceWaitIdle(renderer()->vkDevice());

	_vkSwapchainFramebuffers.clear();
	_vkSwapchainImageViews.clear();
	_vkSwapchainImages.clear();
	_vkSwapchain.reset(nullptr);
}

void
RenderContext_Vk::createSwapchainFramebuffers()
{
	auto* vkDevice = _renderer->vkDevice();
	auto* vkAlloCallbacks = _renderer->allocCallbacks();

	auto count = _vkSwapchainImageViews.size();

	_vkSwapchainFramebuffers.resize(count);
	for (size_t i = 0; i < count; i++)
	{
		VkImageView attachments[] =
		{
			_vkSwapchainImageViews[i],
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _testVkRenderPass;
		framebufferInfo.attachmentCount = ArraySize<decltype(attachments)>;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _swapchainInfo.extent.width;
		framebufferInfo.height = _swapchainInfo.extent.height;
		framebufferInfo.layers = 1;

		auto ret = vkCreateFramebuffer(vkDevice, &framebufferInfo, vkAlloCallbacks, _vkSwapchainFramebuffers[i].ptrForInit());
		Util::throwIfError(ret);
	}
}

void
RenderContext_Vk::createCommandPool(Vk_CommandPool_T** outVkCmdPool, u32 queueIdx)
{
	Util::createCommandPool(outVkCmdPool, queueIdx, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

//void
//RenderContext_Vk::createCommandBuffer(Vk_CommandPool* vkCmdPool)
//{
//	_vkCommandBuffers.resize(s_kFrameInFlightCount);
//	for (auto& e : _vkCommandBuffers)
//	{
//		e.create(vkCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
//	}
//}

void
RenderContext_Vk::createSyncObjects()
{
	auto* vkDevice = renderer()->vkDevice();

	_imageAvailableVkSmps.resize(s_kFrameInFlightCount);
	_renderCompletedVkSmps.resize(s_kFrameInFlightCount);
	_inFlightVkFences.resize(s_kFrameInFlightCount);

	Vector<Vk_Semaphore*, s_kFrameInFlightCount> tmpVkSmps0;	tmpVkSmps0.resize(s_kFrameInFlightCount);
	Vector<Vk_Semaphore*, s_kFrameInFlightCount> tmpVkSmps1;	tmpVkSmps1.resize(s_kFrameInFlightCount);
	Vector<Vk_Fence*, s_kFrameInFlightCount> tmpVkFences;	tmpVkFences.resize(s_kFrameInFlightCount);

	for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		Util::createSemaphore(&tmpVkSmps0[i], vkDevice);
		Util::createSemaphore(&tmpVkSmps1[i], vkDevice);
		Util::createFence(&tmpVkFences[i], vkDevice);
	}

	Util::convertToVkPtrs(_imageAvailableVkSmps, tmpVkSmps0);
	Util::convertToVkPtrs(_renderCompletedVkSmps, tmpVkSmps1);
	Util::convertToVkPtrs(_inFlightVkFences, tmpVkFences);
}

void
RenderContext_Vk::createTestRenderPass()
{
	auto* vkDevice = _renderer->vkDevice();
	const auto* vkAllocCallbacks = _renderer->allocCallbacks();

	VkAttachmentDescription	colorAttachment = {};
	colorAttachment.format = _swapchainInfo.surfaceFormat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	//depthAttachment.format			= findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;	// index
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;	// index
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	//subpass.pDepthStencilAttachment = &depthAttachmentRef;

	Vector<VkAttachmentDescription, 2> attachments;
	attachments.reserve(2);
	attachments.emplace_back(colorAttachment);
	//attachments.emplace_back(depthAttachment);

	// for image layout transition
	Vector<VkSubpassDependency, 12> subpassDeps;
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDep.dstSubpass = 0;
		subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDep.srcAccessMask = 0;
		subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT /*| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT*/;
		subpassDeps.emplace_back(subpassDep);
	}
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass = 0;
		subpassDep.dstSubpass = VK_SUBPASS_EXTERNAL;
		subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDep.dstAccessMask = 0 /*| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT*/;
		subpassDeps.emplace_back(subpassDep);
	}

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = sCast<u32>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.dependencyCount = sCast<u32>(subpassDeps.size());
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.pDependencies = subpassDeps.data();

	auto ret = vkCreateRenderPass(vkDevice, &renderPassInfo, vkAllocCallbacks, _testVkRenderPass.ptrForInit());
	Util::throwIfError(ret);
}

void
RenderContext_Vk::createTestGraphicsPipeline()
{
	RDS_PROFILE_SCOPED();

	auto* vkDevice = _renderer->vkDevice();
	const auto* vkAllocCallbacks = _renderer->allocCallbacks();

	// shader module and stage
	VkPipelineShaderStageCreateInfo	vsStageInfo{};
	VkPipelineShaderStageCreateInfo	psStageInfo{};

	VkPtr<Vk_ShaderModule> vsModule;
	VkPtr<Vk_ShaderModule> psModule;

	{
		Util::createShaderModule(vsModule.ptrForInit(), "asset/shader/vulkan/local_temp/bin/hello_triangle2.hlsl.vert.spv", vkDevice);
		Util::createShaderModule(psModule.ptrForInit(), "asset/shader/vulkan/local_temp/bin/hello_triangle2.hlsl.frag.spv", vkDevice);

		vsStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vsStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vsStageInfo.module = vsModule;
		vsStageInfo.pName = "vs_main";
		vsStageInfo.pSpecializationInfo = nullptr;

		psStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		psStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		psStageInfo.module = psModule;
		psStageInfo.pName = "ps_main";
		psStageInfo.pSpecializationInfo = nullptr;

	}
	VkPipelineShaderStageCreateInfo shaderStages[] = { vsStageInfo, psStageInfo };

	// vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto vtxBindingDescription = TestVertex::getBindingDescription();
	auto vtxAttrDescriptions = TestVertex::getAttributeDescriptions();
	{
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = sCast<u32>(vtxAttrDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &vtxBindingDescription;		// Optional
		vertexInputInfo.pVertexAttributeDescriptions = vtxAttrDescriptions.data();	// Optional
	}

	// input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	{
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	Vector<VkDynamicState, 2> dynamicStates;
	{
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = sCast<u32>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();
	}

	// viewport stage
	VkPipelineViewportStateCreateInfo viewportState = {};
	{
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = sCast<float>(_swapchainInfo.extent.width);
		viewport.height = sCast<float>(_swapchainInfo.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = _swapchainInfo.extent;

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.pScissors = &scissor;
	}

	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	{
		rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable			= VK_FALSE; // true when no want to discard out bound pixel, useful for shawdow map
		rasterizer.rasterizerDiscardEnable	= VK_FALSE;	// true then no gemotry pass through the rasterizer stage
		rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth				= 1.0f;		// check gpu features
		rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace				= VK_FRONT_FACE_CLOCKWISE;	// VK_FRONT_FACE_CLOCKWISE VK_FRONT_FACE_COUNTER_CLOCKWISE
		rasterizer.depthBiasEnable			= VK_FALSE;
		rasterizer.depthBiasConstantFactor	= 0.0f; // Optional
		rasterizer.depthBiasClamp			= 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor		= 0.0f; // Optional
	}

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	{
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;		// Optional
		multisampling.pSampleMask = nullptr;	// Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional
	}

	// depth and stencil
	//VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
	{

	}

	// color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};	// for per attached framebuffer
	VkPipelineColorBlendStateCreateInfo colorBlending = {};			// is global color blending setting (for all framebuffer)
	{
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;				// Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;		// Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;							// Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;						// Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;					// Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;							// Optional

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	{
		Vk_DescriptorSetLayout_T* setLayouts[] = { _testVkDescriptorSetLayout.hnd()};

		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= ArraySize<decltype(setLayouts)>;	// Optional
		pipelineLayoutInfo.pSetLayouts				= setLayouts;						// Optional
		pipelineLayoutInfo.pushConstantRangeCount	= 0;		// Optional
		pipelineLayoutInfo.pPushConstantRanges		= nullptr;	// Optional
		auto ret = vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, vkAllocCallbacks, _testVkPipelineLayout.ptrForInit());
		Util::throwIfError(ret);
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;			// Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = _testVkPipelineLayout;
	pipelineInfo.renderPass = _testVkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineInfo.basePipelineIndex = -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	auto ret = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineInfo, vkAllocCallbacks, _testVkPipeline.ptrForInit());
	Util::throwIfError(ret);
}

void
RenderContext_Vk::createTestVertexBuffer()
{
	//auto* vkAllocCallbacks	= renderer()->allocCallbacks();
	//auto* vkDev		= renderer()->vkDevice();
	auto* allocVk	= renderer()->memoryContext()->allocVk();

	auto vtxData = TestVertex::make2();
	auto bufSize = vtxData.size();
	
	Vk_Buffer			_vkStagingBuf;

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::CpuOnly;
		allocInfo.flags |= RenderAllocFlags::HostWrite;
		Util::createBuffer(_vkStagingBuf, allocVk, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		u8* mappedData = nullptr;
		Vk_ScopedMemMap mm = { &mappedData, &_vkStagingBuf };
		memory_copy(reinCast<u8*>(mappedData), reinCast<u8*>(vtxData.data()), bufSize);
	}

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::GpuOnly;
		Util::createBuffer(_testVkVtxBuffer, allocVk, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Util::copyBuffer(&_testVkVtxBuffer, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), _vkTransferQueue);
	}

}

void
RenderContext_Vk::createTestIndexBuffer()
{
	//auto* vkAllocCallbacks	= renderer()->allocCallbacks();
	//auto* vkDev = renderer()->vkDevice();
	auto* allocVk	= renderer()->memoryContext()->allocVk();

	auto indices = TestVertex::makeIndices();
	auto bufSize = sizeof(indices[0]) * indices.size();

	Vk_Buffer			_vkStagingBuf;

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::CpuOnly;
		allocInfo.flags |= RenderAllocFlags::HostWrite;
		Util::createBuffer(_vkStagingBuf, allocVk, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		u8* mappedData = nullptr;
		Vk_ScopedMemMap mm = { &mappedData, &_vkStagingBuf };
		memory_copy(reinCast<u8*>(mappedData), reinCast<u8*>(indices.data()), bufSize);
	}

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::GpuOnly;
		Util::createBuffer(_testVkIdxBuffer, allocVk, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Util::copyBuffer(&_testVkIdxBuffer, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), _vkTransferQueue);
	}

	Util::copyBuffer(&_testVkIdxBuffer, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), _vkTransferQueue);
}

void 
RenderContext_Vk::createTestDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding			= 0;
	uboLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount	= 1;
	uboLayoutBinding.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings	= &uboLayoutBinding;

	_testVkDescriptorSetLayout.create(&layoutInfo);
}

void 
RenderContext_Vk::createTestUniformBuffer()
{
	auto* allocVk			= renderer()->memoryContext()->allocVk();
	VkDeviceSize bufferSize = sizeof(TestUBO);

	Vk_AllocInfo allocInfo;
	allocInfo.usage  = RenderMemoryUsage::CpuToGpu;

	_testVkUniformBuffers.resize(s_kFrameInFlightCount);
	_memMapUniformBufs.resize(s_kFrameInFlightCount);
	for (size_t i = 0; i < s_kFrameInFlightCount; ++i)
	{
		auto& uniBuf = _testVkUniformBuffers[i];
		auto& memMap = _memMapUniformBufs[i];

		uniBuf.create(allocVk, &allocInfo, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, QueueTypeFlags::Graphics);
		allocVk->mapMem(&memMap, uniBuf._internal_allocHnd());
	}
}

void 
RenderContext_Vk::createTestDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount	= sCast<u32>(s_kFrameInFlightCount);

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount	= 1;
	poolInfo.pPoolSizes		= &poolSize;
	poolInfo.maxSets		= sCast<u32>(s_kFrameInFlightCount);
	//poolInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	_testVkDescriptorPool.create(&poolInfo);
}

void 
RenderContext_Vk::createTestDescriptorSets()
{
	auto* vkDev = renderer()->vkDevice();

	/*Vector<Vk_DescriptorSetLayout_T*, s_kFrameInFlightCount> layouts;
	layouts.resize(s_kFrameInFlightCount);
	for (auto& e : layouts)
	{
		e = _testVkDescriptorSetLayout.hnd();
	}*/

	Vk_DescriptorSetLayout_T* layouts[] = { _testVkDescriptorSetLayout.hnd() };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool		= _testVkDescriptorPool.hnd();
	allocInfo.descriptorSetCount	= 1;
	allocInfo.pSetLayouts			= layouts;
	
	_testVkDescriptorSets.resize(s_kFrameInFlightCount);
	for (auto& e : _testVkDescriptorSets)
	{
		e.create(&allocInfo);
	}

	for (size_t i = 0; i < s_kFrameInFlightCount; i++) 
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer	= _testVkUniformBuffers[i].hnd();
		bufferInfo.offset	= 0;
		bufferInfo.range	= sizeof(TestUBO);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet				= _testVkDescriptorSets[i].hnd();
		descriptorWrite.dstBinding			= 0;
		descriptorWrite.dstArrayElement		= 0;
		descriptorWrite.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount		= 1;
		descriptorWrite.pBufferInfo			= &bufferInfo;
		descriptorWrite.pImageInfo			= nullptr; // Optional
		descriptorWrite.pTexelBufferView	= nullptr; // Optional

		vkUpdateDescriptorSets(vkDev, 1, &descriptorWrite, 0, nullptr);
	}
}

void 
RenderContext_Vk::updateTestUBO(u32 curImageIdx)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	TestUBO ubo = {};
	ubo.model		= Mat4f::s_rotateZ(math::radians(90.0f) * time);
	ubo.view		= Mat4f::s_lookAt(Vec3f{ 2.0f, 2.0f, 2.0f }, Vec3f::s_zero(), Vec3f{ 0.0f, 0.0f, 1.0f });
	ubo.proj		= Mat4f::s_perspective(math::radians(45.0f), _framebufferSize.x / _framebufferSize.y, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	ubo.mvp			= ubo.proj * ubo.view * ubo.model;

	memory_copy(_memMapUniformBufs[curImageIdx], reinCast<u8*>(&ubo), sizeof(ubo));
}


#endif
}

#endif // RDS_RENDER_HAS_VULKAN