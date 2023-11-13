#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

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
TransferContext_Vk::onCreate()
{
	Base::onCreate();

	_vkTransferFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _vkTransferFrames)
	{
		e.create(this);
	}

	_vkTransferQueue.create(QueueTypeFlags::Transfer, device());
}

void 
TransferContext_Vk::onDestroy()
{
	Base::onDestroy();

	for (auto& e : _vkTransferFrames)
	{
		e.destroy(this);
	}
	_vkTransferFrames.clear();
	_vkTransferQueue.destroy();
}

void 
TransferContext_Vk::onCommit(TransferCommandBuffer& cmdBuf)
{
	Base::onCommit(cmdBuf);

	if (cmdBuf.commands().is_empty())
	{
		return;
	}

	auto* rdDev = device();
	auto& frame = transferFrame();

	auto& cmdPool	= frame._vkCommandPool;
	auto* vkCmdBuf	= cmdPool.requestCommandBuffer(VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	_curVkCmdBuf = vkCmdBuf;

	vkCmdBuf->beginRecord(&_vkTransferQueue);

	#if 1

	for (auto& cmd : cmdBuf.commands())
	{
		_dispatchCommand(this, cmd);
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

	frame._inFlightVkFence.reset(rdDev);

	vkCmdBuf->submit(&frame._inFlightVkFence, 
					//&frame._completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, 
					&frame._completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);

	
	//frame._inFlightVkFence.wait(device());
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

	auto* vkCmdBuf		= _curVkCmdBuf;
	auto* statingBufHnd = transferFrame().getVkStagingBufHnd(cmd->_stagingIdx);

	auto*			dst			= Vk_Texture::getImageHnd(cmd->dst);
	const auto&		size		= cmd->dst->size();
	auto			vkFormat	= Util::toVkFormat(cmd->dst->format());

	auto tranferFamilyIdx	= device()->queueFamilyIndices().getFamilyIdx(QueueTypeFlags::Transfer);
	auto graphicsFamilyIdx	= device()->queueFamilyIndices().getFamilyIdx(QueueTypeFlags::Graphics);

	vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdBuf->cmd_copyBufferToImage		(dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, size.x, size.y);
	vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, tranferFamilyIdx, graphicsFamilyIdx, true);
}

#endif

}

#endif