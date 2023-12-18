#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rds_render_api_layer/command/rdsRenderRequest.h"

#include "rdsGpuProfilerContext_Vk.h"

#include "rdsVk_Allocator.h"
#include "rdsVk_RenderFrame.h"

#include "rdsVk_Swapchain.h"
#include "pass/rdsVk_RenderPassPool.h"
#include "pass/rdsVk_FramebufferPool.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class Vk_RenderGraph;

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderResource_Vk<RenderContext>
{
	friend class Vk_RenderGraph;
public:
	using Base				= RenderResource_Vk<RenderContext>;
	using Vk_RenderFrames	= Vector<Vk_RenderFrame, s_kFrameInFlightCount>;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Vk_Queue* vkGraphicsQueue();
	Vk_Queue* vkTransferQueue();
	Vk_Queue* vkPresentQueue();

	Vk_CommandBuffer_T* vkCommandBuffer();
	Vk_CommandBuffer*	graphicsVkCmdBuf();

	Vk_RenderFrame& vkRdFrame();

public:
	void onRenderCommand_ClearFramebuffers(RenderCommand_ClearFramebuffers* cmd);
	void onRenderCommand_SwapBuffers(RenderCommand_SwapBuffers* cmd);
	void onRenderCommand_SetScissorRect(RenderCommand_SetScissorRect* cmd);

	void onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd);
	void _onRenderCommand_DrawCall(Vk_CommandBuffer* cmdBuf, RenderCommand_DrawCall* cmd);

	void onRenderCommand_DrawRenderables(RenderCommand_DrawRenderables* cmd);

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender() override;
	virtual void onEndRender()	 override;

	virtual void onSetFramebufferSize(const Vec2f& newSize) override;

	virtual void onCommit(RenderCommandBuffer& renderBuf) override;
	virtual void onCommit(RenderGraph&			rdGraph)  override;

	void test_extraDrawCall(RenderCommandBuffer& renderCmdBuf);

protected:
	void beginRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline);

	void beginRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRenderPass	(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);

	void invalidateSwapchain(VkResult ret, const Vec2f& newSize);

	Vk_CommandBuffer* requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName);

	void _setDebugName();

protected:
	void createTestRenderPass(Vk_Swapchain_CreateDesc& vkSwapchainCDesc);

protected:
	RDS_PROFILE_GPU_CTX_VK(_gpuProfilerCtx);

	Vk_Swapchain	_vkSwapchain;
	Vk_RenderPass	_testVkRenderPass;

	Vk_Queue _vkGraphicsQueue;
	Vk_Queue _vkPresentQueue;
	Vk_Queue _vkTransferQueue;

	Vk_RenderFrames		_vkRdFrames;
	Vk_CommandBuffer*	_curGraphicsVkCmdBuf = nullptr;

	bool _shdSwapBuffers = false;

	Vk_RenderPassPool	_vkRdPassPool;		// persistent
	Vk_FramebufferPool	_vkFramebufPool;	// clear when invalidate swapchain
};

inline Vk_Queue* RenderContext_Vk::vkGraphicsQueue()	{ return &_vkGraphicsQueue; }
inline Vk_Queue* RenderContext_Vk::vkTransferQueue()	{ return &_vkTransferQueue; }
inline Vk_Queue* RenderContext_Vk::vkPresentQueue()		{ return &_vkPresentQueue; }


inline Vk_CommandBuffer_T*	RenderContext_Vk::vkCommandBuffer()		{ return _curGraphicsVkCmdBuf->hnd(); }
inline Vk_CommandBuffer*	RenderContext_Vk::graphicsVkCmdBuf()	{ return _curGraphicsVkCmdBuf; }

inline Vk_RenderFrame& RenderContext_Vk::vkRdFrame() { return _vkRdFrames[_curFrameIdx]; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


