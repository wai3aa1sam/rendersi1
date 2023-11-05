#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderer_Vk.h"

#include "rds_render_api_layer/command/rdsTransferRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/shader/rdsMaterial_Vk.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

#if RDS_RENDER_HAS_VULKAN

#define RDS_TEST_DRAW_CALL 0
#define RDS_TEST_MT_DRAW_CALLS 0

namespace rds
{

SPtr<RenderContext> Renderer_Vk::onCreateContext(const RenderContext_CreateDesc& cDesc)
{
	auto p = SPtr<RenderContext>(makeSPtr<RenderContext_Vk>());
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
RenderContext_Vk::waitIdle()
{
	vkDeviceWaitIdle(renderer()->vkDevice());
}

void
RenderContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	_renderer = Renderer_Vk::instance();
	auto vkDevice = _renderer->vkDevice();

	_renderFrames.resize(s_kFrameInFlightCount);
	for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		_renderFrames[i].create();
	}

	_vkGraphicsQueue.create(_renderer->queueFamilyIndices().graphics.value(), vkDevice);
	 _vkPresentQueue.create(_renderer->queueFamilyIndices().present.value(),  vkDevice);
	_vkTransferQueue.create(_renderer->queueFamilyIndices().transfer.value(), vkDevice);
	
	createTestUniformBuffer();
	updateTestUBO(_vkSwapchain.curImageIdx());

	createTestTextureImage();
	createTestTextureImageView();
	createTestTextureSampler();

	createTestDescriptorSetLayout();
	createTestDescriptorPool();

	auto vkSwapchainCDesc = _vkSwapchain.makeCDesc();
	vkSwapchainCDesc.rdCtx				= this;
	vkSwapchainCDesc.wnd				= cDesc.window;
	vkSwapchainCDesc.framebufferRect2f	= math::toRect2_wh(framebufferSize());
	//vkSwapchainCDesc.vkRdPass			= &_testVkRenderPass;
	vkSwapchainCDesc.colorFormat		= VK_FORMAT_B8G8R8A8_SRGB;
	vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	vkSwapchainCDesc.depthFormat		= VK_FORMAT_D32_SFLOAT_S8_UINT;

	createTestRenderPass(vkSwapchainCDesc);
	_vkSwapchain.create(vkSwapchainCDesc);

	createTestGraphicsPipeline();
	createTestDescriptorSets();

	createTestVertexBuffer(&_testVkVtxBuffer);
	createTestVertexBuffer(&_testVkVtxBuffer2, -0.5f);
	createTestIndexBuffer();
	
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
	waitIdle();

	RDS_TODO("remove");
	_testVkDescriptorPool.destroy(renderer());
	_testVkTextureImageView.destroy(renderer());
	_testVkTextureSampler.destroy(renderer());
}

void
RenderContext_Vk::onBeginRender()
{
	RDS_PROFILE_SCOPED();

	auto* vkDevice = renderer()->vkDevice();
	VkResult ret;

	//const auto* inFlightVkFence = reinCast<const VkFence*>(_inFlightVkFence.address());
	Vk_Fence_T* vkFences[] = { renderFrame().inFlightFence()->hnd()};
	u32 vkFenceCount = ArraySize<decltype(vkFences)>;
	{
		RDS_PROFILE_SECTION("vkWaitForFences()");
		vkWaitForFences(vkDevice, vkFenceCount, vkFences, VK_TRUE, NumLimit<u64>::max());
	}
	//vkResetFences(vkDevice, vkFenceCount, vkFences);		// reset too early will cause deadlock, since the invalidate wii cause no work submitted (returned) and then no one will signal it

	ret = _vkSwapchain.acquireNextImage(renderFrame().imageAvaliableSmp());

	if (!Util::isSuccess(ret))
	{
		invalidateSwapchain(ret, _vkSwapchain.framebufferSize());
		return;
	}

	{
		renderFrame().reset();

		_curGraphicsCmdBuf	= renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		#if RDS_TEST_DRAW_CALL
		auto* vkCmdBuf		= _curGraphicsCmdBuf->hnd();
		beginRecord(vkCmdBuf, _curImageIdx);
		RDS_PROFILE_GPU_ZONE_VK(_gpuProfilerCtx, vkCmdBuf, "Test Draw Call");
		beginRenderPass(vkCmdBuf, _curImageIdx);
		testDrawCall(vkCmdBuf, _curImageIdx, &_testVkVtxBuffer);
		testDrawCall(vkCmdBuf, _curImageIdx, &_testVkVtxBuffer2);
		endRenderPass(vkCmdBuf, _curImageIdx);
		#endif // RDS_TEST_DRAW_CALL
	}
}

void
RenderContext_Vk::onEndRender()
{
	RDS_PROFILE_SCOPED();

	#if RDS_TEST_DRAW_CALL
	endRecord(_curGraphicsCmdBuf->hnd(), _curImageIdx);
	_shdSwapBuffers = true;
	#endif // RDS_TEST_DRAW_CALL


	if (_shdSwapBuffers)
	{
		RDS_TODO("handle do not get next image idx if no swap buffers in the next frame");
		auto ret = _vkSwapchain.swapBuffers(&_vkPresentQueue, _curGraphicsCmdBuf, renderFrame().renderCompletedSmp());
		invalidateSwapchain(ret, _vkSwapchain.framebufferSize());
		_shdSwapBuffers = false;
	}

	// next frame idx
	_curFrameIdx = math::modPow2Val(_curFrameIdx + 1, s_kFrameInFlightCount);
}

void
RenderContext_Vk::onSetFramebufferSize(const Vec2f& newSize)
{
	bool isInvalidSize = math::equals0(newSize.x) || math::equals0(newSize.y);
	if (isInvalidSize)
		return;

	invalidateSwapchain(VK_ERROR_OUT_OF_DATE_KHR, newSize);
}

