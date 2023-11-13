#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rdsVk_TransferFrame.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsTransferContext_Vk-Decl ---
#endif // 0
#if 1

class TransferContext_Vk : public RenderResource_Vk<TransferContext>
{
public:
	using Vk_TransferFrames = Vector<Vk_TransferFrame, s_kFrameInFlightCount>;

public:
	TransferContext_Vk();
	~TransferContext_Vk();

	Vk_TransferFrame& transferFrame();

public:
	void onTransferCommand_CopyBuffer	(TransferCommand_CopyBuffer*	cmd);
	void onTransferCommand_UploadBuffer	(TransferCommand_UploadBuffer*	cmd);
	void onTransferCommand_UploadTexture(TransferCommand_UploadTexture* cmd);

protected:
	virtual void onCreate()		override;
	virtual void onDestroy()	override;

	virtual void onCommit(TransferCommandBuffer& cmdBuf) override;


protected:
	Vk_TransferFrames	_vkTransferFrames;
	Vk_Queue			_vkTransferQueue;

	Vk_CommandBuffer* _curVkCmdBuf = nullptr;
};

inline Vk_TransferFrame& TransferContext_Vk::transferFrame() { return _vkTransferFrames[iFrame()]; }

#endif

}

#endif
