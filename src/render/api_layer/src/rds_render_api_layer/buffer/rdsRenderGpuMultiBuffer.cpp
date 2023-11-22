#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuMultiBuffer.h"

#include "../rdsRenderer.h"
#include "../command/rdsTransferRequest.h"

namespace rds
{

SPtr<RenderGpuMultiBuffer> 
RenderDevice::createRenderGpuMultiBuffer(RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = makeSPtr<RenderGpuMultiBuffer>();
	cDesc._internal_create(this);
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderGpuMultiBuffer-Impl ---
#endif // 0
#if 1

RenderGpuMultiBuffer::CreateDesc 
RenderGpuMultiBuffer::makeCDesc() 
{ 
	return CreateDesc{}; 
}

SPtr<RenderGpuMultiBuffer> 
RenderGpuMultiBuffer::make(CreateDesc& cDesc) 
{ 
	return Renderer::rdDev()->createRenderGpuMultiBuffer(cDesc); 
}

RenderGpuMultiBuffer::RenderGpuMultiBuffer()
{
	iFrame = 0;
}

RenderGpuMultiBuffer::~RenderGpuMultiBuffer()
{
	destroy();
}

void RenderGpuMultiBuffer::create(CreateDesc& cDesc)
{
	RDS_CORE_ASSERT(_renderGpuBuffers.is_empty(), "already create");

	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void RenderGpuMultiBuffer::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
RenderGpuMultiBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	RDS_TODO("upload to first buffer instead of create a new");

	transferRequest().uploadBuffer(nextBuffer(data.size() - offset), data, offset, this);
	//nextBuffer(data.size() - offset)->uploadToGpu(data, offset);
	//onUploadToGpu(data, offset);
}

void 
RenderGpuMultiBuffer::onCreate(CreateDesc& cDesc)
{
	_renderGpuBuffers.reserve(s_kFrameInFlightCount);
	auto e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc));
}

void 
RenderGpuMultiBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderGpuMultiBuffer::onDestroy()
{
	_renderGpuBuffers.clear();
}

void 
RenderGpuMultiBuffer::onUploadToGpu(ByteSpan data, SizeType offset)
{
	//nextBuffer(data.size() - offset)->uploadToGpu(data, offset);
	//renderFrame().addUploadBufferParent(this);
}

void RenderGpuMultiBuffer::rotate()
{
	iFrame = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
}

SPtr<RenderGpuBuffer>& 
RenderGpuMultiBuffer::nextBuffer(SizeType bufSize)
{
	auto nextIdx = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
	if (_renderGpuBuffers.size() < s_kFrameInFlightCount)
	{
		auto newCDesc = makeCDesc();
		newCDesc.typeFlags	= desc().typeFlags;
		newCDesc.stride		= desc().stride;
		newCDesc.bufSize	= bufSize;
		_renderGpuBuffers.emplace_back(RenderGpuBuffer::make(newCDesc));
	}
	return _renderGpuBuffers[nextIdx];
}



#endif


}