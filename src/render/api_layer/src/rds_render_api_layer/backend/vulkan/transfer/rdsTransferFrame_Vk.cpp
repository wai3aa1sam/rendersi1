#include "rds_render_api_layer-pch.h"
#include "rdsTransferFrame_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

UPtr<TransferFrame> 
RenderDevice_Vk::onCreateTransferFrame(TransferFrame_CreateDesc& cDesc)
{
	auto p = UPtr<TransferFrame>(makeUPtr<TransferFrame_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsStagingBuffer_Vk-Impl ---
#endif // 0
#if 1

TransferFrame_Vk::TransferFrame_Vk()
{

}

TransferFrame_Vk::~TransferFrame_Vk()
{
	destroy();
}

void 
TransferFrame_Vk::createRenderResource(const RenderFrameParam& rdFrameParam)
{

}

void 
TransferFrame_Vk::destroyRenderResource(const RenderFrameParam& rdFrameParam)
{

}

void
TransferFrame_Vk::requestStagingHandle(StagingHandle& out, SizeType size)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	out = _vkLinearStagingBuf.alloc(size);
}

void 
TransferFrame_Vk::uploadToStagingBuf(StagingHandle& out, ByteSpan data, SizeType offset)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	VkDeviceSize bytes = sCast<VkDeviceSize>(data.size());
	requestStagingHandle(out, bytes);
	auto* dst = _vkLinearStagingBuf.mappedData<u8*>(out);
	memory_copy(dst, data.data(), bytes);
}

void*	
TransferFrame_Vk::mappedStagingBufData(StagingHandle hnd)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	auto* dst = _vkLinearStagingBuf.mappedData<u8*>(hnd);
	return dst;
}

Vk_Buffer_T* 
TransferFrame_Vk::getVkStagingBufHnd(StagingHandle hnd)
{
	return _vkLinearStagingBuf.vkStagingBufHnd(hnd);
}
void 
TransferFrame_Vk::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	_vkLinearStagingBuf.create(renderDeviceVk());
}

void 
TransferFrame_Vk::onPostCreate(CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);

}

void 
TransferFrame_Vk::onDestroy()
{
	_vkLinearStagingBuf.destroy();

	Base::onDestroy();
}

void 
TransferFrame_Vk::onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd)
{

}

//StagingHandle 
//StagingBuffer_Vk::alloc(SizeType size)
//{
//	RDS_CORE_ASSERT(_rdDevVk, "not yet create()");
//
//	auto data = _chunks.scopedULock();
//	auto hnd = data->alloc(size, &_vkAlloc, _rdDevVk);
//	return hnd;
//}

//void 
//StagingBuffer_Vk::reset()
//{
//	auto data = _chunks.scopedULock();
//	data->reset();
//}
//
//void 
//StagingBuffer_Vk::clear()
//{
//	auto data = _chunks.scopedULock();
//	data->clear();
//}
//
//Vk_Buffer_T* 
//StagingBuffer_Vk::vkStagingBufHnd(StagingHandle hnd)
//{
//	auto data = _chunks.scopedSLock();
//	auto* bufHnd = data->vkStagingBufHnd(hnd);
//	return bufHnd;
//}

#endif


}


#endif