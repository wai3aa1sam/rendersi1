#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rdsVk_TransferFrame.h"

#include "rds_render_api_layer/transfer/command/rds_transfer_command.h"

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

	void reset(u64 frameCount);

	/*
	* --- Engine Thread only ---
	*/
public:
	void	requestStagingBuf(		StagingHandle& outHnd,	SizeType size);
	void	uploadToStagingBuf(		StagingHandle& outHnd,	ByteSpan data, SizeType offset = 0);
	void*	mappedStagingBufData(	StagingHandle  hnd);
	/*
	* --- Engine Thread only ---
	*/

public:
	//void transitImageLayout(Vk_Image_T* hnd, const Texture_Desc& desc, VkImageLayout srcLayout, VkImageLayout dstLayout, QueueTypeFlags queueType);
	//void transitImageLayout(Vk_Image_T* hnd, const Texture_Desc& desc, VkImageLayout dstLayout, QueueTypeFlags queueType);

public:
	void onTransferCommand_CreateRenderGpuBuffer(	TransferCommand_CreateRenderGpuBuffer*	cmd);
	void onTransferCommand_CreateTexture(			TransferCommand_CreateTexture*			cmd);
	void onTransferCommand_CreateShader(			TransferCommand_CreateShader*			cmd);
	void onTransferCommand_CreateMaterial(			TransferCommand_CreateMaterial*			cmd);
	void onTransferCommand_CreateRenderContext(		TransferCommand_CreateRenderContext*	cmd);
	void onTransferCommand_CreateRenderDevice(		TransferCommand_CreateRenderDevice*		cmd);
	
	void onTransferCommand_DestroyRenderGpuBuffer(	TransferCommand_DestroyRenderGpuBuffer*	cmd);
	void onTransferCommand_DestroyTexture(			TransferCommand_DestroyTexture*			cmd);
	void onTransferCommand_DestroyShader(			TransferCommand_DestroyShader*			cmd);
	void onTransferCommand_DestroyMaterial(			TransferCommand_DestroyMaterial*		cmd);
	void onTransferCommand_DestroyRenderContext(	TransferCommand_DestroyRenderContext*	cmd);
	void onTransferCommand_DestroyRenderDevice(		TransferCommand_DestroyRenderDevice*	cmd);

	void onTransferCommand_SetSwapchainSize(		TransferCommand_SetSwapchainSize*	cmd);

	void onTransferCommand_CopyBuffer(				TransferCommand_CopyBuffer*		cmd);
	void onTransferCommand_UploadBuffer(			TransferCommand_UploadBuffer*	cmd);
	void onTransferCommand_UploadTexture(			TransferCommand_UploadTexture*	cmd);
	
public:
	Vk_TransferFrame& vkTransferFrame(u64 frameIdx);

protected:
	virtual void onCreate(const CreateDesc& cDesc)	override;
	virtual void onDestroy()						override;

	virtual void onCommit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)			override;
	virtual void onCommitRenderResources(TransferCommandBuffer& createQueue, TransferCommandBuffer& destroyQueue)	override;

	Vk_Queue* requestVkQueue(QueueTypeFlags type);

protected:
	void _commitUploadCmdsToDstQueue(const RenderDebugLabel& debugLabel, TransferCommandBuffer& tsfCmdBuf, QueueTypeFlags queueType, bool isWaitImmediate);

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
