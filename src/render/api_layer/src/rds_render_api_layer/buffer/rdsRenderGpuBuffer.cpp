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

RenderGpuBuffer::CreateDesc RenderGpuBuffer::makeCDesc() { return CreateDesc{}; }

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
}

void RenderGpuBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderGpuBuffer::onDestroy()
{

}

void 
RenderGpuBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	throwIf(data.size() + offset > bufSize(), "RenderGpuBuffer oversize");
	onUploadToGpu(data, offset);
}

void 
RenderGpuBuffer::onUploadToGpu(ByteSpan data, SizeType offset)
{
	ByteSpan bs = ByteSpan{data.data() + offset, data.size() - offset};
	QueueTypeFlags typeFlags = QueueTypeFlags::Graphics | QueueTypeFlags::Transfer;
	renderFrame().uploadBuffer(this, bs, typeFlags);
}


#endif


}