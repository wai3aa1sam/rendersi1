#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{


#if 0
#pragma mark --- rdsTransferContext_Vk-Impl ---
#endif // 0
#if 1

TransferContext_Vk::TransferContext_Vk()
{

}

TransferContext_Vk::~TransferContext_Vk()
{
	destroy();
}

void 
TransferContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	_vkTransferFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _vkTransferFrames)
	{
		e.create(this);
	}

	auto* rdDevVk		= renderDeviceVk();

	_vkGraphicsQueue.create(QueueTypeFlags::Graphics, rdDevVk);
	_vkTransferQueue.create(QueueTypeFlags::Transfer, rdDevVk);
	 _vkComputeQueue.create(QueueTypeFlags::Compute,  rdDevVk);
}

void 
TransferContext_Vk::onDestroy()
{
	_vkTransferFrames.clear();
	Base::onDestroy();
}

void 
TransferContext_Vk::onCommit(TransferRequest& tsfReq)
{
	Base::onCommit(tsfReq);

	Span<TransferCommand*> tsfCmds			= tsfReq.transferCommandBuffer().commands();
	Span<TransferCommand*> uploadBufCmds	= tsfReq.uploadBufCmds().commands();
	Span<TransferCommand*> uploadTexCmds	= tsfReq.uploadTexCmds().commands();

	if (tsfCmds.is_empty() && uploadBufCmds.is_empty() && uploadTexCmds.is_empty())
	{
		return;
	}

	auto* rdDevVk		= renderDeviceVk();
	auto& vkTsfFrame	= vkTransferFrame();

	auto& inFlighVkFence	= vkTsfFrame._inFlightVkFnc;
	auto& completedVkSmp	= vkTsfFrame._completedVkSmp;
	auto* vkCmdBuf			= vkTsfFrame.requestTransferCommandBuffer(VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	_curVkCmdBuf = vkCmdBuf;

	vkCmdBuf->beginRecord(&_vkTransferQueue);

	#if 1

	for (auto* cmd : tsfCmds)
	{
		_dispatchCommand(this, cmd);
	}

	for (auto* cmd : uploadBufCmds)
	{
		onTransferCommand_UploadBuffer(sCast<TransferCommand_UploadBuffer*>(cmd));
	}

	for (auto* cmd : uploadTexCmds)
	{
		onTransferCommand_UploadTexture(sCast<TransferCommand_UploadTexture*>(cmd));
	}

	#else
	// record transfer buf
	{
		auto lockedData = transferFrame()._uploadBufReqs.scopedULock();
		auto& data = *lockedData;
		for (auto& e : data)
		{
			vkCmdBuf->cmd_copyBuffer(e.src, e.dst, e.size, 0, 0);
		}
	}

	{
		auto lockedData = transferFrame()._uploadImgReqs.scopedULock();
		auto& data = *lockedData;
		for (auto& e : data)
		{
			vkCmdBuf->cmd_addImageMemBarrier	(e.dst, e.format,	VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkCmdBuf->cmd_copyBufferToImage		(e.dst, e.src,		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, e.size.x, e.size.y);
			vkCmdBuf->cmd_addImageMemBarrier	(e.dst, e.format,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}
	#endif // 1

	vkCmdBuf->endRecord();

	inFlighVkFence.wait(rdDevVk);
	inFlighVkFence.reset(rdDevVk);

	vkCmdBuf->submit(&inFlighVkFence, 
					//&frame._completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, 
					&completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);

	//frame._inFlightVkFence.wait(device());
	_commitUploadCmdsToDstQueue(tsfReq.uploadBufCmds(), tsfReq.uploadTexCmds(), QueueTypeFlags::Graphics);
}

void 
TransferContext_Vk::_commitUploadCmdsToDstQueue(TransferCommandBuffer& bufCmds, TransferCommandBuffer& texCmds, QueueTypeFlags queueType)
{
	auto* rdDevVk		= renderDeviceVk();
	auto& vkTsfFrame	= vkTransferFrame();
	auto* vkCmdBuf		= vkTsfFrame.requestCommandBuffer(queueType);

	//auto& srcInFlighVkFence = frame._inFlightVkFnc;
	auto& srcCompletedVkSmp = vkTsfFrame._completedVkSmp;

	auto& dstInFlighVkFnc	= *vkTsfFrame.requestInFlightVkFnc(queueType);
	auto& dstCompletedVkSmp = *vkTsfFrame.requestCompletedVkSmp(queueType);

	vkCmdBuf->beginRecord(requestVkQueue(queueType));

	for (auto& e : bufCmds.commands())
	{
		RDS_CORE_ASSERT(e->type() == TransferCommandType::UploadBuffer, "");
		auto* cmd = sCast<TransferCommand_UploadBuffer*>(e);
		cmd->dst;
	}

	for (auto& e : texCmds.commands())
	{
		RDS_CORE_ASSERT(e->type() == TransferCommandType::UploadTexture, "");
		auto*	cmd			= sCast<TransferCommand_UploadTexture*>(e);
		auto	vkFormat	= Util::toVkFormat(cmd->dst->format());
		auto*	dst			= Vk_Texture::getImageHnd(cmd->dst);

		vkCmdBuf->cmd_addImageMemBarrier(dst, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferQueueFamilyIdx(), queueFamilyIdx(queueType), false);
	}

	vkCmdBuf->endRecord();

	dstInFlighVkFnc.reset(rdDevVk);

	vkCmdBuf->submit(&dstInFlighVkFnc, 
		&srcCompletedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, 
		&dstCompletedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
}

void 
TransferContext_Vk::onTransferCommand_CopyBuffer	(TransferCommand_CopyBuffer* cmd)
{
	RDS_NOT_YET_SUPPORT();
}

void 
TransferContext_Vk::onTransferCommand_UploadBuffer	(TransferCommand_UploadBuffer* cmd)
{
	RDS_NOT_YET_SUPPORT();
}

void 
TransferContext_Vk::onTransferCommand_UploadTexture(TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd,		"");
	RDS_CORE_ASSERT(cmd->dst,	"");

	RDS_TODO("revisit _hasTransferedGraphicsResoures");
	vkTransferFrame()._hasTransferedGraphicsResoures = true;

	auto* vkCmdBuf		= _curVkCmdBuf;
	auto* statingBufHnd = vkTransferFrame().getVkStagingBufHnd(cmd->_stagingIdx);

	auto*			dst			= Vk_Texture::getImageHnd(cmd->dst);
	const auto&		size		= cmd->dst->size();
	auto			vkFormat	= Util::toVkFormat(cmd->dst->format());

	vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdBuf->cmd_copyBufferToImage		(dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, size.x, size.y);
	vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferQueueFamilyIdx(), graphicsQueueFamilyIdx(), true);
}

Vk_TransferFrame& TransferContext_Vk::vkTransferFrame() { return _vkTransferFrames[renderDeviceVk()->iFrame()]; }

#endif

}

#endif