void 
RenderContext_Vk::onCommit(RenderCommandBuffer& renderCmdBuf)
{
	Base::onCommit(renderCmdBuf);

	if (!_vkSwapchain.isValid())
	{
		return;
	}

	// wait job sysytem handle
	#if !RDS_TEST_DRAW_CALL

	beginRecord(_curGraphicsCmdBuf->hnd(), _vkSwapchain.curImageIdx());

	// begin render pass
	{
		Vector<VkClearValue, 4> clearValues;
		auto* p = renderCmdBuf.getClearFramebuffersCmd();
		auto attachmentCount = 2; // TODO: revise

		RDS_WARN_ONCE("TODO: RenderCommand_ClearFrambuffers, shd handele properly");
		for (size_t i = 0; i < attachmentCount; i++)
		{
			auto& e = clearValues.emplace_back();
			bool isColorAttachment = i == 0;	// TODO: revise

			if (isColorAttachment && p->clearColor.has_value())
			{
				e = Util::toVkClearValue(p->clearColor.value());
			}

			if (!isColorAttachment && p->clearDepthStencil.has_value())
			{
				const auto& v = p->clearDepthStencil.value();
				e = Util::toVkClearValue(v.first, v.second);
			}
		}

		auto fbufRect2 = _vkSwapchain.framebufferRect2f();

		// VK_SUBPASS_CONTENTS_INLINE, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
		
		#if RDS_TEST_MT_DRAW_CALLS
		_curGraphicsCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		#else
		_curGraphicsCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_INLINE);
		_curGraphicsCmdBuf->setViewport(fbufRect2);
		_curGraphicsCmdBuf->setScissor (fbufRect2);
		#endif // TE
	}

	//_curGraphicsCmdBuf->setViewport(Util::toRect2f(_swapchainInfo.extent));
	// _curGraphicsCmdBuf->setScissor(Util::toRect2f(_swapchainInfo.extent));

	for (auto* cmd : renderCmdBuf.commands())
	{
		RDS_WARN_ONCE("TODO: if (cmd->type() == RenderCommandType::DrawCall)");
		#if RDS_TEST_MT_DRAW_CALLS
		if (cmd->type() == RenderCommandType::DrawCall)
			continue;
		#endif // RDS_TEST_MT_DRAW_CALLS
		_dispatchCommand(this, cmd);
	}

	//test_extraDrawCall(renderCmdBuf);

	// end render pass
	_curGraphicsCmdBuf->endRenderPass();

	endRecord(_curGraphicsCmdBuf->hnd(), _vkSwapchain.curImageIdx());

	#endif // !RDS_TEST_DRAW_CALL
}

void 
RenderContext_Vk::test_extraDrawCall(RenderCommandBuffer& renderCmdBuf)
{
	RDS_WARN_ONCE("TODO: renderCmdBuf.drawCallCmds() for a extra Sec.cmd buf to execute but seems has order problem or maybe copy unity to have a executeCmdBuf()");
	{
		auto* vkCmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY);

		vkCmdBuf->beginSecondaryRecord(vkGraphicsQueue(), &_testVkRenderPass, _vkSwapchain.framebuffer(), 0);

		auto rect2 = _vkSwapchain.framebufferRect2f();
		vkCmdBuf->setViewport(rect2);
		vkCmdBuf->setScissor(rect2);

		for (auto* cmd : renderCmdBuf.commands())
		{
			if (cmd->type() != RenderCommandType::DrawCall)
				continue;
			_onRenderCommand_DrawCall(vkCmdBuf, sCast<RenderCommand_DrawCall*>(cmd));
		}

		vkCmdBuf->endRecord();

		auto* primaryCmdBuf = _curGraphicsCmdBuf;
		primaryCmdBuf->executeSecondaryCmdBufs(Span<Vk_CommandBuffer*>{ &vkCmdBuf, 1});
	}
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
	auto* vkAlloc	= Renderer_Vk::instance()->memoryContext()->vkAlloc();

	auto totalSize					= rdfUploadBuf.totalDataSize();
	const auto* inlineUploadBuffer	= rdfUploadBuf.getInlineUploadBuffer();

	if (totalSize == 0)
	{
		return;
	}

	Vk_Buffer vkStageBuf;
	RDS_TODO("use exclusive queue instead of concurrent");
	Vk_AllocInfo allocInfo = {};
	allocInfo.usage  = RenderMemoryUsage::CpuOnly;
	allocInfo.flags |= RenderAllocFlags::HostWrite;
	Util::createBuffer(vkStageBuf, vkAlloc, &allocInfo, totalSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

	{
		const auto& chunks	= inlineUploadBuffer->bufData.chunks();
		auto chunkCount		= chunks.size();

		Vk_ScopedMemMapBuf mm = { &vkStageBuf };
		u8* mappedData = mm.data<u8*>();

		RDS_CORE_ASSERT(chunkCount == 1, "not yet handle > 1");
		for (size_t i = 0; i < chunkCount; i++)
		{
			auto& e = chunks[i];
			memory_copy(mappedData, e->data(), e->size());
			mappedData += e->size();
		}
	}

	auto* transferCmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	transferCmdBuf->beginRecord(&_vkTransferQueue, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	auto CmdCount = inlineUploadBuffer->uploadBufCmds.size();
	for (size_t iCmd = 0; iCmd < CmdCount; iCmd++)
	{
		auto& cmd = inlineUploadBuffer->uploadBufCmds[iCmd];
		auto* dstBuf = sCast<RenderGpuBuffer_Vk*>(cmd->dst.ptr());
		transferCmdBuf->cmd_CopyBuffer(dstBuf->vkBuf(), &vkStageBuf, cmd->data.size(), 0, inlineUploadBuffer->bufOffsets[iCmd]);		// dst offset already handled in RenderGpuBuffer::onUpload()
		RDS_ASSERT(!BitUtil::has(cmd->queueTypeflags, QueueTypeFlags::Compute), "not yet support");
	}

	RDS_TODO("add smp for transfer and graphics queue");

	transferCmdBuf->endRecord();
	transferCmdBuf->submit();

	RDS_TODO("remove transferCmdBuf->waitIdle()");
	/*
	example:
	https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#swapchain-image-acquire-and-present
	Command Buffer Recording and Submission for a unified transfer/graphics queue:
	*/
	transferCmdBuf->waitIdle();

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

	updateTestUBO(imageIdx);
	_curGraphicsCmdBuf->beginRecord(vkGraphicsQueue(), VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	RDS_PROFILE_GPU_ZONET_VK(_gpuProfilerCtx, _curGraphicsCmdBuf->hnd(), "RenderContext_Vk::onCommit");
}

void
RenderContext_Vk::endRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();

	VkResult ret;
	auto* vkDevice = renderer()->vkDevice();

	RDS_PROFILE_GPU_COLLECT_VK(_gpuProfilerCtx, vkCmdBuf);

	_curGraphicsCmdBuf->endRecord();

	{
		Vk_Fence_T* vkFences[]		= { renderFrame().inFlightFence()->hnd()};
		u32			vkFenceCount	= ArraySize<decltype(vkFences)>;
		ret = vkResetFences(vkDevice, vkFenceCount, vkFences);		// should handle it to signaled if the function throw?
		Util::throwIfError(ret);
	}

	_curGraphicsCmdBuf->submit(renderFrame().inFlightFence()
							 , renderFrame().imageAvaliableSmp(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR
							 , renderFrame().renderCompletedSmp(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
}

void
RenderContext_Vk::bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline)
{
	RDS_PROFILE_SCOPED();

	vkCmdBindPipeline(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->hnd());

	// since we set viewport and scissor state is dynamic
	const auto& rect2f = _vkSwapchain.framebufferRect2f();
	_curGraphicsCmdBuf->setViewport(rect2f);
	 _curGraphicsCmdBuf->setScissor(rect2f);
}

void 
RenderContext_Vk::beginRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();
	Vector<VkClearValue, 4> clearValues;
	clearValues.reserve(4);
	clearValues.emplace_back().color		= { 0.8f, 0.6f, 0.4f, 1.0f };
	clearValues.emplace_back().depthStencil = { 1.0f, 0 };

	const auto& rect2f = _vkSwapchain.framebufferRect2f();
	_curGraphicsCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), rect2f, clearValues.span(), VK_SUBPASS_CONTENTS_INLINE);
}

