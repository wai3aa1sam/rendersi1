#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/transfer/rdsVk_TransferFrame.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/shader/rdsMaterial_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#if RDS_RENDER_HAS_VULKAN

#define RDS_TEST_MT_DRAW_CALLS 0
#define RDS_TEST_DUMMY_FOR_NO_SWAP_BUF 1

namespace rds
{

//static VkFormat g_testSwapchainVkFormat = VK_FORMAT_B8G8R8A8_UNORM; //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;
static VkFormat g_testSwapchainVkFormat = VK_FORMAT_R8G8B8A8_UNORM; //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;

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
	vkSwapchainCDesc.outBackbuffers		= &_backbuffers;
	vkSwapchainCDesc.framebufferRect2f	= math::toRect2_wh(framebufferSize());
	//vkSwapchainCDesc.vkRdPass			= &_testVkRenderPass;
	vkSwapchainCDesc.colorFormat		= Util::toVkFormat(cDesc.backbufferFormat); //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;
	vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	vkSwapchainCDesc.depthFormat		= Util::toVkFormat(cDesc.depthFormat);

	createTestRenderPass(vkSwapchainCDesc);
	_vkSwapchain.create(vkSwapchainCDesc);
	
	#if RDS_DEVELOPMENT

	{
		Vk_GpuProfilerContext_CreateDesc gpuProfilercDesc = {};
		gpuProfilercDesc.rdCtx		= this;
		gpuProfilercDesc.name		= "gfx";
		gpuProfilercDesc.queueType	= QueueTypeFlags::Graphics;
		gpuProfilercDesc.vkQueue	= this->vkGraphicsQueue();
		_gfxProfiler.create(gpuProfilercDesc);

		//_computeProfiler.create(cDesc);
	}

	#endif // RDS_DEVELOPMENT


	_setDebugName();

	_vkRdPassPool.create(renderDeviceVk());
	//_vkFramebufPool.create(renderDeviceVk());
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
	_backbuffers.destroy();

	//_vkFramebufPool.destroy();
	_vkRdPassPool.destroy();

	Base::onDestroy();
}

void 
RenderContext_Vk::addPendingGraphicsVkCommandBufHnd(Vk_CommandBuffer_T* hnd)
{
	_pendingGfxVkCmdbufHnds.emplace_back(hnd);
}

bool 
RenderContext_Vk::isFrameCompleted()
{
	return vkRdFrame().inFlightFence()->isSignaled(renderDeviceVk()) || vkRdFrame().submitCount() == 0;
}

void
RenderContext_Vk::onBeginRender()
{
	RDS_PROFILE_SCOPED();

	auto*		rdDevVk = renderDeviceVk();
	VkResult	ret		= {};

	//RDS_TODO("i tihnk no need to wait too early, since it will block the cpu to record RenderCommands, query the fence and reset the frame if it is signaled");
	// 2023.12.19: but the cmd buf is still executing, must wait before reset, must wait here
	{
		RDS_PROFILE_SECTION("vkWaitForFences()");
		if (vkRdFrame().submitCount() > 0)
		{
			RDS_PROFILE_SCOPED();
		}
		if (vkRdFrame().submitCount() != 0)
		{
			vkRdFrame().inFlightFence()->wait(rdDevVk);
		}
	}

	//vkResetFences(vkDevice, vkFenceCount, vkFences);		// reset too early will cause deadlock, since the invalidate wii cause no work submitted (returned) and then no one will signal it
	
	ret = _vkSwapchain.acquireNextImage(_curImageIdx, vkRdFrame().imageAvaliableSmp()); RDS_UNUSED(ret);
	if (!Util::isSuccess(ret))
	{
		//invalidateSwapchain(ret, framebufferSize());
		////return;
	}

	{
		{
			RDS_TODO("remove temporary test, frame buf should be cache and reuse, but current solution has problem when cache and reuse when using rdGraph");
			vkRdFrame()._vkFramebufPool.destroy();
			vkRdFrame()._vkFramebufPool.create(rdDevVk);

			//
			vkRdFrame().reset();
		}
	}

}

