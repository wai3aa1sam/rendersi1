#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rds_vk_texture.h"
#include "rds_render_api_layer/backend/vulkan/transfer/rdsTransferFrame_Vk.h"

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
	RDS_TODO("this design is bug, should explicitly know thread ownership, resource upload should have its own allocator");
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
TransferContext_Vk::onTransferBegin()
{
	Base::onTransferBegin();

	auto& rdFrameParam = renderFrameParam();
	//waitFrameFinished(rdFrameParam);
	reset(rdFrameParam.frameCount());

	// should not use reset
	// separate the reset() to reset(Type type) and to wait(Type type)
	// _commitUploadCmdsToDstQueue its own fence could wait then reset
}

void 
TransferContext_Vk::onTransferEnd()
{
	Base::onTransferEnd();

}

void 
TransferContext_Vk::onCommit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)
{
	Base::onCommit(rdFrameParam, tsfReq, isWaitImmediate);

	auto* rdDevVk		= renderDeviceVk();
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	/*
	* no wait here then must follow TransferFrame pattern
	*/
	auto& vkQueueData		= vkTsfFrame.getVkQueueData(QueueTypeFlags::Transfer);
	vkTsfFrame.waitAndResetQueueData(QueueTypeFlags::Transfer);	// no want to want then may use same method in TransferFrame, but seems on9, just keep simple

	auto lock = tsfReq.transferCommandBuffer().scopedULock();		// maybe swap to a local, then no need to lock too long, but only this own it now
	auto& tsfCmdBuf = *lock;
	Span<TransferCommand*> tsfCmds = tsfCmdBuf.commands();

	if (tsfCmds.is_empty())
	{
		return;
	}

	auto& inFlightVkFnc		= vkQueueData.inFlightVkFence;
	auto& completedVkSmp	= vkQueueData.completedVkSemaphore;
	auto* vkCmdBuf			= vkTsfFrame.requestTransferCommandBuffer(VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, "TransferContext_Vk::onCommit-vkCmdBuf");

	_curVkCmdBuf = vkCmdBuf;
	vkCmdBuf->beginRecord(&_vkTransferQueue);

	for (auto* cmd : tsfCmds)
	{
		_dispatchCommand(this, cmd);
	}

	vkCmdBuf->endRecord();
	
	// if wait in here, then must follow TransferFrame pattern, instead of s_kFrameInFlightCount
	RDS_TODO("must wait last submit here, since same frame index gpu may using the resources, currently waited in Engine so it is safe now");
	RDS_TODO("btw need revisit vulkan barrier, will diff submit being pick and have race condition?");
	RDS_TODO("revisit _hasTransferedGraphicsResoures");

	RenderDebugLabel debugLabel;
	debugLabel.name = "TransferContext_Vk::onCommit()";
	if (!isWaitImmediate)
	{
		vkCmdBuf->submit(debugLabel, &inFlightVkFnc, 
			//&frame._completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, 
			&completedVkSmp, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
		vkTsfFrame._hasTransferedGraphicsResoures = true;
	}
	else
	{
		vkCmdBuf->submit(debugLabel, &inFlightVkFnc);
		inFlightVkFnc.wait(rdDevVk);
		
		// ensure the render part wont need to signal the "completedVkSmp"
		vkTsfFrame._hasTransferedGraphicsResoures = false;
		//inFlighVkFence.reset(rdDevVk);
	}

	//frame._inFlightVkFence.wait(device());
	debugLabel.name = "TransferContext_Vk::_commitUploadCmdsToDstQueue()";
	_commitUploadCmdsToDstQueue(debugLabel, tsfCmdBuf, QueueTypeFlags::Graphics, isWaitImmediate);
}

void 
TransferContext_Vk::_commitUploadCmdsToDstQueue(const RenderDebugLabel& debugLabel, TransferCommandBuffer& tsfCmdBuf, QueueTypeFlags queueType, bool isWaitImmediate)
{
	auto  frameIdx		= frameIndex();
	auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	if (BitUtil::has(queueType, QueueTypeFlags::Graphics)	&& !vkTsfFrame.hasTransferedGraphicsResoures())	{ return; }
	if (BitUtil::has(queueType, QueueTypeFlags::Compute)	&& !vkTsfFrame.hasTransferedComputeResoures())	{ return; }

	auto* rdDevVk		= renderDeviceVk();
	auto* vkCmdBuf		= vkTsfFrame.requestCommandBuffer(queueType, VK_COMMAND_BUFFER_LEVEL_PRIMARY, RDS_FMT_DEBUG("_commitUploadCmdsToDstQueue-{}", queueType));

	//auto& srcInFlighVkFence = frame._inFlightVkFnc;
	auto& srcVkQueueData	= vkTsfFrame.getVkQueueData(QueueTypeFlags::Transfer);
	auto& srcCompletedVkSmp = srcVkQueueData.completedVkSemaphore;

	auto& dstVkQueueData	= vkTsfFrame.getVkQueueData(queueType);
	vkTsfFrame.waitAndResetQueueData(queueType);

	auto& dstInFlighVkFnc	= dstVkQueueData.inFlightVkFence;
	auto& dstCompletedVkSmp	= dstVkQueueData.completedVkSemaphore;

	vkCmdBuf->beginRecord(requestVkQueue(queueType));
	
	for (auto& e : tsfCmdBuf.commands())
	{
		using SRC = TransferCommandType;
		switch (e->type())
		{
			case SRC::UploadBuffer:		{ RDS_TODO("onTransferCommand_UploadBuffer() and here need to add barrier, but validation layer ignore it? (although the buffer is created with VK_SHARING_MODE_EXCLUSIVE)"); } break;
			case SRC::UploadTexture:	
			{
			
				auto*	cmd			= sCast<TransferCommand_UploadTexture*>(e);
				auto*	dst			= Vk_Texture::getVkImageHnd(cmd->dst);

				vkCmdBuf->cmd_addImageMemBarrier(dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, cmd->dst->desc(), transferQueueFamilyIdx(), queueFamilyIdx(queueType), false);

			} break;
		}
	}

	vkCmdBuf->endRecord();

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
TransferContext_Vk::onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate)
{
	if (renderDevice() && isProcessCreate)
	{
		renderDeviceVk()->bindlessResourceVk().reserve(rscQueue.commands().size());
	}

	_dispatchCommands(this, rscQueue);
}

void 
TransferContext_Vk::_setDebugName()
{
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkGraphicsQueue);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkTransferQueue);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkComputeQueue);
}

