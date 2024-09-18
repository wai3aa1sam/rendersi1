#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTextureCube_Vk.h"

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
TransferContext_Vk::reset(u64 frameCount)
{
	auto frameIdx = Traits::rotateFrame(frameCount);
	vkTransferFrame(frameIdx).clear();
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

	auto* rdDevVk = renderDeviceVk();

	_vkGraphicsQueue.create(QueueTypeFlags::Graphics, rdDevVk);
	_vkTransferQueue.create(QueueTypeFlags::Transfer, rdDevVk);
	 _vkComputeQueue.create(QueueTypeFlags::Compute,  rdDevVk);

	 _setDebugName();
}

void 
TransferContext_Vk::onDestroy()
{
	_vkTransferFrames.clear();
	Base::onDestroy();
}

void 
TransferContext_Vk::onCommit(TransferRequest& tsfReq, bool isWaitImmediate)
{
	Base::onCommit(tsfReq, isWaitImmediate);

	Span<TransferCommand*> tsfCmds			= tsfReq.transferCommandBuffer().commands();
	Span<TransferCommand*> uploadBufCmds	= tsfReq.uploadBufCmds().commands();
	Span<TransferCommand*> uploadTexCmds	= tsfReq.uploadTexCmds().commands();

	if (tsfCmds.is_empty() && uploadBufCmds.is_empty() && uploadTexCmds.is_empty())
	{
		return;
	}

	auto* rdDevVk		= renderDeviceVk();
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	auto& inFlighVkFence	= vkTsfFrame._inFlightVkFnc;
	auto& completedVkSmp	= vkTsfFrame._completedVkSmp;
	auto* vkCmdBuf			= vkTsfFrame.requestTransferCommandBuffer(VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, "TransferContext_Vk::onCommit-vkCmdBuf");

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
		//RDS_LOG("onTransferCommand_UploadBuffer: {}", sCast<TransferCommand_UploadBuffer*>(cmd)->dst->debugName());
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

	RenderDebugLabel debugLabel;
	debugLabel.name = "TransferContext_Vk::onCommit()";
	if (!isWaitImmediate)
	{
		vkCmdBuf->submit(debugLabel, &inFlighVkFence, 
			//&frame._completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, 
			&completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
	}
	else
	{
		vkCmdBuf->submit(debugLabel, &inFlighVkFence);
		inFlighVkFence.wait(rdDevVk);
		
		// ensure the render part wont need to signal the "completedVkSmp"
		vkTsfFrame._hasTransferedGraphicsResoures = false;
		//inFlighVkFence.reset(rdDevVk);
	}

	//frame._inFlightVkFence.wait(device());
	debugLabel.name = "TransferContext_Vk::_commitUploadCmdsToDstQueue()";
	_commitUploadCmdsToDstQueue(debugLabel, tsfReq.uploadBufCmds(), tsfReq.uploadTexCmds(), QueueTypeFlags::Graphics, isWaitImmediate);
}

void 
TransferContext_Vk::_commitUploadCmdsToDstQueue(const RenderDebugLabel& debugLabel, TransferCommandBuffer& bufCmds, TransferCommandBuffer& texCmds, QueueTypeFlags queueType, bool isWaitImmediate)
{
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	if (queueType == QueueTypeFlags::Graphics	&& !vkTsfFrame.hasTransferedGraphicsResoures())	{ return; }
	if (queueType == QueueTypeFlags::Compute	&& !vkTsfFrame.hasTransferedComputeResoures())	{ return; }

	auto* rdDevVk		= renderDeviceVk();
	auto* vkCmdBuf		= vkTsfFrame.requestCommandBuffer(queueType, VK_COMMAND_BUFFER_LEVEL_PRIMARY, RDS_FMT_DEBUG("_commitUploadCmdsToDstQueue-{}", queueType));

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
		RDS_TODO("onTransferCommand_UploadBuffer() and here need to add barrier, but validation layer ignore it? (although the buffer is created with VK_SHARING_MODE_EXCLUSIVE)");
	}

	for (auto& e : texCmds.commands())
	{
		RDS_CORE_ASSERT(e->type() == TransferCommandType::UploadTexture, "");
		auto*	cmd			= sCast<TransferCommand_UploadTexture*>(e);
		auto*	dst			= Vk_Texture::getVkImageHnd(cmd->dst);

		vkCmdBuf->cmd_addImageMemBarrier(dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
										, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
										, cmd->dst->desc(), transferQueueFamilyIdx(), queueFamilyIdx(queueType), false);
	}

	vkCmdBuf->endRecord();

	dstInFlighVkFnc.reset(rdDevVk);

	if (!isWaitImmediate)
	{
		// ("this submit call could put on the last submit queue? but the last submit queue may record many stuffs, submit here maybe faster");
		vkCmdBuf->submit(debugLabel, &dstInFlighVkFnc
			, &srcCompletedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT
			, &dstCompletedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT
		);
	}
	else
	{
		vkCmdBuf->submit(debugLabel, &dstInFlighVkFnc);
		dstInFlighVkFnc.wait(rdDevVk);
	}
}