void 
RenderContext_Vk::endRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	_curGraphicsCmdBuf->endRenderPass();
}

void
RenderContext_Vk::testDrawCall(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx, Vk_Buffer* vtxBuf)
{
	RDS_PROFILE_SCOPED();

	bindPipeline(vkCmdBuf, &_testVkPipeline);

	{
		Vk_Buffer_T* vertexBuffers[]	= { vtxBuf->hnd() };
		VkDeviceSize offsets[]			= { 0 };
		vkCmdBindVertexBuffers(vkCmdBuf, 0, 1, vertexBuffers, offsets);
	}

	{
		vkCmdBindIndexBuffer(vkCmdBuf, _testVkIdxBuffer.hnd(), 0, VK_INDEX_TYPE_UINT16);
	}

	{
		Vk_DescriptorSet_T* descSets[] = { _testVkDescriptorSets[imageIdx].hnd() };
		vkCmdBindDescriptorSets(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _testVkPipelineLayout.hnd(), 0, 1, descSets, 0, nullptr);
	}

	//u32 vtxCount = TestVertex::s_kVtxCount;
	u32 idxCount = TestVertex::s_kIdxCount;
	vkCmdDrawIndexed(vkCmdBuf, idxCount, 1, 0, 0, 0);
}

void 
RenderContext_Vk::invalidateSwapchain(VkResult ret, const Vec2f& newSize)
{
	// VK_ERROR_OUT_OF_DATE_KHR		means window size is changed
	// VK_SUBOPTIMAL_KHR			means sucessfully presented but the surface properties are not matched

	if (Util::isSuccess(ret) || ret == VkResult::VK_NOT_READY)
	{
		return;
	}

	if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR)
	{
		auto vkSwapchainCDesc = _vkSwapchain.makeCDesc();
		vkSwapchainCDesc.rdCtx				= this;
		vkSwapchainCDesc.wnd				= nativeUIWindow();
		vkSwapchainCDesc.framebufferRect2f	= math::toRect2_wh(newSize);
		vkSwapchainCDesc.vkRdPass			= &_testVkRenderPass;
		vkSwapchainCDesc.colorFormat		= VK_FORMAT_B8G8R8A8_SRGB;
		vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		vkSwapchainCDesc.depthFormat		= VK_FORMAT_D32_SFLOAT_S8_UINT;

		_vkSwapchain.create(vkSwapchainCDesc);

		return; // should not record if size is changed
	}
	else
	{
		throwIf(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR, "");	// VK_SUBOPTIMAL_KHR 
	}
}


#if 0
#pragma mark --- rdsRenderContext_Vk-createResource
#endif // 0
#if 1