#if 1

void 
TransferContext_Vk::onTransferCommand_CreateRenderGpuBuffer(TransferCommand_CreateRenderGpuBuffer* cmd)
{
	auto* rdDevVk		= renderDeviceVk();
	auto& bindlessRscVk = rdDevVk->bindlessResourceVk();
	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(cmd->dst.ptr());

	dstBuf->createRenderResource(rdDevVk->renderFrameParam());
	bindlessRscVk.onCommit_RenderGpuBuffer(dstBuf);
}

void 
TransferContext_Vk::onTransferCommand_CreateTexture(TransferCommand_CreateTexture* cmd)
{
	using SRC = RenderDataType;
	auto* rdDevVk		= renderDeviceVk();
	auto& bindlessRscVk = rdDevVk->bindlessResourceVk();
	auto* dstTex		= cmd->dst.ptr();

	RDS_VK_TEXTURE_INVOKE(dstTex, createRenderResource(rdDevVk->renderFrameParam()));
	bindlessRscVk.onCommit_Texture(dstTex);
}

void 
TransferContext_Vk::onTransferCommand_CreateShader(TransferCommand_CreateShader* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_CreateMaterial(TransferCommand_CreateMaterial* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_CreateRenderContext(TransferCommand_CreateRenderContext* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_CreateRenderDevice(TransferCommand_CreateRenderDevice* cmd)
{

}

void
TransferContext_Vk::onTransferCommand_DestroyRenderGpuBuffer(TransferCommand_DestroyRenderGpuBuffer* cmd)
{
	auto* rdDevVk		= renderDeviceVk();
	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(cmd->dst);

	RenderResource::destroyObject(dstBuf, rdDevVk->renderFrameParam());
}

void 
TransferContext_Vk::onTransferCommand_DestroyTexture(TransferCommand_DestroyTexture* cmd)
{
	auto* rdDevVk		= renderDeviceVk();
	auto* dstTex		= cmd->dst;

	RDS_VK_TEXTURE_INVOKE(dstTex, destroyRenderResource(rdDevVk->renderFrameParam()));
	RenderResource::destroyObject(dstTex, rdDevVk->renderFrameParam());
}

void 
TransferContext_Vk::onTransferCommand_DestroyShader(TransferCommand_DestroyShader* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_DestroyMaterial(TransferCommand_DestroyMaterial* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_DestroyRenderContext(TransferCommand_DestroyRenderContext* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_DestroyRenderDevice(TransferCommand_DestroyRenderDevice* cmd)
{

}

void 
TransferContext_Vk::onTransferCommand_SetDebugName(TransferCommand_SetDebugName* cmd)
{
	auto* dst = cmd->dst.ptr();
	dst->onRenderResouce_SetDebugName(cmd);
}

void 
TransferContext_Vk::onTransferCommand_SetSwapchainSize(TransferCommand_SetSwapchainSize* cmd)
{
	auto* rdCtx = cmd->renderContext.ptr();
	rdCtx->onSetSwapchainSize(cmd->size);
}

void
TransferContext_Vk::onTransferCommand_CopyBuffer(TransferCommand_CopyBuffer* cmd)
{
	RDS_CORE_ASSERT(cmd->type() == TransferCommandType::CopyBuffer, "");
	RDS_NOT_YET_SUPPORT();
}

void 
TransferContext_Vk::onTransferCommand_UploadBuffer(TransferCommand_UploadBuffer* cmd)
{
	//auto  frameIdx		= frameIndex();
	//auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	auto* vkCmdBuf		= _curVkCmdBuf;
	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(cmd->dst.ptr());
	auto* dstBufHnd		= dstBuf->vkBufHnd();
	const auto size		= Util::toVkDeviceSize(cmd->data.size());

	cmd->_stagingHnd.checkValid();
	RDS_CORE_ASSERT(cmd->dst,									"RenderGpuBuffer_vk is nullptr");
	RDS_CORE_ASSERT(cmd->offset == 0,							"not yet supported");
	RDS_CORE_ASSERT(dstBuf && dstBufHnd,						"buffer is nullptr");
	RDS_CORE_ASSERT(size > 0 && size <= cmd->dst->bufSize(),	"overflow");

	auto& tsfFrameVk	= transferFrameVk();
	if (dstBuf->isConstantBuffer())
	{
		auto memmap = dstBuf->vkBuf()->scopedMemMap();
		auto& constBufAlloc = tsfFrameVk.constBufferAllocator();
		constBufAlloc.uploadToDst(memmap.data<u8*>(), cmd->_stagingHnd, size);
	}
	else
	{
		//auto* statingBufHnd = vkTransferFrame().getVkStagingBufHnd(cmd->_stagingIdx);
		auto* statingBufHnd = tsfFrameVk.getVkStagingBufHnd(cmd->_stagingHnd);
		RDS_CORE_ASSERT(statingBufHnd,		"buffer is nullptr");

		vkCmdBuf->cmd_copyBuffer(dstBufHnd, statingBufHnd, size, 0, cmd->_stagingHnd.offset);
	}
}

void 
TransferContext_Vk::onTransferCommand_UploadTexture(TransferCommand_UploadTexture* cmd)
{
	//auto  frameIdx		= frameIndex();
	//auto& vkTsfFrame	= vkTransferFrame(frameIdx);

	cmd->_stagingHnd.checkValid();

	auto* vkCmdBuf		= _curVkCmdBuf;

	auto& tsfFrameVk	= transferFrameVk();
	auto* statingBufHnd = tsfFrameVk.getVkStagingBufHnd(cmd->_stagingHnd);

	Texture*		dstTex		= cmd->dst;
	auto*			dst			= Vk_Texture::getVkImageHnd(dstTex);
	const auto&		size		= dstTex->size();
	auto			vkFormat	= Util::toVkFormat(dstTex->format());

	RDS_CORE_ASSERT(cmd,		"");
	RDS_CORE_ASSERT(cmd->dst,	"");
	RDS_CORE_ASSERT(dst,		"");

	switch (dstTex->type())
	{
		case RenderDataType::Texture2D:
		{
			vkCmdBuf->cmd_addImageMemBarrier(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkCmdBuf->cmd_copyBufferToImage( dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, size.x, size.y, cmd->_stagingHnd.offset);
			vkCmdBuf->cmd_addImageMemBarrier(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferQueueFamilyIdx(), graphicsQueueFamilyIdx(), true);
		} break;

		case RenderDataType::TextureCube:
		{
			using CopyToTextureCubeDesc = Vector<VkBufferImageCopy, TextureCube::s_kFaceCount>;
			CopyToTextureCubeDesc cpyDescs;

			auto& texDesc = dstTex->desc();

			VkDeviceSize offset = cmd->_stagingHnd.offset;
			for (u32 face = 0; face < TextureCube::s_kFaceCount; face++)
			{
				for (u32 mip = 0; mip < texDesc.mipCount; mip++)
				{
					auto& cpyDesc = cpyDescs.emplace_back();
					cpyDesc.bufferOffset		= offset;
					cpyDesc.bufferRowLength		= 0;
					cpyDesc.bufferImageHeight	= 0;
					cpyDesc.imageExtent			= VkExtent3D {dstTex->size().x, dstTex->size().y, dstTex->size().z};
					cpyDesc.imageOffset			= VkOffset3D {0, 0, 0};

					cpyDesc.imageSubresource = Util::toVkImageSubresourceLayers(texDesc, mip, face, 1);
					offset += texDesc.totalByteSize();
				}
			}

			vkCmdBuf->cmd_addImageMemBarrier(dst, vkFormat,			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, 1, 0, TextureCube::s_kFaceCount);
			vkCmdBuf->cmd_copyBufferToImage( dst, statingBufHnd,	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cpyDescs);
			vkCmdBuf->cmd_addImageMemBarrier(dst, vkFormat,			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
												, 0, 1, 0, TextureCube::s_kFaceCount, transferQueueFamilyIdx(), graphicsQueueFamilyIdx(), true);
		} break;

		default: { throwError("not yet support"); } break;
	}
}


#endif // 1

Vk_TransferFrame& TransferContext_Vk::vkTransferFrame(u64 frameIdx) { return _vkTransferFrames[frameIdx]; }
TransferFrame_Vk& TransferContext_Vk::transferFrameVk()				{ return sCast<TransferFrame_Vk&>(TransferContext::transferFrame()); }


#endif

}

#endif