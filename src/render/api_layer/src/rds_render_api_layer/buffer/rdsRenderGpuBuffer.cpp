#include "rds_render_api_layer-pch.h"

#include "rdsRenderGpuBuffer.h"

#include "../rdsRenderer.h"
#include "../command/rdsTransferRequest.h"
#include "../rdsRenderFrame.h"

namespace rds
{

SPtr<RenderGpuBuffer> Renderer::createRenderGpuBuffer(const RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = onCreateRenderGpuBuffer(cDesc);
	return p;
}

#if 0
#pragma mark --- RenderGpuBuffer-Impl ---
#endif // 0
#if 1

RenderGpuBuffer::CreateDesc RenderGpuBuffer::makeCDesc() { return CreateDesc{}; }

SPtr<RenderGpuBuffer> RenderGpuBuffer::make(const CreateDesc& cDesc) { return Renderer::instance()->createRenderGpuBuffer(cDesc); }

RenderGpuBuffer::RenderGpuBuffer()
{
}

RenderGpuBuffer::~RenderGpuBuffer()
{
}

void RenderGpuBuffer::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);

	onPostCreate(cDesc);
}

void RenderGpuBuffer::destroy()
{
	onDestroy();
}

void RenderGpuBuffer::onCreate(const CreateDesc& cDesc)
{
	_cDesc = cDesc;
}

void RenderGpuBuffer::onPostCreate(const CreateDesc& cDesc)
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
	RenderFrameContext::instance()->renderFrame().uploadBuffer(this, bs, typeFlags);
}


#endif


}