void 
TransferContext_Vk::requestStagingBuf(StagingHandle& outHnd, SizeType size)
{
	RDS_CORE_ASSERT(size > 0, "");
	auto frameIdx = engineFrameIndex();
	vkTransferFrame(frameIdx).requestStagingHandle(outHnd, size);
}

void 
TransferContext_Vk::uploadToStagingBuf(StagingHandle& outHnd, ByteSpan data, SizeType offset)
{
	RDS_CORE_ASSERT(offset == 0, "not yet supported");
	auto frameIdx = engineFrameIndex();
	vkTransferFrame(frameIdx).uploadToStagingBuf(outHnd, data, offset);
}

void*	
TransferContext_Vk::mappedStagingBufData(StagingHandle  hnd)
{
	auto frameIdx = engineFrameIndex();
	return vkTransferFrame(frameIdx).mappedStagingBufData(hnd);
}

//void 
//TransferContext_Vk::transitImageLayout(Vk_Image_T* hnd, const Texture_Desc& desc, VkImageLayout srcLayout, VkImageLayout dstLayout, QueueTypeFlags srcQueueType)
//{
//	auto& vkTsfFrame	= vkTransferFrame();
//	auto* hndVkCmd		= vkTsfFrame.requestCommandBuffer(srcQueueType, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "TransferContext_Vk::transitImageLayout");
//	auto* srcQueue		= requestVkQueue(srcQueueType);
//
//	Util::transitionImageLayout(hnd, desc, Util::toVkFormat(desc.format), dstLayout, srcLayout, nullptr, srcQueue, hndVkCmd);
//}
//
//void 
//TransferContext_Vk::transitImageLayout(Vk_Image_T* hnd, const Texture_Desc& desc, VkImageLayout dstLayout, QueueTypeFlags queueType)
//{
//	transitImageLayout(hnd, desc, VK_IMAGE_LAYOUT_UNDEFINED, dstLayout, queueType);
//}

void 
TransferContext_Vk::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkGraphicsQueue);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkTransferQueue);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkComputeQueue);
}

#if 0

#endif // 0
#if 1

void 
TransferContext_Vk::onTransferCommand_CopyBuffer	(TransferCommand_CopyBuffer* cmd)
{
	RDS_CORE_ASSERT(cmd->type() == TransferCommandType::CopyBuffer, "");
	RDS_NOT_YET_SUPPORT();
}

