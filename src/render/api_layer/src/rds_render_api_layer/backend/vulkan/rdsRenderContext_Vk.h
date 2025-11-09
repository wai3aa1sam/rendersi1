#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rds_render_api_layer/command/rdsRenderRequest.h"

#include "rdsVk_GpuProfiler.h"

#include "rdsVk_Allocator.h"
#include "rdsVk_RenderFrame.h"

#include "rdsVk_Swapchain.h"
#include "pass/rdsVk_RenderPassPool.h"
#include "pass/rdsVk_FramebufferPool.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class Vk_RenderGraph;
class RenderJob_Vk;

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

	void onTransferCommand_Create(CmdCreate* cmd);
	void onTransferCommand_Destroy();

public:
	Vk_CommandBuffer* requestCmdBuf_Graphics(StrView debugName, VkCommandBufferLevel bufLevel = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);

public:
	Vk_Queue* vkGraphicsQueue();
	Vk_Queue* vkComputeQueue();
	Vk_Queue* vkTransferQueue();
	Vk_Queue* vkPresentQueue();

	RenderJob_Vk&		renderJob_Vk();
	Vk_RenderFrame&		vkRenderFrame();
	Vk_TransferFrame&	vkTransferFrame();

public:
	// TODO: a marco for declare all those onRenderCommand_XXXX
	void onRenderCommand_Dispatch(RenderCommand_Dispatch*	cmd, void* userData);

	void onRenderCommand_ClearFramebuffers	(RenderCommand_ClearFramebuffers*	cmd, void* userData);
	void onRenderCommand_SwapBuffers		(RenderCommand_SwapBuffers*			cmd, void* userData);
	void onRenderCommand_SetScissorRect		(RenderCommand_SetScissorRect*		cmd, void* userData);
	void onRenderCommand_SetViewport		(RenderCommand_SetViewport*			cmd, void* userData);

	void _onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd, Vk_CommandBuffer* cmdBuf);
	//void onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd);
	void onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd, void* userData);

	void onRenderCommand_DrawRenderables(RenderCommand_DrawRenderables* cmd, void* userData);

	void onRenderCommand_CopyTexture(RenderCommand_CopyTexture* cmd, void* userData);

	void onRenderCommand_DebugLabelBegin(	RenderCommand_DebugLabelBegin*	cmd, void* userData);
	void onRenderCommand_DebugLabelEnd(		RenderCommand_DebugLabelEnd*	cmd, void* userData);
	void onRenderCommand_DebugLabelInsert(	RenderCommand_DebugLabelInsert* cmd, void* userData);

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender() override;
	virtual void onEndRender()	 override;

	virtual void onSetSwapchainSize(const Vec2f& newSize) override;

	virtual void onCommit(RenderCommandBuffer&	renderBuf)	override;
	virtual void onCommit(RenderGraph& rdGraph)				override;
	virtual void onCommit() override;

protected:
	void invalidateSwapchain(VkResult ret, const Vec2f& newSize);

public:
	virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

protected:
	Vk_GpuProfiler	_gpuProfilerCtx;
	Vk_Swapchain	_vkSwapchain;

	Vk_Queue _vkGraphicsQueue;
	Vk_Queue _vkComputeQueue;
	Vk_Queue _vkPresentQueue;
	Vk_Queue _vkTransferQueue;

	//Vk_RenderFrames		_vkRdFrames;
	//Vk_CommandBuffer*	_curGraphicsVkCmdBuf = nullptr;

	bool _shdSwapBuffers				= false;
	bool _hasSwapedBuffersInLastFrame	= true;

	Vk_RenderPassPool	_vkRdPassPool;		// persistent
	//Vk_FramebufferPool	_vkFramebufPool;	// clear when invalidate swapchain

	//Vector<Vk_CommandBuffer_T*, 32> _pendingGfxVkCmdbufHnds;
};

inline Vk_Queue*			RenderContext_Vk::vkGraphicsQueue()		{ return &_vkGraphicsQueue; }
inline Vk_Queue*			RenderContext_Vk::vkComputeQueue()		{ return &_vkComputeQueue; }
inline Vk_Queue*			RenderContext_Vk::vkTransferQueue()		{ return &_vkTransferQueue; }
inline Vk_Queue*			RenderContext_Vk::vkPresentQueue()		{ return &_vkPresentQueue; }

//inline Vk_CommandBuffer_T*	RenderContext_Vk::vkCommandBuffer()		{ return _curGraphicsVkCmdBuf->hnd(); }
//inline Vk_CommandBuffer*	RenderContext_Vk::graphicsVkCmdBuf()	{ return _curGraphicsVkCmdBuf; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


