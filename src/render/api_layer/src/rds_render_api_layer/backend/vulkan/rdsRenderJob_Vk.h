#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderFrame.h"
#include "rds_render_api_layer/backend/vulkan/transfer/rdsTransferFrame_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class RenderContext_Vk;

#if 0
#pragma mark --- rdsRenderJob_Vk-Decl ---
#endif // 0
#if 1

class RenderJob_Vk : public RenderJob // RenderResource_Vk<RenderJob>
{
public:
	using Base = RenderJob; // RenderResource_Vk<RenderJob>;
	using Vk_CmdBufHnds = Vector<Vk_CommandBuffer_T*, 32>;

public:
	RenderJob_Vk();
	virtual ~RenderJob_Vk();

public:
	void reset(RenderDevice_Vk* rdDevVk);

public:
	Vk_CommandBuffer* requestCommandBuffer(Vk_Queue& vk_queue, VkCommandBufferLevel bufLevel, StrView debugName);

public:
	Vk_RenderFrame&		vkRenderFrame();
	Vk_CmdBufHnds&		pendingGfxVkCmdbufHnds();

	RenderDevice_Vk*	renderDeviceVk();
	Vk_TransferFrame&	vkTransferFrame();
	TransferFrame_Vk&	transferFrameVk();

protected:
	virtual void onCreate(CreateDesc& cDesc)			override;
	virtual void onDestroy()							override;
	virtual void onReset(RenderContext* rdCtx)			override;

	virtual bool onCheckUploadCompleted() override;
	virtual bool onCheckRenderCompleted() override;

public:
	//virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

protected:
	Vk_RenderFrame	_vk_rdFrame;
	Vk_CmdBufHnds	_pendingGfxVkCmdbufHnds;
};

#endif

inline Vk_RenderFrame&				RenderJob_Vk::vkRenderFrame()			{ return _vk_rdFrame; }
inline RenderJob_Vk::Vk_CmdBufHnds&	RenderJob_Vk::pendingGfxVkCmdbufHnds()	{ return _pendingGfxVkCmdbufHnds; }

inline RenderDevice_Vk*				RenderJob_Vk::renderDeviceVk()			{ return transferFrameVk().renderDeviceVk(); }
inline Vk_TransferFrame&			RenderJob_Vk::vkTransferFrame()			{ return transferFrameVk().vkTransferFrame(); }
inline TransferFrame_Vk&			RenderJob_Vk::transferFrameVk()			{ return sCast<TransferFrame_Vk&>(*_transferFrame); }

}
#endif