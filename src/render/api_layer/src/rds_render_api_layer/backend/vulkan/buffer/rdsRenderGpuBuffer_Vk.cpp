#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

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
RenderGpuBuffer_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	
	auto* rdDevVk	= renderDeviceVk();
	auto* vkAlloc	= rdDevVk->memoryContext()->vkAlloc();

	auto targetSize = math::alignTo(bufSize(), s_kAlign);
	
	Vk_AllocInfo allocInfo = {};
	allocInfo.usage  = RenderMemoryUsage::GpuOnly;
	_vkBuf.create(rdDevVk, vkAlloc, &allocInfo, targetSize, 
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | Util::toVkBufferUsage(cDesc.typeFlags)
				, QueueTypeFlags::Graphics | QueueTypeFlags::Transfer);

	RDS_VK_SET_DEBUG_NAME(_vkBuf);
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
RenderGpuBuffer_Vk::onUploadToGpu(ByteSpan data, SizeType offset)
{
	Base::onUploadToGpu(data, offset);
}


#endif
}
#endif