void
RenderContext_Vk::createCommandPool(Vk_CommandPool_T** outVkCmdPool, u32 queueIdx)
{
	Util::createCommandPool(outVkCmdPool, queueIdx, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void
RenderContext_Vk::createTestRenderPass(Vk_Swapchain_CreateDesc& vkSwapchainCDesc)
{
	//auto* vkDevice = _renderer->vkDevice();
	//const auto* vkAllocCallbacks = _renderer->allocCallbacks();

	VkAttachmentDescription	colorAttachment = {};
	colorAttachment.format			= vkSwapchainCDesc.colorFormat; //_vkSwapchain.colorFormat();
	colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format			= vkSwapchainCDesc.depthFormat; // _vkSwapchain.depthFormat();
	depthAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment	= 0;	// index
	colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment	= 1;	// index
	depthAttachmentRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount	= 0;
	subpass.colorAttachmentCount	= 1;
	subpass.pInputAttachments		= nullptr;
	subpass.pColorAttachments		= &colorAttachmentRef;
	subpass.pDepthStencilAttachment	= &depthAttachmentRef;

	Vector<VkAttachmentDescription, 4> attachments;
	attachments.reserve(4);
	attachments.emplace_back(colorAttachment);
	attachments.emplace_back(depthAttachment);

	// for image layout transition
	Vector<VkSubpassDependency, 12> subpassDeps;
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass		= VK_SUBPASS_EXTERNAL;
		subpassDep.dstSubpass		= 0;
		subpassDep.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.srcAccessMask	= VK_ACCESS_NONE_KHR;
		subpassDep.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDeps.emplace_back(subpassDep);
	}
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass		= 0;
		subpassDep.dstSubpass		= VK_SUBPASS_EXTERNAL;
		subpassDep.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.srcAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDep.dstAccessMask	= VK_ACCESS_NONE_KHR;
		subpassDeps.emplace_back(subpassDep);
	}

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount	= sCast<u32>(attachments.size());
	renderPassInfo.subpassCount		= 1;
	renderPassInfo.dependencyCount	= sCast<u32>(subpassDeps.size());
	renderPassInfo.pAttachments		= attachments.data();
	renderPassInfo.pSubpasses		= &subpass;
	renderPassInfo.pDependencies	= subpassDeps.data();

	_testVkRenderPass.create(&renderPassInfo);
	vkSwapchainCDesc.vkRdPass = &_testVkRenderPass;
}

void
RenderContext_Vk::createTestGraphicsPipeline()
{
	RDS_PROFILE_SCOPED();

	//auto* vkDevice					= _renderer->vkDevice();
	//const auto* vkAllocCallbacks	= _renderer->allocCallbacks();

	// shader module and stage
	VkPipelineShaderStageCreateInfo	vsStageInfo = {};
	VkPipelineShaderStageCreateInfo	psStageInfo = {};

	Vk_ShaderModule vsModule;
	Vk_ShaderModule psModule;

	{
		vsModule.create(renderer(), "LocalTemp/imported/shader/asset/shader/test_texture_set0.shader/spirv/pass0/vs_1.1.bin");
		psModule.create(renderer(), "LocalTemp/imported/shader/asset/shader/test_texture_set0.shader/spirv/pass0/ps_1.1.bin");

		vsStageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vsStageInfo.stage					= VK_SHADER_STAGE_VERTEX_BIT;
		vsStageInfo.module					= vsModule.hnd();
		vsStageInfo.pName					= "vs_main";
		vsStageInfo.pSpecializationInfo		= nullptr;

		psStageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		psStageInfo.stage					= VK_SHADER_STAGE_FRAGMENT_BIT;
		psStageInfo.module					= psModule.hnd();
		psStageInfo.pName					= "ps_main";
		psStageInfo.pSpecializationInfo		= nullptr;

	}
	VkPipelineShaderStageCreateInfo shaderStages[] = { vsStageInfo, psStageInfo };

	// vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto vtxBindingDescription	= TestVertex::getBindingDescription();
	auto vtxAttrDescriptions	= TestVertex::getAttributeDescriptions();
	{
		vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount	= 1;
		vertexInputInfo.vertexAttributeDescriptionCount	= sCast<u32>(vtxAttrDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions		= &vtxBindingDescription;		// Optional
		vertexInputInfo.pVertexAttributeDescriptions	= vtxAttrDescriptions.data();	// Optional
	}

	// input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	{
		inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable	= VK_FALSE;
	}

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	Vector<VkDynamicState, 2> dynamicStates;
	{
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

		dynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount	= sCast<u32>(dynamicStates.size());
		dynamicState.pDynamicStates		= dynamicStates.data();
	}

	// viewport stage
	VkPipelineViewportStateCreateInfo viewportState = {};
	{
		VkViewport viewport = {};
		viewport.x			= 0.0f;
		viewport.y			= 0.0f;
		viewport.width		= _vkSwapchain.framebufferRect2f().w;
		viewport.height		= _vkSwapchain.framebufferRect2f().h;
		viewport.minDepth	= 0.0f;
		viewport.maxDepth	= 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = _vkSwapchain.framebufferVkExtent2D();

		viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount	= 1;
		viewportState.scissorCount	= 1;
		viewportState.pViewports	= &viewport;
		viewportState.pScissors		= &scissor;
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
		multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable	= VK_FALSE;
		multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading		= 1.0f;		// Optional
		multisampling.pSampleMask			= nullptr;	// Optional
		multisampling.alphaToCoverageEnable	= VK_FALSE; // Optional
		multisampling.alphaToOneEnable		= VK_FALSE; // Optional
	}

	// depth and stencil
	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	{
		depthStencilCreateInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable			= VK_TRUE;
		depthStencilCreateInfo.depthWriteEnable			= VK_TRUE;
		depthStencilCreateInfo.depthCompareOp			= VK_COMPARE_OP_LESS;
		depthStencilCreateInfo.depthBoundsTestEnable	= VK_FALSE;
		depthStencilCreateInfo.minDepthBounds			= 0.0f; // Optional
		depthStencilCreateInfo.maxDepthBounds			= 1.0f; // Optional
		depthStencilCreateInfo.stencilTestEnable		= VK_FALSE;
		depthStencilCreateInfo.front					= {}; // Optional
		depthStencilCreateInfo.back						= {}; // Optional
	}

	// color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};	// for per attached framebuffer
	VkPipelineColorBlendStateCreateInfo colorBlending = {};			// is global color blending setting (for all framebuffer)
	{
		colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable			= VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;				// Optional
		colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;		// Optional
		colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD;							// Optional
		colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;						// Optional
		colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;					// Optional
		colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD;							// Optional

		colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable		= VK_FALSE;
		colorBlending.logicOp			= VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount	= 1;
		colorBlending.pAttachments		= &colorBlendAttachment;
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
		_testVkPipelineLayout.create(renderer(), &pipelineLayoutInfo);
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount				= 2;
	pipelineInfo.pStages				= shaderStages;
	pipelineInfo.pVertexInputState		= &vertexInputInfo;
	pipelineInfo.pInputAssemblyState	= &inputAssembly;
	pipelineInfo.pViewportState			= &viewportState;
	pipelineInfo.pRasterizationState	= &rasterizer;
	pipelineInfo.pMultisampleState		= &multisampling;
	pipelineInfo.pDepthStencilState		= &depthStencilCreateInfo;			// Optional
	pipelineInfo.pColorBlendState		= &colorBlending;
	pipelineInfo.pDynamicState			= &dynamicState;
	pipelineInfo.layout					= _testVkPipelineLayout.hnd();
	pipelineInfo.renderPass				= _testVkRenderPass.hnd();
	pipelineInfo.subpass				= 0;
	pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineInfo.basePipelineIndex		= -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	_testVkPipeline.create(renderer(), &pipelineInfo, 1, vkPipelineCache);
}

void
RenderContext_Vk::createTestVertexBuffer(Vk_Buffer* vkBuf, float z)
{
	//auto* vkAllocCallbacks	= renderer()->allocCallbacks();
	//auto* vkDev		= renderer()->vkDevice();
	auto* vkAlloc	= renderer()->memoryContext()->vkAlloc();

	auto vtxData = TestVertex::make2(z);
	auto bufSize = vtxData.size();

	Vk_Buffer			_vkStagingBuf;

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::CpuOnly;
		allocInfo.flags |= RenderAllocFlags::HostWrite;
		Util::createBuffer(_vkStagingBuf, vkAlloc, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Vk_ScopedMemMapBuf mm = { &_vkStagingBuf };
		memory_copy(mm.data<u8*>(), reinCast<u8*>(vtxData.data()), bufSize);
	}

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::GpuOnly;
		Util::createBuffer(*vkBuf, vkAlloc, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Util::copyBuffer(vkBuf, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), &_vkTransferQueue);
	}
}

