#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"

namespace rds
{

SPtr<RenderGpuBuffer> 
RenderDevice::createRenderGpuBuffer(RenderGpuBuffer_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateRenderGpuBuffer(cDesc);
	return p;
}

#if 0
#pragma mark --- RenderGpuBuffer-Impl ---
#endif // 0
#if 1

RenderGpuBuffer::CreateDesc RenderGpuBuffer::makeCDesc(RDS_DEBUG_SRCLOC_PARAM) { return CreateDesc { RDS_DEBUG_SRCLOC_ARG }; }

SPtr<RenderGpuBuffer> 
RenderGpuBuffer::make(CreateDesc& cDesc)
{ 
	return Renderer::rdDev()->createRenderGpuBuffer(cDesc); 
}

RenderGpuBuffer::RenderGpuBuffer()
{
}

RenderGpuBuffer::~RenderGpuBuffer()
{
}

void RenderGpuBuffer::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void RenderGpuBuffer::destroy()
{
	onDestroy();
	Base::destroy();
}

void RenderGpuBuffer::onCreate(CreateDesc& cDesc)
{
	_desc = cDesc;
	_internal_setSubResourceCount(1);
	/*
	no need to set a state as if it is read, then it should not be mutable,
	if it is write, maybe Transfer_Dst, but we do not need to transit state as Tsf_Dst
	so no need to set the state
	*/

	if (BitUtil::hasAny(desc().typeFlags, RenderGpuBufferTypeFlags::Compute))
	{
		_bindlessHnd = renderDevice()->bindlessResource().allocBuffer(this);
	}
}

void RenderGpuBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderGpuBuffer::onDestroy()
{
	if (/*!isNull() || */bindlessHandle().isValid())
	{
		renderDevice()->bindlessResource().freeBuffer(this);
	}
}

void 
RenderGpuBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	transferRequest().uploadBuffer(this, data, offset);
}

void 
RenderGpuBuffer::onUploadToGpu(TransferCommand_UploadBuffer* cmd)
{
	RDS_CORE_ASSERT(!cmd || cmd->data.size() > 0 && cmd->data.size() <= bufSize(), "");

	//ByteSpan bs = ByteSpan{data.data() + offset, data.size() - offset};
	//QueueTypeFlags typeFlags = QueueTypeFlags::Graphics | QueueTypeFlags::Transfer;
	//renderFrame().uploadBuffer(this, bs, typeFlags);
}


#endif


}