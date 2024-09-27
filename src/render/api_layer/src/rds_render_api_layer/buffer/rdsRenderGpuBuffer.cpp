#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuBuffer.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

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
	return Renderer::renderDevice()->createRenderGpuBuffer(cDesc); 
}

RenderGpuBuffer::RenderGpuBuffer()
{
}

RenderGpuBuffer::~RenderGpuBuffer()
{
	//RDS_LOG_ERROR("~RenderGpuBuffer() {}", debugName());
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

	if (isComputeBuffer())
	{
		_bindlessHnd = renderDevice()->bindlessResource().allocBuffer(this);
	}

	transferContext().createBuffer(this);
}

void RenderGpuBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderGpuBuffer::onDestroy()
{
	renderDevice()->bindlessResource().freeBuffer(this);
}


void 
RenderGpuBuffer::_internal_requestDestroyObject()
{
	Base::_internal_requestDestroyObject();
	transferContext().destroyBuffer(this);
}

void 
RenderGpuBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	transferRequest().uploadBuffer(this, data, offset);
}

void 
RenderGpuBuffer::onUploadToGpu(TransferCommand_UploadBuffer* cmd)
{
	RDS_CORE_ASSERT(!cmd || StrUtil::len(debugName()) > 0, "set a debug name before upload gpu buffer");
	RDS_CORE_ASSERT(!cmd || cmd->data.size() > 0 && cmd->data.size() <= bufSize(), "");

	//ByteSpan bs = ByteSpan{data.data() + offset, data.size() - offset};
	//QueueTypeFlags typeFlags = QueueTypeFlags::Graphics | QueueTypeFlags::Transfer;
	//renderFrame().uploadBuffer(this, bs, typeFlags);
}


#endif


}