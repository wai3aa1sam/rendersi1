#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<RenderGpuBuffer> 
Renderer_Vk::onCreateRenderGpuBuffer(const RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = SPtr<RenderGpuBuffer>(RDS_NEW(RenderGpuBuffer_Vk)());
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
RenderGpuBuffer_Vk::onCreate(const CreateDesc& cDesc)
{
	destroy();

	Base::onCreate(cDesc);

	auto* allocVk	= Renderer_Vk::instance()->memoryContext()->allocVk();

	auto targetSize = math::alignTo(bufSize(), s_kAlign);

	AllocInfo_Vk allocInfo = {};
	allocInfo.usage  = RenderMemoryUsage::GpuOnly;
	Util::createBuffer(_vkBuf, allocVk, &allocInfo, targetSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | Util::toVkBufferUsage(cDesc.typeFlags)
		, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);
}

void
RenderGpuBuffer_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void
RenderGpuBuffer_Vk::onDestroy()
{
	Base::onDestroy();
}

void 
RenderGpuBuffer_Vk::onUploadToGpu(ByteSpan data, SizeType offset)
{
	Base::onUploadToGpu(data, offset);

	//auto* allocVk	= Renderer_Vk::instance()->memoryContext()->allocVk();

	//// TODO: request to upload context, then copy to a bug stage buffer
	//VkPtr<Vk_Buffer>			vkStagingBuf;

	//{
	//	AllocInfo_Vk allocInfo;
	//	allocInfo.usage  = RenderMemoryUsage::CpuOnly;
	//	allocInfo.flags |= RenderAllocFlags::HostWrite;
	//	Util::createBuffer(vkStagingBuf, allocVk, &allocInfo, bufSize()
	//		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	//		, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

	//	void* mappedData = nullptr;
	//	ScopedMemMap_Vk mm = { &mappedData, &vkStagingBuf };
	//	memory_copy(reinCast<u8*>(mappedData) + offset, data.data(), data.size());
	//}

	//Util::copyBuffer(_vkBuf, _vkStagingBuf, bufSize, _vkTransferCommandPool, _vkTransferQueue);
}


#endif
}
#endif