void 
TransferContext_Vk::onTransferCommand_UploadBuffer	(TransferCommand_UploadBuffer* cmd)
{
	RDS_CORE_ASSERT(cmd->type() == TransferCommandType::UploadBuffer, "");
	RDS_CORE_ASSERT(cmd->dst, "RenderGpuBuffer_vk is nullptr");
	RDS_CORE_ASSERT(cmd->offset == 0, "not yet supported");

	RDS_TODO("revisit _hasTransferedGraphicsResoures");
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);
	vkTsfFrame._hasTransferedGraphicsResoures = true;

	cmd->_stagingHnd.checkValid();

	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(cmd->dst.ptr());
	auto* dstBufHnd		= dstBuf->vkBufHnd();
	//auto* statingBufHnd = vkTransferFrame().getVkStagingBufHnd(cmd->_stagingIdx);
	auto* statingBufHnd = vkTsfFrame.getVkStagingBufHnd(cmd->_stagingHnd);

	RDS_CORE_ASSERT(dstBuf && dstBufHnd && statingBufHnd, "buffer is nullptr");

	const auto size = Util::toVkDeviceSize(cmd->data.size());
	RDS_CORE_ASSERT(size > 0 && size <= cmd->dst->bufSize(), "overflow");

	auto* vkCmdBuf = _curVkCmdBuf;
	vkCmdBuf->cmd_copyBuffer(dstBufHnd, statingBufHnd, size, 0, cmd->_stagingHnd.offset);
}

void 
TransferContext_Vk::onTransferCommand_UploadTexture(TransferCommand_UploadTexture* cmd)
{
	RDS_CORE_ASSERT(cmd->type() == TransferCommandType::UploadTexture, "");
	RDS_CORE_ASSERT(cmd,		"");
	RDS_CORE_ASSERT(cmd->dst,	"");

	RDS_TODO("revisit _hasTransferedGraphicsResoures");
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);
	vkTsfFrame._hasTransferedGraphicsResoures = true;

	cmd->_stagingHnd.checkValid();

	auto* vkCmdBuf		= _curVkCmdBuf;
	//auto* statingBufHnd = vkTransferFrame().getVkStagingBufHnd(cmd->_stagingIdx);
	auto* statingBufHnd = vkTsfFrame.getVkStagingBufHnd(cmd->_stagingHnd);

	auto*			dst			= Vk_Texture::getVkImageHnd(cmd->dst);
	const auto&		size		= cmd->dst->size();
	auto			vkFormat	= Util::toVkFormat(cmd->dst->format());

	switch (cmd->dst->type())
	{
		case RenderDataType::Texture2D:
		{
			vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkCmdBuf->cmd_copyBufferToImage		(dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, size.x, size.y, cmd->_stagingHnd.offset);
			vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferQueueFamilyIdx(), graphicsQueueFamilyIdx(), true);
		} break;

		case RenderDataType::TextureCube:
		{
			using CopyToTextureCubeDesc = Vector<VkBufferImageCopy, TextureCube::s_kFaceCount>;
			CopyToTextureCubeDesc cpyDescs;

			auto& texDesc = cmd->dst->desc();

			VkDeviceSize offset = cmd->_stagingHnd.offset;
			for (u32 face = 0; face < TextureCube::s_kFaceCount; face++)
			{
				for (u32 mip = 0; mip < texDesc.mipCount; mip++)
				{
					auto& cpyDesc = cpyDescs.emplace_back();
					cpyDesc.bufferOffset		= offset;
					cpyDesc.bufferRowLength		= 0;
					cpyDesc.bufferImageHeight	= 0;
					cpyDesc.imageExtent			= VkExtent3D {cmd->dst->size().x, cmd->dst->size().y, cmd->dst->size().z};
					cpyDesc.imageOffset			= VkOffset3D {0, 0, 0};

					cpyDesc.imageSubresource = Util::toVkImageSubresourceLayers(texDesc, mip, face, 1);
					offset += texDesc.totalByteSize();
				}
			}

			vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, 1, 0, TextureCube::s_kFaceCount);
			vkCmdBuf->cmd_copyBufferToImage		(dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cpyDescs);
			vkCmdBuf->cmd_addImageMemBarrier	(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
												, 0, 1, 0, TextureCube::s_kFaceCount, transferQueueFamilyIdx(), graphicsQueueFamilyIdx(), true);
		} break;

		default: { throwIf(true, "not yet supprot"); } break;
	}
}


#endif // 1

Vk_TransferFrame& TransferContext_Vk::vkTransferFrame(u64 frameIdx) { return _vkTransferFrames[frameIdx]; }

#endif

}

#endif