void
RenderContext_Vk::createTestIndexBuffer()
{
	//auto* vkAllocCallbacks	= renderer()->allocCallbacks();
	//auto* vkDev = renderer()->vkDevice();
	auto* vkAlloc	= renderer()->memoryContext()->vkAlloc();

	auto indices = TestVertex::makeIndices();
	auto bufSize = sizeof(indices[0]) * indices.size();

	Vk_Buffer			_vkStagingBuf;

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::CpuOnly;
		allocInfo.flags |= RenderAllocFlags::HostWrite;
		Util::createBuffer(_vkStagingBuf, vkAlloc, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Vk_ScopedMemMapBuf mm = { &_vkStagingBuf };
		memory_copy(mm.data<u8*>(), reinCast<u8*>(indices.data()), bufSize);
	}

	{
		Vk_AllocInfo allocInfo;
		allocInfo.usage  = RenderMemoryUsage::GpuOnly;
		Util::createBuffer(_testVkIdxBuffer, vkAlloc, &allocInfo, bufSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Util::copyBuffer(&_testVkIdxBuffer, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), &_vkTransferQueue);
	}

	Util::copyBuffer(&_testVkIdxBuffer, &_vkStagingBuf, bufSize, renderFrame().commandPool(QueueTypeFlags::Transfer).hnd(), &_vkTransferQueue);
}

void 
RenderContext_Vk::createTestUniformBuffer()
{
	auto* vkAlloc			= renderer()->memoryContext()->vkAlloc();
	VkDeviceSize bufferSize = sizeof(TestUBO);

	Vk_AllocInfo allocInfo;
	allocInfo.usage  = RenderMemoryUsage::CpuToGpu;

	_testVkUniformBuffers.resize(s_kFrameInFlightCount);
	_memMapUniformBufs.reserve(s_kFrameInFlightCount);
	for (size_t i = 0; i < s_kFrameInFlightCount; ++i)
	{
		auto& uniBuf = _testVkUniformBuffers[i];

		uniBuf.create(vkAlloc, &allocInfo, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, QueueTypeFlags::Graphics);
		_memMapUniformBufs.emplace_back(&uniBuf);
	}
}

void 
RenderContext_Vk::createTestDescriptorSetLayout()
{
	Vector<VkDescriptorSetLayoutBinding, 16> bindings;

	{
		auto& e = bindings.emplace_back();
		e.binding				= 0;
		e.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		e.descriptorCount		= 1;
		e.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT;
		e.pImmutableSamplers	= nullptr; // Optional
	}
	{
		auto& e = bindings.emplace_back();
		e.binding				= 4;
		e.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		e.descriptorCount		= 1;
		e.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;
		e.pImmutableSamplers	= nullptr;
	}

	{
		auto& e = bindings.emplace_back();
		e.binding				= 8;
		e.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLER;
		e.descriptorCount		= 1;
		e.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;
		e.pImmutableSamplers	= nullptr;
	}////

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = sCast<u32>(bindings.size());
	layoutInfo.pBindings	= bindings.data();

	_testVkDescriptorSetLayout.create(&layoutInfo);
	g_testVkDescriptorSetLayout = _testVkDescriptorSetLayout.hnd();
}

