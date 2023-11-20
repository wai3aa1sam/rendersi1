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

	Vk_TransferFrame& vkTransferFrame();

public:
	void onTransferCommand_CopyBuffer	(TransferCommand_CopyBuffer*	cmd);
	void onTransferCommand_UploadBuffer	(TransferCommand_UploadBuffer*	cmd);
	void onTransferCommand_UploadTexture(TransferCommand_UploadTexture* cmd);

protected:
	virtual void onCreate	(const CreateDesc& cDesc)	override;
	virtual void onDestroy	()							override;

	virtual void onCommit(TransferRequest& tsfReq) override;

	Vk_Queue* requestVkQueue(QueueTypeFlags type);

protected:
	void _commitUploadCmdsToDstQueue(TransferCommandBuffer& bufCmds, TransferCommandBuffer& texCmds, QueueTypeFlags queueType);

	void _setDebugName();

protected:
	Vk_TransferFrames	_vkTransferFrames;

	Vk_Queue _vkGraphicsQueue;
	Vk_Queue _vkTransferQueue;
	Vk_Queue _vkComputeQueue;

	Vk_CommandBuffer* _curVkCmdBuf = nullptr;
};


inline 
Vk_Queue* 
TransferContext_Vk::requestVkQueue(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return &_vkGraphicsQueue; } break;
		case SRC::Transfer: { return &_vkTransferQueue; } break;
		case SRC::Compute:	{ return &_vkComputeQueue; } break;
		default: { RDS_THROW(""); }
	}
}

#endif

}

#endif
