#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuBuffer_Vk-Impl ---
#endif // 0
#if 1

RenderGpuBuffer_Vk::RenderGpuBuffer_Vk()
	: Base()
{

}

RenderGpuBuffer_Vk::~RenderGpuBuffer_Vk()
{
	
}

void 
RenderGpuBuffer_Vk::onTransferCommand_Create(CmdCreate* cmd)
{
	auto* rdDevVk	= renderDeviceVk();
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	RDS_TODO("do not call the stuff in Engine side, although it is immutable");
	auto targetSize = math::alignTo(cmd->cDesc.bufSize, s_kAlign);

	Vk_AllocInfo allocInfo = {};

	RenderGpuBufferTypeFlags	bufTypeFlags	= typeFlags();
	VkBufferUsageFlags			usageFlags		= Util::toVkBufferUsages(bufTypeFlags);
	if (!isConstantBuffer())
	{
		usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	else
	{
		allocInfo.flags |= RenderAllocFlags::HostWrite;
	}

	_vkBuf.create(rdDevVk, vkAlloc, &allocInfo, targetSize
		, usageFlags
		, QueueTypeFlags::Graphics);

	VkBufferDeviceAddressInfo address_info = {};
	address_info.sType	= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
	address_info.buffer = vkBufHnd();
	_gpuAddress = sCast<u64>(vkGetBufferDeviceAddress(rdDevVk->vkDevice(), &address_info));

	RDS_TODO("TransferCommand_setDebugName");
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkBuf);

	renderDeviceVk()->bindlessResourceVk().onCommit_RenderGpuBuffer(this);
}

void 
RenderGpuBuffer_Vk::onTransferCommand_Destroy()
{
	_vkBuf.destroy();
	_gpuAddress = 0;
}

void 
RenderGpuBuffer_Vk::onUploadToGpu(TransferCommand_UploadBuffer* cmd)
{
	Base::onUploadToGpu(cmd);
	
	auto& tsfFrameVk	= transferFrameVk(); // rdDev->transferFrame(rdDev->engineFrameIndex());
	if (isConstantBuffer())
	{
		tsfFrameVk.constBufferAllocator().uploadToBuffer(cmd->_stagingHnd, cmd->data);
	}
	else
	{
		tsfFrameVk.uploadToStagingBuf(cmd->_stagingHnd, cmd->data);
	}
}

void 
RenderGpuBuffer_Vk::onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd)
{
	auto& name = cmd->name;
	Base::onRenderResouce_SetDebugName(cmd);
	//RDS_LOG_ERROR("onRenderResouce_SetDebugName: {}", name);
	RDS_VK_SET_DEBUG_NAME(_vkBuf, name);
}


#endif

}
#endif