void
RenderContext_Vk::onEndRender()
{
	RDS_PROFILE_SCOPED();

	auto*		rdDevVk = renderDeviceVk();
	rdDevVk->bindlessResourceVk().commit();

	// submit
	{
		//auto* rdDevVk = renderDeviceVk();
		vkRdFrame().inFlightFence()->reset(rdDevVk);

		#if RDS_TEST_DUMMY_FOR_NO_SWAP_BUF
		if (_pendingGfxVkCmdbufHnds.is_empty())
		{
			rdDevVk->waitIdle();

			// easy handle for deadlock when nothing is committed
			auto* vkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "dummyBegin");
			vkCmdBuf->beginRecord(vkGraphicsQueue());
			vkCmdBuf->endRecord();
			_pendingGfxVkCmdbufHnds.emplace_back(vkCmdBuf->hnd());	
		}
		//else
		#endif // 0
		{
			Vector<Vk_SmpSubmitInfo, 8> waitSmps;
			Vector<Vk_SmpSubmitInfo, 8> signalSmps;

			waitSmps.emplace_back	(Vk_SmpSubmitInfo{vkRdFrame().imageAvaliableSmp()->hnd(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR});
			if (auto* tsfCompletedVkSmp = vkTransferFrame().requestCompletedVkSmp(QueueTypeFlags::Graphics))
			{
				waitSmps.emplace_back	(Vk_SmpSubmitInfo{tsfCompletedVkSmp->hnd(),	VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT});
			}

			signalSmps.emplace_back	(Vk_SmpSubmitInfo{vkRdFrame().renderCompletedSmp()->hnd(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR});
			
			Vk_CommandBuffer::submit(vkGraphicsQueue(), _pendingGfxVkCmdbufHnds, vkRdFrame().inFlightFence(), waitSmps, signalSmps);
		}

		#if RDS_TEST_DUMMY_FOR_NO_SWAP_BUF && 0
		if (_pendingGfxVkCmdbufHnds.is_empty())
		{
			auto* vkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "dummyEnd");
			vkCmdBuf->beginRecord(vkGraphicsQueue());
			vkCmdBuf->endRecord();

			_pendingGfxVkCmdbufHnds.clear();
			_pendingGfxVkCmdbufHnds.emplace_back(vkCmdBuf->hnd());

			Vector<Vk_SmpSubmitInfo, 1> vkSmpWaitDummy;
			vkSmpWaitDummy.emplace_back(Vk_SmpSubmitInfo{ vkRdFrame().renderCompletedSmp()->hnd(), VK_PIPELINE_STAGE_2_NONE });
			Vk_CommandBuffer::submit(vkGraphicsQueue(), _pendingGfxVkCmdbufHnds, vkRdFrame().vkFenceDummy(), vkSmpWaitDummy, {});
			//_hasSwapedBuffersInLastFrame = false;
		}
		#endif // 0
	}

	// present if needed
	auto ret = _vkSwapchain.swapBuffers(&_vkPresentQueue, vkRdFrame().renderCompletedSmp()); RDS_UNUSED(ret);

	if (_shdSwapBuffers)
	{
		RDS_TODO("handle do not get next image idx if no swap buffers in the next frame");
		//invalidateSwapchain(ret, _vkSwapchain.framebufferSize());
		_shdSwapBuffers = false;
	}

	// next frame idx
	{
		vkRdFrame().setSubmitCount(_pendingGfxVkCmdbufHnds.size());
		_pendingGfxVkCmdbufHnds.clear();
		_presentVkCmdBuf = nullptr;

		_curFrameIdx = (_curFrameIdx + 1) % s_kFrameInFlightCount;
	}
}

void
RenderContext_Vk::onSetFramebufferSize(const Vec2f& newSize)
{
	invalidateSwapchain(VK_ERROR_OUT_OF_DATE_KHR, newSize);
}

