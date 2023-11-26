#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/transfer/rdsVk_TransferFrame.h"
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

static VkFormat g_testSwapchainVkFormat = VK_FORMAT_B8G8R8A8_UNORM; //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;

SPtr<RenderContext> 
RenderDevice_Vk::onCreateContext(const RenderContext_CreateDesc& cDesc)
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
{

}

RenderContext_Vk::~RenderContext_Vk()
{
	RDS_PROFILE_SCOPED();
	destroy();
}

void
RenderContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	auto* rdDevVk = renderDeviceVk();
	auto* vkDevice = rdDevVk->vkDevice();

	_vkRdFrames.resize(s_kFrameInFlightCount);
	for (size_t i = 0; i < s_kFrameInFlightCount; i++)
	{
		_vkRdFrames[i].create(this);
	}

	_vkGraphicsQueue.create(rdDevVk->queueFamilyIndices().graphics.value(), vkDevice);
	 _vkPresentQueue.create(rdDevVk->queueFamilyIndices().present.value(),  vkDevice);
	_vkTransferQueue.create(rdDevVk->queueFamilyIndices().transfer.value(), vkDevice);

	auto vkSwapchainCDesc = _vkSwapchain.makeCDesc();
	vkSwapchainCDesc.rdCtx				= this;
	vkSwapchainCDesc.wnd				= cDesc.window;
	vkSwapchainCDesc.framebufferRect2f	= math::toRect2_wh(framebufferSize());
	//vkSwapchainCDesc.vkRdPass			= &_testVkRenderPass;
	vkSwapchainCDesc.colorFormat		= g_testSwapchainVkFormat; //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;
	vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	vkSwapchainCDesc.depthFormat		= VK_FORMAT_D32_SFLOAT_S8_UINT;

	createTestRenderPass(vkSwapchainCDesc);
	_vkSwapchain.create(vkSwapchainCDesc);

	_curGraphicsVkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "RenderContext_Vk::_curGraphicsVkCmdBuf-Prim");

	RDS_TODO("recitfy");
	RDS_PROFILE_GPU_CTX_CREATE(_gpuProfilerCtx, "Main Window");

	_setDebugName();
}

void
RenderContext_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void
RenderContext_Vk::onDestroy()
{
	auto* rdDevVk = renderDeviceVk();

	rdDevVk->waitIdle();
	_testVkRenderPass.destroy(rdDevVk);

	_vkRdFrames.clear();
	_vkSwapchain.destroy(nullptr);
	Base::onDestroy();
}

void
RenderContext_Vk::onBeginRender()
{
	RDS_PROFILE_SCOPED();

	auto* rdDevVk = renderDeviceVk();

	VkResult ret = {};

	RDS_TODO("i tihnk no need to wait too early, since it will block the cpu to record RenderCommands, query the fence and reset the frame if it is signaled");
	{
		RDS_PROFILE_SECTION("vkWaitForFences()");
		vkRdFrame().inFlightFence()->wait(rdDevVk);
	}

	//vkResetFences(vkDevice, vkFenceCount, vkFences);		// reset too early will cause deadlock, since the invalidate wii cause no work submitted (returned) and then no one will signal it

	ret = _vkSwapchain.acquireNextImage(vkRdFrame().imageAvaliableSmp());

	if (!Util::isSuccess(ret))
	{
		invalidateSwapchain(ret, _vkSwapchain.framebufferSize());
		return;
	}

	{
		vkRdFrame().clear();

		_curGraphicsVkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "RenderContext_Vk::_curGraphicsVkCmdBuf-Prim");

		#if RDS_TEST_DRAW_CALL
		auto* vkCmdBuf		= _curGraphicsVkCmdBuf->hnd();
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
	endRecord(_curGraphicsVkCmdBuf->hnd(), _curImageIdx);
	_shdSwapBuffers = true;
	#endif // RDS_TEST_DRAW_CALL


	if (_shdSwapBuffers)
	{
		RDS_TODO("handle do not get next image idx if no swap buffers in the next frame");
		auto ret = _vkSwapchain.swapBuffers(&_vkPresentQueue, _curGraphicsVkCmdBuf, vkRdFrame().renderCompletedSmp());
		invalidateSwapchain(ret, _vkSwapchain.framebufferSize());
		_shdSwapBuffers = false;
}

	// next frame idx
	_curFrameIdx = (_curFrameIdx + 1) % s_kFrameInFlightCount;
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

	beginRecord(_curGraphicsVkCmdBuf->hnd(), _vkSwapchain.curImageIdx());

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
		_curGraphicsVkCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		#else
		_curGraphicsVkCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_INLINE);
		_curGraphicsVkCmdBuf->setViewport(fbufRect2);
		_curGraphicsVkCmdBuf->setScissor (fbufRect2);
		#endif // TE
	}

	//_curGraphicsVkCmdBuf->setViewport(Util::toRect2f(_swapchainInfo.extent));
	// _curGraphicsVkCmdBuf->setScissor(Util::toRect2f(_swapchainInfo.extent));

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
	_curGraphicsVkCmdBuf->endRenderPass();

	endRecord(_curGraphicsVkCmdBuf->hnd(), _vkSwapchain.curImageIdx());

	#endif // !RDS_TEST_DRAW_CALL
}