void 
RenderContext_Vk::createTestDescriptorPool()
{
	Vector<VkDescriptorPoolSize, 16> poolSizes;
	{
		auto& e = poolSizes.emplace_back();
		e.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		e.descriptorCount	= sCast<u32>(s_kFrameInFlightCount);
	}
	{
		auto& e = poolSizes.emplace_back();
		e.type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		e.descriptorCount	= sCast<u32>(s_kFrameInFlightCount);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount	= sCast<u32>(poolSizes.size());
	poolInfo.pPoolSizes		= poolSizes.data();
	poolInfo.maxSets		= sCast<u32>(s_kFrameInFlightCount);
	//poolInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	_testVkDescriptorPool.create(&poolInfo, renderer());
}

void 
RenderContext_Vk::createTestDescriptorSets()
{
	auto* vkDev = renderer()->vkDevice();

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

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView		= _testVkTextureImageView.hnd();
		//imageInfo.sampler		= _testVkTextureSampler.hnd();


		VkDescriptorImageInfo samplerInfo = {};
		samplerInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//samplerInfo.imageView	= _testVkTextureImageView.hnd();
		samplerInfo.sampler		= _testVkTextureSampler.hnd();

		Vector<VkWriteDescriptorSet, 16> writeDescs;

		{
			auto& e = writeDescs.emplace_back();
			e.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			e.dstSet			= _testVkDescriptorSets[i].hnd();
			e.dstBinding		= 0;
			e.dstArrayElement	= 0;
			e.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			e.descriptorCount	= 1;
			e.pBufferInfo		= &bufferInfo;
			e.pImageInfo		= nullptr; // Optional
			e.pTexelBufferView	= nullptr; // Optional
		}

		{
			auto& e = writeDescs.emplace_back();
			e.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			e.dstSet			= _testVkDescriptorSets[i].hnd();
			e.dstBinding		= 4;
			e.dstArrayElement	= 0;
			e.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			e.descriptorCount	= 1;
			e.pBufferInfo		= nullptr;
			e.pImageInfo		= &imageInfo; // Optional
			e.pTexelBufferView	= nullptr; // Optional
		}

		{
			auto& e = writeDescs.emplace_back();
			e.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			e.dstSet			= _testVkDescriptorSets[i].hnd();
			e.dstBinding		= 8;
			e.dstArrayElement	= 0;
			e.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLER;
			e.descriptorCount	= 1;
			e.pBufferInfo		= nullptr;
			e.pImageInfo		= &samplerInfo; // Optional
			e.pTexelBufferView	= nullptr; // Optional
		}

		vkUpdateDescriptorSets(vkDev, sCast<u32>(writeDescs.size()), writeDescs.data(), 0, nullptr);
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
	ubo.proj		= Mat4f::s_perspective(math::radians(45.0f), aspectRatio(), 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	ubo.mvp			= ubo.proj * ubo.view * ubo.model;

	memory_copy(_memMapUniformBufs[curImageIdx].data<u8*>(), reinCast<u8*>(&ubo), sizeof(ubo));
}

void 
RenderContext_Vk::createTestTextureImage()
{
	auto* vkAlloc			= renderer()->memoryContext()->vkAlloc();

	Image image;
	image.load("asset/texture/uvChecker.png");

	Vk_Buffer stagingBuffer;

	{
		Vk_AllocInfo allocInfo = {};
		allocInfo.usage = RenderMemoryUsage::CpuToGpu;

		stagingBuffer.create(vkAlloc, &allocInfo, image.totalByteSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

		Vk_ScopedMemMapBuf memmap { &stagingBuffer };
		memory_copy(memmap.data<u8*>(), image.data().data(), image.totalByteSize());
	}

	u32 imageWidth	= sCast<u32>(image.width());
	u32 imageHeight = sCast<u32>(image.height());

	VkFormat vkFormat = VK_FORMAT_R8G8B8A8_SRGB;
	{
		Vk_AllocInfo allocInfo = {};
		allocInfo.usage = RenderMemoryUsage::GpuOnly;

		_testVkTextureImage.create(vkAlloc, &allocInfo
			, imageWidth, imageHeight
			, vkFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);
	}

	{
		auto* cmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Util::transitionImageLayout(&_testVkTextureImage, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, nullptr, &_vkTransferQueue, cmdBuf);
	}
	{
		auto* cmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Util::copyBufferToImage(&_testVkTextureImage, &stagingBuffer, imageWidth, imageHeight, &_vkTransferQueue, cmdBuf);
	}
	{
		auto* cmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Util::transitionImageLayout(&_testVkTextureImage, vkFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, nullptr, &_vkGraphicsQueue, cmdBuf);
	}
}

void 
RenderContext_Vk::createTestTextureImageView()
{
	_testVkTextureImageView.create(&_testVkTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void 
RenderContext_Vk::createTestTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter				= VK_FILTER_LINEAR;
	samplerInfo.minFilter				= VK_FILTER_LINEAR;
	samplerInfo.addressModeU			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable		= Renderer_Vk::instance()->adapterInfo().feature.hasSamplerAnisotropy;
	samplerInfo.maxAnisotropy			= Renderer_Vk::instance()->adapterInfo().limit.maxSamplerAnisotropy;
	samplerInfo.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable			= VK_FALSE;
	samplerInfo.compareOp				= VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias				= 0.0f;
	samplerInfo.minLod					= 0.0f;
	samplerInfo.maxLod					= 0.0f;

	_testVkTextureSampler.create(&samplerInfo);
}

#endif // 1

#if 0
#pragma mark --- rdsVkonRenderCommand-Impl ---
#endif // 0
#if 1


void 
RenderContext_Vk::onRenderCommand_ClearFramebuffers(RenderCommand_ClearFramebuffers* cmd)
{
	// vkCmdClearAttachments
	// vkCmdClearColorImage
	// vkCmdClearDepthStencilImage

}

void 
RenderContext_Vk::onRenderCommand_SwapBuffers(RenderCommand_SwapBuffers* cmd)
{
	_shdSwapBuffers = true;
	//_curGraphicsCmdBuf->swapBuffers(&_vkPresentQueue, _vkSwapchain, _curImageIdx, renderFrame().renderCompletedSmp());
}

void
RenderContext_Vk::onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd)
{
	_onRenderCommand_DrawCall(_curGraphicsCmdBuf, cmd);
}

void 
RenderContext_Vk::_onRenderCommand_DrawCall(Vk_CommandBuffer* cmdBuf, RenderCommand_DrawCall* cmd)
{
	if (!cmd->vertexLayout) { RDS_CORE_ASSERT(false, "drawcall no vertexLayout"); return; }

	auto* vkCmdBuf = cmdBuf->hnd();

	auto vtxCount = sCast<u32>(cmd->vertexCount);
	auto idxCount = sCast<u32>(cmd->indexCount);

	RenderGpuBuffer_Vk* idxBufVk = nullptr;
	if (idxCount > 0)
	{
		idxBufVk = sCast<RenderGpuBuffer_Vk*>(cmd->indexBuffer.ptr());
		if (!idxBufVk) { RDS_CORE_ASSERT(false, "drawcall no index buf while idxCount > 0"); return; }
	}

	auto* vtxBufHndVk = cmd->vertexBuffer ? sCast<RenderGpuBuffer_Vk*>(cmd->vertexBuffer.ptr())->vkBuf()->hnd() : nullptr;
	//if (vtxCount > 0 && !vtxBufHndVk) { RDS_CORE_ASSERT(false, "drawcall no vertex buf while vtxCount > 0"); return; }
	if (!vtxBufHndVk) { RDS_CORE_ASSERT(false, "drawcall no vertex buf"); return; }
	
	if (auto* pass = cmd->getMaterialPass())
	{
		pass->bind(this, cmd->vertexLayout);
	}
	else
	{
		Vk_DescriptorSet_T* descSets[] = { _testVkDescriptorSets[_vkSwapchain.curImageIdx()].hnd() };
		vkCmdBindPipeline(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _testVkPipeline.hnd());
		vkCmdBindDescriptorSets(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, _testVkPipelineLayout.hnd(), 0, 1, descSets, 0, nullptr);
	}

	Vk_Buffer_T* vertexBuffers[]	= { vtxBufHndVk };
	VkDeviceSize offsets[]			= { Util::toVkDeviceSize(cmd->vertexOffset) };
	vkCmdBindVertexBuffers(vkCmdBuf, 0, 1, vertexBuffers, offsets);

	if (idxCount > 0)
	{
		u32				idxOffset = sCast<u32>(cmd->indexOffset);
		VkIndexType		vkIdxType = Util::toVkIndexType(cmd->indexType);
		vkCmdBindIndexBuffer(vkCmdBuf, idxBufVk->vkBuf()->hnd(), idxOffset, vkIdxType);

		vkCmdDrawIndexed(vkCmdBuf, idxCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(vkCmdBuf, vtxCount, 1, 0, 0);
	}
}

void 
RenderContext_Vk::onRenderCommand_DrawRenderables(RenderCommand_DrawRenderables* cmd)
{
	RDS_PROFILE_SCOPED();

	#if 0

	static constexpr SizeType s_kMinBatchSize = 200;

	const auto&	drawCallCmds	= cmd->hashedDrawCallCmds->drawCallCmds();
	auto		drawCallCount	= drawCallCmds.size();
	//auto batchSize		= drawCallCount / OsTraits::logicalThreadCount();
	auto batchSizePerThread	= math::max(s_kMinBatchSize, drawCallCount / OsTraits::logicalThreadCount());
	auto jobCount			= sCast<SizeType>(math::ceil((float)drawCallCount / batchSizePerThread));

	class ParRecordDrawCall : public Job_Base
	{
	public:
		void setup(SizeType batchOffset, SizeType batchSize, SizeType batchId, RenderContext_Vk* rdCtx, HashedDrawCallCommands* drawCallCmds
			, Span<Vk_CommandBuffer*> outVkCmdBufs, Vk_Queue* graphicsQueue, Vk_RenderPass* renderPass, Vk_Framebuffer* vkFrameBuf, u32 subpassIdx, const math::Rect2f& framebufferRect2f)
		{
			_batchOffset		= batchOffset;
			_batchSize			= batchSize;
			_batchId			= batchId;
			_rdCtx				= rdCtx;
			_drawCallCmds		= drawCallCmds;
			_outVkCmdBufs		= outVkCmdBufs;
			_graphicsQueue		= graphicsQueue;
			_renderPass			= renderPass;
			_vkFrameBuf			= vkFrameBuf;
			_subpassIdx			= subpassIdx;
			_framebufferRect2f	= framebufferRect2f;
		}

		#if 0
		virtual void onBegin()
		{
			_notYetSupported(); // need to modify the jobsystem and test, if the job is no parent, then call the JobDispatchBase::onBegin/onEnd()

			auto* vkCmdBuf = _vkCmdBuf;

			vkCmdBuf->beginSecondaryRecord(_graphicsQueue, _renderPass, _vkFrameBuf, _subpassIdx);

			auto rect2 = Util::toRect2f(_swapchainInfo->extent);
			vkCmdBuf->setViewport(rect2);
			vkCmdBuf->setScissor (rect2);
		}

		virtual void onEnd()
		{
			_notYetSupported();

			auto* vkCmdBuf = _vkCmdBuf;

			vkCmdBuf->endRecord();
		}
		#endif // 0

		virtual void execute() override
		{
			RDS_PROFILE_SCOPED();

			auto* vkCmdBuf = _rdCtx->renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

			vkCmdBuf->beginSecondaryRecord(_graphicsQueue, _renderPass, _vkFrameBuf, _subpassIdx);

			const auto& rect2 = _framebufferRect2f;
			vkCmdBuf->setViewport(rect2);
			vkCmdBuf->setScissor (rect2);

			for (size_t i = 0; i < _batchSize; i++)
			{
				auto id = _batchOffset + i;

				const auto& drawCallCmds = _drawCallCmds->drawCallCmds();
				_rdCtx->_onRenderCommand_DrawCall(vkCmdBuf, drawCallCmds[id]);
			}

			vkCmdBuf->endRecord();

			_outVkCmdBufs[_batchId] = vkCmdBuf;
		}

	public:
		SizeType				_batchOffset	= 0;
		SizeType				_batchSize		= 0;
		SizeType				_batchId		= 0;
		RenderContext_Vk*		_rdCtx			= nullptr;
		HashedDrawCallCommands* _drawCallCmds	= nullptr;
		Span<Vk_CommandBuffer*> _outVkCmdBufs;
		Vk_Queue*				_graphicsQueue	= nullptr;
		Vk_RenderPass*			_renderPass		= nullptr;
		Vk_Framebuffer*			_vkFrameBuf		= nullptr;
		u32						_subpassIdx		= 0;
		Vk_SwapchainInfo*		_swapchainInfo	= nullptr;
		math::Rect2f			_framebufferRect2f;
	};

	Vector<Vk_CommandBuffer*,		s_kThreadCount> vkCmdBufs;
	Vector<UPtr<ParRecordDrawCall>, s_kThreadCount> recordJobs;
	Vector<JobHandle,				s_kThreadCount> recordJobHandles;

	recordJobs.resize(jobCount);
	recordJobHandles.resize(jobCount);
	vkCmdBufs.resize(jobCount);

	SizeType remain = drawCallCount;
	RDS_WARN_ONCE("TODO: make a JobGatherer, the for loop dispatch can ");
	for (int i = 0; i < jobCount; ++i)
	{
		auto& job		= recordJobs[i];
		job = makeUPtr<ParRecordDrawCall>();

		auto batchSize = math::min(batchSizePerThread, remain);
		remain -= batchSize;

		job->setup(batchSizePerThread * i, batchSize, i, this, cmd->hashedDrawCallCmds, vkCmdBufs.span(), vkGraphicsQueue()
			, &_testVkRenderPass, _vkSwapchain.framebuffer(), 0, _vkSwapchain.framebufferRect2f());

		auto handle = job->dispatch();
		recordJobHandles[i] = handle;
	}

	for (auto* e : recordJobHandles)
	{
		JobSystem::instance()->waitForComplete(e);
	}

	auto* primaryCmdBuf = _curGraphicsCmdBuf;
	primaryCmdBuf->executeSecondaryCmdBufs(vkCmdBufs.span());

	#else
	
	static constexpr u32 s_kMinBatchSize = 200;

	const auto&	drawCallCmds	= cmd->hashedDrawCallCmds->drawCallCmds();
	const auto	drawCallCount	= sCast<u32>(drawCallCmds.size());
	const auto	clusterInfo		= JobCluster::clusterInfo(drawCallCount, s_kMinBatchSize);

	class ParRecordDrawCall : public JobCluster_Base<JobFor_Base>
	{
	public:
		ParRecordDrawCall(RenderContext_Vk* rdCtx, HashedDrawCallCommands* drawCallCmds
						, Span<Vk_CommandBuffer*> outVkCmdBufs, Vk_Queue* graphicsQueue
						, Vk_RenderPass* renderPass, Vk_Framebuffer* vkFrameBuf, u32 subpassIdx, const math::Rect2f& framebufferRect2f)
		{
			_rdCtx				= rdCtx;
			_drawCallCmds		= drawCallCmds;
			_outVkCmdBufs		= outVkCmdBufs;
			_graphicsQueue		= graphicsQueue;
			_renderPass			= renderPass;
			_vkFrameBuf			= vkFrameBuf;
			_subpassIdx			= subpassIdx;
			_framebufferRect2f	= framebufferRect2f;
		}

		virtual void onBegin()
		{
			_vkCmdBuf = _rdCtx->renderFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

			auto* vkCmdBuf = _vkCmdBuf;
			vkCmdBuf->beginSecondaryRecord(_graphicsQueue, _renderPass, _vkFrameBuf, _subpassIdx);

			vkCmdBuf->setViewport(_framebufferRect2f);
			vkCmdBuf->setScissor (_framebufferRect2f);
		}

		virtual void onEnd()
		{
			auto* vkCmdBuf = _vkCmdBuf;

			vkCmdBuf->endRecord();
			_outVkCmdBufs[clusterArgs().clusterId] = vkCmdBuf;
		}

		virtual void execute(const JobArgs& args) override
		{
			auto id = clusterArgs().clusterOffset + args.batchIndex;
			auto* vkCmdBuf = _vkCmdBuf;

			const auto& drawCallCmds = _drawCallCmds->drawCallCmds();
			_rdCtx->_onRenderCommand_DrawCall(vkCmdBuf, drawCallCmds[id]);
		}

	public:
		RenderContext_Vk*		_rdCtx			= nullptr;
		HashedDrawCallCommands* _drawCallCmds	= nullptr;
		Span<Vk_CommandBuffer*> _outVkCmdBufs;
		Vk_Queue*				_graphicsQueue	= nullptr;
		Vk_RenderPass*			_renderPass		= nullptr;
		Vk_Framebuffer*			_vkFrameBuf		= nullptr;
		u32						_subpassIdx		= 0;
		Vk_SwapchainInfo*		_swapchainInfo	= nullptr;
		math::Rect2f			_framebufferRect2f;

		Vk_CommandBuffer*		_vkCmdBuf		= nullptr;
	};

	Vector<Vk_CommandBuffer*,		s_kThreadCount> vkCmdBufs;
	Vector<UPtr<ParRecordDrawCall>, s_kThreadCount> recordJobs;

	vkCmdBufs.resize(clusterInfo.clusterCount);

	auto jobClusterHnd = JobCluster::prepare(recordJobs, drawCallCount, s_kMinBatchSize
											, this, cmd->hashedDrawCallCmds, vkCmdBufs.span(), vkGraphicsQueue()
											, &_testVkRenderPass, _vkSwapchain.framebuffer(), 0, _vkSwapchain.framebufferRect2f());

	JobSystem::instance()->submit(jobClusterHnd);
	JobSystem::instance()->waitForComplete(jobClusterHnd);
	RDS_WARN_ONCE("TODO: DrawRenderables could delay wait?");

	auto* primaryCmdBuf = _curGraphicsCmdBuf;
	primaryCmdBuf->executeSecondaryCmdBufs(vkCmdBufs.span());

	#endif // 0
}



#endif // 1



#endif
}

#endif // RDS_RENDER_HAS_VULKAN