void 
RenderContext_Vk::onCommit(RenderCommandBuffer& renderCmdBuf)
{
	RDS_PROFILE_SCOPED();

	Base::onCommit(renderCmdBuf);

	if (!_vkSwapchain.isValid())
		return;

	auto* vkCmdBuf = vkRdFrame().requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "RenderContext_Vk::onCommit-Graphics");
	
	vkCmdBuf->beginRecord(vkGraphicsQueue(), VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	_gfxProfiler.beginProfile(vkCmdBuf->hnd(), "RenderContext_Vk::onCommit(RenderCommandBuffer)");

	renderDeviceVk()->bindlessResourceVk().bind(vkCmdBuf->hnd(), VK_PIPELINE_BIND_POINT_GRAPHICS);

	#if !RDS_TEST_DRAW_CALL

	// begin render pass
	{
		Vector<VkClearValue, 4> clearValues;
		auto* clearValueCmd = renderCmdBuf.getClearValue();
		Util::getVkClearValuesTo(clearValues, clearValueCmd, 1, true);

		auto fbufRect2 = _vkSwapchain.framebufferRect2f(); RDS_UNUSED(fbufRect2);

		#if RDS_TEST_MT_DRAW_CALLS
		// wait job sysytem handle
		_curGraphicsVkCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		#else
		vkCmdBuf->beginRenderPass(&_testVkRenderPass, _vkSwapchain.framebuffer(), fbufRect2, clearValues.span(), VK_SUBPASS_CONTENTS_INLINE);
		//vkCmdBuf->setViewport(fbufRect2);
		//vkCmdBuf->setScissor (fbufRect2);
		#endif // TE
	}

	for (auto* cmd : renderCmdBuf.commands())
	{
		_dispatchCommand(this, cmd, vkCmdBuf);
	}

	//test_extraDrawCall(renderCmdBuf);

	// end render pass
	vkCmdBuf->endRenderPass();

	_gfxProfiler.endProfile(vkCmdBuf->hnd());
	//RDS_PROFILE_GPU_COLLECT_VK(_gfxProfiler, vkCmdBuf->hnd());

	vkCmdBuf->endRecord();

	addPendingGraphicsVkCommandBufHnd(vkCmdBuf->hnd());

	#endif // !RDS_TEST_DRAW_CALL
}