void 
RenderContext_Vk::test_extraDrawCall(RenderCommandBuffer& renderCmdBuf)
{
	RDS_WARN_ONCE("TODO: renderCmdBuf.drawCallCmds() for a extra Sec.cmd buf to execute but seems has order problem or maybe copy unity to have a executeCmdBuf()");
	{
		auto* vkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY, "RenderContext_Vk::extraDrawCall-2ry");

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

		auto* primaryCmdBuf = _curGraphicsVkCmdBuf;
		primaryCmdBuf->executeSecondaryCmdBufs(Span<Vk_CommandBuffer*>{ &vkCmdBuf, 1});
	}
}

void
RenderContext_Vk::beginRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();

	_curGraphicsVkCmdBuf->beginRecord(vkGraphicsQueue(), VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	RDS_PROFILE_GPU_ZONET_VK(_gpuProfilerCtx, _curGraphicsVkCmdBuf->hnd(), "RenderContext_Vk::onCommit");
}

void
RenderContext_Vk::endRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	RDS_PROFILE_SCOPED();

	auto* rdDevVk = renderDeviceVk();

	RDS_PROFILE_GPU_COLLECT_VK(_gpuProfilerCtx, vkCmdBuf);

	_curGraphicsVkCmdBuf->endRecord();

	vkRdFrame().inFlightFence()->reset(rdDevVk);
	#if 1

	Vector<Vk_SmpSubmitInfo, 8> waitSmps;
	Vector<Vk_SmpSubmitInfo, 8> signalSmps;

	waitSmps.emplace_back	(Vk_SmpSubmitInfo{vkRdFrame().imageAvaliableSmp()->hnd(),									VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR});
	if (auto* tsfCompletedVkSmp = vkTransferFrame().requestCompletedVkSmp(QueueTypeFlags::Graphics))
	{
		waitSmps.emplace_back	(Vk_SmpSubmitInfo{tsfCompletedVkSmp->hnd(),	VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT});
	}

	signalSmps.emplace_back	(Vk_SmpSubmitInfo{vkRdFrame().renderCompletedSmp()->hnd(),								VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR});

	_curGraphicsVkCmdBuf->submit(vkRdFrame().inFlightFence(), waitSmps, signalSmps);

	#else

	_curGraphicsVkCmdBuf->submit(vkRdFrame().inFlightFence()
		, vkRdFrame().imageAvaliableSmp(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR
		, vkRdFrame().renderCompletedSmp(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);

	#endif // 0

}

void
RenderContext_Vk::bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline)
{
	RDS_PROFILE_SCOPED();

	vkCmdBindPipeline(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->hnd());

	// since we set viewport and scissor state is dynamic
	const auto& rect2f = _vkSwapchain.framebufferRect2f();
	_curGraphicsVkCmdBuf->setViewport(rect2f);
	_curGraphicsVkCmdBuf->setScissor(rect2f);
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
	_curGraphicsVkCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), rect2f, clearValues.span(), VK_SUBPASS_CONTENTS_INLINE);
}

void 
RenderContext_Vk::endRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx)
{
	_curGraphicsVkCmdBuf->endRenderPass();
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
		vkSwapchainCDesc.colorFormat		= g_testSwapchainVkFormat; //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;
		vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		vkSwapchainCDesc.depthFormat		= VK_FORMAT_D32_SFLOAT_S8_UINT;

		// the render pass should have same format after invalidate
		RDS_TODO("if it is invalidate, those ptr is not changed, only size is changed, the RenderPass should store the color format to match with the swapchain");

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
RenderContext_Vk::createTestRenderPass(Vk_Swapchain_CreateDesc& vkSwapchainCDesc)
{
	//auto* vkDevice = rdDevVk()->vkDevice();
	//const auto* vkAllocCallbacks = rdDevVk()->allocCallbacks();

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

	_testVkRenderPass.create(&renderPassInfo, renderDeviceVk());
	vkSwapchainCDesc.vkRdPass = &_testVkRenderPass;
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
	//_curGraphicsVkCmdBuf->swapBuffers(&_vkPresentQueue, _vkSwapchain, _curImageIdx, vkRdFrame().renderCompletedSmp());
}

void 
RenderContext_Vk::onRenderCommand_SetScissorRect(RenderCommand_SetScissorRect* cmd)
{
	auto* vkCmdBuf = _curGraphicsVkCmdBuf;
	
	vkCmdBuf->setScissor(cmd->rect);
}

void
RenderContext_Vk::onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd)
{
	_onRenderCommand_DrawCall(_curGraphicsVkCmdBuf, cmd);
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
		RDS_THROW("RenderCommand_DrawCall no passes");
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

			auto* vkCmdBuf = _rdCtx->vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

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

	auto* primaryCmdBuf = _curGraphicsVkCmdBuf;
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
			_vkCmdBuf = _rdCtx->vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY, "RenderContext_Vk::ParRecordDrawCall-2ry");

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

	auto* primaryCmdBuf = _curGraphicsVkCmdBuf;
	primaryCmdBuf->executeSecondaryCmdBufs(vkCmdBufs.span());

	#endif // 0
}

#endif // 1

void 
RenderContext_Vk::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME_FMT(_testVkRenderPass);
	RDS_VK_SET_DEBUG_NAME_FMT(_vkSwapchain);

	RDS_VK_SET_DEBUG_NAME_FMT(_vkGraphicsQueue);
	RDS_VK_SET_DEBUG_NAME_FMT(_vkPresentQueue);
	RDS_VK_SET_DEBUG_NAME_FMT(_vkTransferQueue);
}


#endif
}

#endif // RDS_RENDER_HAS_VULKAN