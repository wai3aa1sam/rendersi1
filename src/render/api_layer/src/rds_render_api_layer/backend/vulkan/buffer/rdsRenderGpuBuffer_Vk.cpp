#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/command/rdsTransferRequest.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<RenderGpuBuffer> 
RenderDevice_Vk::onCreateRenderGpuBuffer(RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = SPtr<RenderGpuBuffer>(makeSPtr<RenderGpuBuffer_Vk>());
	p->create(cDesc);
	return p;
}

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
	destroy();
}

void 
RenderGpuBuffer_Vk::setDebugName(StrView name)
{
	Base::setDebugName(name);
	RDS_VK_SET_DEBUG_NAME(_vkBuf, name);
}

void
RenderGpuBuffer_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	
	auto* rdDevVk	= renderDeviceVk();
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	auto targetSize = math::alignTo(bufSize(), s_kAlign);

	RDS_TODO("if it is const buf, no need staging");

	Vk_AllocInfo allocInfo = {};

	VkBufferUsageFlags usageFlags = Util::toVkBufferUsage(cDesc.typeFlags);
	if (cDesc.typeFlags != RenderGpuBufferTypeFlags::Const)
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

	RDS_VK_SET_DEBUG_NAME_SRCLOC(_vkBuf);
}

void
RenderGpuBuffer_Vk::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void
RenderGpuBuffer_Vk::onDestroy()
{
	Base::onDestroy();
}

void 
RenderGpuBuffer_Vk::onUploadToGpu(TransferCommand_UploadBuffer* cmd)
{
	Base::onUploadToGpu(cmd);

	// or create each time, but it has to handle destroy, cannot destroy it when using
	throwIf(!_vkBuf, "RenderGpuBuffer not yet created");

	RDS_TODO("revisit, if the data is large, may be put to other thread");
	RDS_TODO("should have a inline staging buffer, ");

	if (BitUtil::has(typeFlags(), RenderGpuBufferTypeFlags::Const))
	{
		auto memmap =_vkBuf.scopedMemMap();
		memory_copy(memmap.data<u8*>(), cmd->data.data(), cmd->data.size());

		if (cmd->parent)
		{
			RDS_THROW("has bug here, seems should rotate now");
		}
	}
	else
	{
		transferContextVk().uploadToStagingBuf(cmd->_stagingHnd, cmd->data);
		//transferContextVk().uploadToStagingBuf(cmd->_stagingIdx, cmd->data);
	}
}



#endif
}
#endif