void 
RenderContext_Vk::onCommit(RenderGraph& rdGraph)
{
	class Vk_RenderGraph
	{
	public:
		void commit(RenderGraph& rdGraph, RenderContext_Vk* rdCtxVk)
		{
			_rdCtxVk = rdCtxVk;
			auto*	rdDevVk			= _rdCtxVk->renderDeviceVk();

			_curVkCmdBufGraphics	= requestVkCmdBuf(RdgPassTypeFlags::Graphics, fmtAs_T<TempString>("{}-main_graphics", rdGraph.name()));
			//_curVkCmdBufCompute		= requestVkCmdBuf(RdgPassTypeFlags::Compute, "");

			{
				auto* vkCmdBuf = _curVkCmdBufGraphics;
				_curVkCmdBufGraphics->beginRecord();
				rdDevVk->bindlessResourceVk().bind(vkCmdBuf->hnd(), VK_PIPELINE_BIND_POINT_GRAPHICS);
				rdDevVk->bindlessResourceVk().bind(vkCmdBuf->hnd(), VK_PIPELINE_BIND_POINT_COMPUTE);
			}

			//_curVkCmdBufCompute->beginRecord();

			Span<RdgPass*> resultPasses = rdGraph.resultPasses();
			for (RdgPass* pass : resultPasses)
			{
				_commitPass(pass);
			}

			_curVkCmdBufGraphics->endRecord();
		}

		void addGraphicsVkCommandBufHnd(Vk_CommandBuffer_T* hnd)
		{
			_rdCtxVk->addPendingGraphicsVkCommandBufHnd(hnd);
		}

		void _commitPass(RdgPass* pass)
		{
			if (pass->isCommitted())
				return;

			pass->checkValid(); 
			if (!pass->isValid())
				return;

			//RdgPassTypeFlags	typeFlags		= pass->typeFlags();
			//bool				isGraphicsQueue	= BitUtil::hasAny(typeFlags, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);

			Vk_CommandBuffer*	vkCmdBuf	= nullptr;
			Vk_RenderPass*		vkRdPass	= nullptr;
			Vk_Framebuffer*		vkFramebuf	= nullptr;

			bool isSuccess = _getVkResources(pass, &vkCmdBuf, &vkRdPass, &vkFramebuf);
			if (!isSuccess)
				return;

			_recordCommands(pass, vkCmdBuf, vkRdPass, vkFramebuf);

			// submit
			/*if (isGraphicsQueue(pass) && !isMainVkCommandBuffer(vkCmdBuf))
			{
				addGraphicsVkCommandBufHnd(vkCmdBuf->hnd());
			}
			else
			{
				_notYetSupported(RDS_SRCLOC);
			}*/

			pass->_internal_commit();
		}

		bool _getVkResources(RdgPass* pass, Vk_CommandBuffer** outVkCmdBuf, Vk_RenderPass** outVkRdPass, Vk_Framebuffer** outVkFramebuf)
		{
			RDS_TODO("later maybe support multi-thread record, then those pool should be locked");

			Vk_RenderPassPool&	vkRdPassPool	= _rdCtxVk->_vkRdPassPool;
			//Vk_FramebufferPool& vkFramebufPool	= _rdCtxVk->_vkFramebufPool;
			Vk_FramebufferPool& vkFramebufPool = _rdCtxVk->vkRdFrame()._vkFramebufPool;

			bool hasRenderPass	= pass->hasRenderPass();

			bool isRequestNewCmdBuf = false;
			RdgPassTypeFlags typeFlags = pass->typeFlags();
			if (isRequestNewCmdBuf || BitUtil::hasOnly(typeFlags, RdgPassTypeFlags::AsyncCompute) || BitUtil::hasOnly(typeFlags, RdgPassTypeFlags::Transfer))
				*outVkCmdBuf = requestVkCmdBuf(typeFlags, pass->name());
			else
				*outVkCmdBuf = _curVkCmdBufGraphics;

			if (hasRenderPass)
			{
				Vk_RenderPass*	vkRdPass	= vkRdPassPool.request(pass);
				Vk_Framebuffer* vkFramebuf	= vkFramebufPool.request(pass, vkRdPass->hnd());

				*outVkRdPass	= vkRdPass;
				*outVkFramebuf	= vkFramebuf;

				RDS_CORE_ASSERT(vkRdPass && vkFramebuf, "");
				if (!vkRdPass || !vkFramebuf)
					return false;
			}

			return true;
		}

		void _recordCommands(RdgPass* pass, Vk_CommandBuffer* vkCmdBuf, Vk_RenderPass* vkRdPass, Vk_Framebuffer* vkFramebuf)
		{
			auto*	rdDevVk			= _rdCtxVk->renderDeviceVk();
			bool	hasRenderPass	= pass->hasRenderPass();

			Opt<Rect2f> rdTargetExtent = pass->renderTargetExtent();
			if (!rdTargetExtent && hasRenderPass)
				return;

			Vector<VkClearValue, 16> clearValues;
			if (hasRenderPass)	// && pass->has clear operation
			{
				auto* clearValue = pass->commandBuffer().getClearValue();
				Util::getVkClearValuesTo(clearValues, clearValue, pass->renderTargets().size(), pass->depthStencil());
			}

			const char* passName = pass->name().c_str();

			GpuProfilerContext_Vk* queueProfiler = nullptr;
			queueProfiler = pass->isAsyncCompute() ? _rdCtxVk->computeQueueProfiler() : _rdCtxVk->graphicsQueueProfiler();
			RDS_CORE_ASSERT(queueProfiler, "queueProfiler");

			bool isMainVkCmdBuf = isMainVkCommandBuffer(vkCmdBuf);
			if (!isMainVkCmdBuf)
			{
				vkCmdBuf->beginRecord(passName, _rdCtxVk->renderDeviceVk());
				rdDevVk->bindlessResourceVk().bind(vkCmdBuf->hnd(), VK_PIPELINE_BIND_POINT_GRAPHICS);
				rdDevVk->bindlessResourceVk().bind(vkCmdBuf->hnd(), VK_PIPELINE_BIND_POINT_COMPUTE);
			}
			else
			{
				vkCmdBuf->insertDebugLabel(passName);
			}

			queueProfiler->beginProfile(vkCmdBuf->hnd(), pass->name().c_str());

			_recordBarriers(pass, vkCmdBuf);

			RDS_TODO("Vk_RenderPassScope");
			if (hasRenderPass)
			{
				const auto& framebufRect2f = rdTargetExtent.value();
				vkCmdBuf->beginRenderPass(vkRdPass, vkFramebuf, framebufRect2f, clearValues, VK_SUBPASS_CONTENTS_INLINE);
				//vkCmdBuf->setViewport(framebufRect2f);
				//vkCmdBuf->setScissor (framebufRect2f);
			}
			for (auto* cmd : pass->commnads())
			{
				_rdCtxVk->_dispatchCommand(_rdCtxVk, cmd, vkCmdBuf);
			}

			if (hasRenderPass)
			{
				vkCmdBuf->endRenderPass();

				// vkSwapchain.vkImageHnd() is different with rdGraph backBuffer, need to solve
				//if (_rdCtxVk->_shdSwapBuffers)
				//{
				//	auto& vkSwapchain	= _rdCtxVk->_vkSwapchain;
				//	auto* vkImgHnd		= vkSwapchain.vkImageHnd();
				//	vkCmdBuf->cmd_addImageMemBarrier(vkImgHnd, vkSwapchain.colorFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				//	//vkCmdBuf->cmd_addImageMemBarrier(vkImgHnd, vkSwapchain.colorFormat(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				//	_rdCtxVk->_shdSwapBuffers = false;
				//}
			}


			queueProfiler->endProfile(vkCmdBuf->hnd());

			if (!isMainVkCmdBuf)
				vkCmdBuf->endRecord();

			/*
			maybe use sync pt level as each prim buffer, then submit between sync pt
			*/
		}

		void _recordBarriers(RdgPass* pass, Vk_CommandBuffer* vkCmdBuf)
		{
			auto rscAccesses = pass->resourceAccesses();
			for (const auto& rscAccess : rscAccesses)
			{
				using SRC = RdgResourceType;

				auto*	rsc		= rscAccess.rsc;
				auto	type	= rsc->type();
				StrView name	= rsc->name(); RDS_UNUSED(name);

				if (!RenderResourceState::isTransitionNeeded(rscAccess.srcState, rscAccess.dstState))
					continue;

				switch (type)
				{
					case SRC::Buffer:	
					{
						//auto* rdgBuf	= sCast<RdgTexture*>(rsc);
						//auto* bufVk		= sCast<RenderGpuBuffer_Vk*>(RdgResourceAccessor::access(rdgBuf));
						auto srcUsage = RenderResourceStateFlagsUtil::getBufferUsageFlags(rscAccess.srcState);
						if (srcUsage != RenderGpuBufferTypeFlags::None)
						{
							vkCmdBuf->cmd_addMemoryBarrier(rscAccess.srcState, rscAccess.dstState);
						}
					} break;

					case SRC::Texture:	
					{
						auto* rdgTex	= sCast<RdgTexture*>(rsc);
						auto* vkImgHnd	= Vk_Texture::getVkImageHnd(RdgResourceAccessor::access(rdgTex));

						VkImageLayout	srcLayout	= Util::toVkImageLayout(rscAccess.srcState);
						VkImageLayout	dstLayout	= Util::toVkImageLayout(rscAccess.dstState);

						vkCmdBuf->cmd_addImageMemBarrier(vkImgHnd, srcLayout, dstLayout, rdgTex->desc());
					} break;

					default: { RDS_THROW("invalid RenderGraphResource type: {}", type); } break;
				}
			}
		}

		// only transition final
		void transitExportedResources(RenderGraph& rdGraph)
		{
			// use Transfer Queue should be better as all exported are only avaliable in next frame, but need modify code
			//auto* vkCmdBuf = _rdCtxVk->requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "Transit Exported Resources");
			auto* vkCmdBuf = _rdCtxVk->requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "Transit Exported Resources");

			vkCmdBuf->beginRecord();
			
			for (auto& expBuf : rdGraph.exportedBuffers())
			{
				using SRC = RdgResourceType;

				auto* rsc				= expBuf.rdgRsc;
				const auto& stateTrack = sCast<const RdgResource*>(rsc)->stateTrack(); RDS_UNUSED(stateTrack);

				auto srcState	= rsc->renderResource()->renderResourceStateFlags();

				#if 0
				auto srcUsage	= StateUtil::getBufferUsageFlags(srcState);
				auto srcAccess	= StateUtil::getRenderAccess	(srcState);

				auto dstUsage	= StateUtil::getBufferUsageFlags(expBuf.pendingState);
				auto dstAccess	= StateUtil::getRenderAccess	(expBuf.pendingState);
				#endif // 1

				if (srcState == expBuf.pendingState)
					continue;

				vkCmdBuf->cmd_addMemoryBarrier(srcState, expBuf.pendingState);
			}

			for (auto& expTex : rdGraph.exportedTextures())
			{
				using SRC = RdgResourceType;

				auto* rsc				= expTex.rdgRsc;
				const auto& stateTrack	= sCast<const RdgResource*>(rsc)->stateTrack(); RDS_UNUSED(stateTrack);

				auto srcState	= rsc->renderResource()->renderResourceStateFlags();

				if (srcState == expTex.pendingState)
					continue;

				auto* rdgTex	= sCast<RdgTexture*>(rsc);
				auto* vkImgHnd	= Vk_Texture::getVkImageHnd(RdgResourceAccessor::access(rdgTex));

				VkImageLayout	srcLayout	= Util::toVkImageLayout(srcState);
				VkImageLayout	dstLayout	= Util::toVkImageLayout(expTex.pendingState);

				vkCmdBuf->cmd_addImageMemBarrier(vkImgHnd, srcLayout, dstLayout, rdgTex->desc());
			}

			vkCmdBuf->endRecord();
		}

		Vk_CommandBuffer* requestVkCmdBuf(RdgPassTypeFlags typeFlags, StrView name)
		{
			Vk_CommandBuffer* outVkCmdBuf = nullptr;

			if		(BitUtil::hasOnly(typeFlags, RdgPassTypeFlags::Transfer))		
			{ 
				outVkCmdBuf = _rdCtxVk->requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, name);
			}
			else if	(BitUtil::hasOnly(typeFlags, RdgPassTypeFlags::AsyncCompute))	
			{ 
				outVkCmdBuf = _rdCtxVk->requestCommandBuffer(QueueTypeFlags::Compute, VK_COMMAND_BUFFER_LEVEL_PRIMARY, name); 
			}
			else
			{
				outVkCmdBuf = _rdCtxVk->requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, name);
				addGraphicsVkCommandBufHnd(outVkCmdBuf->hnd());
			}

			RDS_CORE_ASSERT(outVkCmdBuf);

			return outVkCmdBuf;
		}

		bool isMainVkCommandBuffer(Vk_CommandBuffer* vkCmdBuf) const
		{
			return vkCmdBuf == _curVkCmdBufGraphics || vkCmdBuf == _curVkCmdBufCompute;
		}

		//Span<Vk_CommandBuffer_T*> graphicsVkCmdBufsHnds() { return _graphicsVkCmdBufsHnds; }

		static bool isGraphicsQueue(RdgPass* pass)
		{
			RdgPassTypeFlags	typeFlags		= pass->typeFlags();
			bool				isGraphicsQueue	= BitUtil::hasAny(typeFlags, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute | RdgPassTypeFlags::Transfer);
			return isGraphicsQueue;
		}

	private:
		RenderContext_Vk*				_rdCtxVk = nullptr;
		//Vector<Vk_CommandBuffer_T*, 64> _graphicsVkCmdBufsHnds;
		Vk_CommandBuffer*				_curVkCmdBufGraphics	= nullptr;
		Vk_CommandBuffer*				_curVkCmdBufCompute		= nullptr;
	};

	RDS_PROFILE_SCOPED();

	if (rdGraph.resultPasses().is_empty())
		return;

	//if (!_vkSwapchain.isValid())
	//	return;

	Base::onCommit(rdGraph);

	Vk_RenderGraph vkRdGraph;
	vkRdGraph.commit(rdGraph, this);
	vkRdGraph.transitExportedResources(rdGraph);
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
		vkSwapchainCDesc.outBackbuffers		= &_backbuffers;
		vkSwapchainCDesc.framebufferRect2f	= math::toRect2_wh(newSize);
		vkSwapchainCDesc.vkRdPass			= &_testVkRenderPass;
		vkSwapchainCDesc.colorFormat		= _vkSwapchain.colorFormat(); //VK_FORMAT_B8G8R8A8_SRGB VK_FORMAT_B8G8R8A8_UNORM;
		vkSwapchainCDesc.colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		vkSwapchainCDesc.depthFormat		= _vkSwapchain.depthFormat();

		_vkSwapchain.create(vkSwapchainCDesc);

		return;
	}
	else
	{
		throwIf(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR, "");	// VK_SUBOPTIMAL_KHR 
	}
}

Vk_CommandBuffer* 
RenderContext_Vk::requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName)
{
	using SRC = rds::QueueTypeFlags;
	switch (queueType)
	{
		case SRC::Graphics: { auto* o = vkRdFrame().requestCommandBuffer(queueType, bufLevel, debugName); o->reset(&_vkGraphicsQueue);	return o; } break;
	//	case SRC::Compute:	{ auto* o = vkRdFrame().requestCommandBuffer(queueType, bufLevel, debugName); o->reset(&_vkComputeQueue);	return o; } break;
		case SRC::Transfer:	{ auto* o = vkRdFrame().requestCommandBuffer(queueType, bufLevel, debugName); o->reset(&_vkTransferQueue);	return o; } break;
		default: { RDS_THROW("invalid vk queue type"); } break;
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
RenderContext_Vk::onRenderCommand_Dispatch(RenderCommand_Dispatch* cmd, void* userData)
{
	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);

	if (auto* pass = cmd->getMaterialPass())
	{
		auto* vkMtlPass = sCast<MaterialPass_Vk*>(pass);
		vkMtlPass->onBind(this, vkCmdBuf, cmd->materialFrameIndex());
	}
	else
	{
		RDS_THROW("onRenderCommand_Dispatch no passes");
	}

	vkCmdBuf->cmd_dispatch(cmd->threadGroups.x, cmd->threadGroups.y, cmd->threadGroups.z);
}

void 
RenderContext_Vk::onRenderCommand_ClearFramebuffers(RenderCommand_ClearFramebuffers* cmd, void* userData)
{
	// vkCmdClearAttachments
	// vkCmdClearColorImage
	// vkCmdClearDepthStencilImage
}

void 
RenderContext_Vk::onRenderCommand_SwapBuffers(RenderCommand_SwapBuffers* cmd, void* userData)
{
	_shdSwapBuffers = true;		// seems useless

	/*auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);

	auto* vkImgHnd = _vkSwapchain.vkImageHnd();
	vkCmdBuf->cmd_addImageMemBarrier(vkImgHnd, _vkSwapchain.colorFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);*/
}

void 
RenderContext_Vk::onRenderCommand_SetScissorRect(RenderCommand_SetScissorRect* cmd, void* userData)
{
	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);
	vkCmdBuf->setScissor(cmd->rect);
}

void 
RenderContext_Vk::onRenderCommand_SetViewport(RenderCommand_SetViewport* cmd, void* userData)
{
	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);
	vkCmdBuf->setViewport(cmd->rect);
}

//void
//RenderContext_Vk::onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd)
//{
//	_onRenderCommand_DrawCall(_curGraphicsVkCmdBuf, cmd);
//}

void 
RenderContext_Vk::onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd, void* userData)
{
	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);
	_onRenderCommand_DrawCall(vkCmdBuf, cmd);
}

void 
RenderContext_Vk::_onRenderCommand_DrawCall(Vk_CommandBuffer* cmdBuf, RenderCommand_DrawCall* cmd)
{
	auto* vtxLayout = cmd->vertexLayout;
	if (!vtxLayout) { RDS_CORE_ASSERT(false, "drawcall no vertexLayout"); return; }

	auto* vkCmdBufHnd = cmdBuf->hnd();

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
		auto* vkMtlPass = sCast<MaterialPass_Vk*>(pass);

		vkMtlPass->onBind(this, vtxLayout, cmdBuf, cmd->materialFrameIndex());

		for (const auto& e : vkMtlPass->info().allStageUnionInfo.pushConstants)
		{
			using T = PerObjectParam;
			if (!cmd->extraDataSize() || !cmd->extraData())
			{
				continue;
			}
			RDS_CORE_ASSERT(e.size == sizeof(T) && e.size == cmd->extraDataSize(), "invalid push_constant");
			auto& extraData = *reinCast<T*>(cmd->extraData()); RDS_UNUSED(extraData);
			vkCmdPushConstants(vkCmdBufHnd, vkMtlPass->vkPipelineLayout().hnd(), VkShaderStageFlagBits::VK_SHADER_STAGE_ALL, e.offset, e.size, &extraData);
		}
	}
	else
	{
		RDS_THROW("RenderCommand_DrawCall no passes");
	}

	Vk_Buffer_T* vertexBuffers[]	= { vtxBufHndVk };
	VkDeviceSize offsets[]			= { Util::toVkDeviceSize(cmd->vertexOffset) };
	vkCmdBindVertexBuffers(vkCmdBufHnd, 0, 1, vertexBuffers, offsets);

	RDS_VK_INSERT_DEBUG_LABEL(cmdBuf, cmd);

	if (idxCount > 0)
	{
		u32				idxOffset = sCast<u32>(cmd->indexOffset);
		VkIndexType		vkIdxType = Util::toVkIndexType(cmd->indexType);
		vkCmdBindIndexBuffer(vkCmdBufHnd, idxBufVk->vkBuf()->hnd(), idxOffset, vkIdxType);

		vkCmdDrawIndexed(vkCmdBufHnd, idxCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(vkCmdBufHnd, vtxCount, 1, 0, 0);
	}
}

void 
RenderContext_Vk::onRenderCommand_DrawRenderables(RenderCommand_DrawRenderables* cmd, void* userData)
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
			, Vk_RenderPass* renderPass, Vk_Framebuffer* vkFrameBuf, u32 subpassIdx, const Rect2f& framebufferRect2f)
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
		Rect2f					_framebufferRect2f;

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

	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);
	vkCmdBuf->executeSecondaryCmdBufs(vkCmdBufs.span());

	#endif // 0
}

void 
RenderContext_Vk::onRenderCommand_CopyTexture(RenderCommand_CopyTexture* cmd, void* userData)
{
	auto* vkCmdBuf = sCast<Vk_CommandBuffer*>(userData);

	VkImageCopy copyRegion = {};

	auto src = Vk_Texture::getVkImageHnd(cmd->src);
	auto dst = Vk_Texture::getVkImageHnd(cmd->dst);

	copyRegion.extent = Util::toVkExtent3D(cmd->extent);

	copyRegion.srcSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.baseArrayLayer	= cmd->srcLayer;
	copyRegion.srcSubresource.layerCount		= 1;
	copyRegion.srcSubresource.mipLevel			= cmd->srcMip;

	copyRegion.dstSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.baseArrayLayer	= cmd->dstLayer;
	copyRegion.dstSubresource.layerCount		= 1;
	copyRegion.dstSubresource.mipLevel			= cmd->dstMip;

	vkCmdBuf->cmd_copyImage(dst, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegion);
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