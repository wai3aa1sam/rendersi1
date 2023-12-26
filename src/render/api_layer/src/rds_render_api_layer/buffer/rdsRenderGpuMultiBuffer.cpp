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
	_iFrame = 0;
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
	_desc = cDesc;
	auto& e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc)); RDS_UNUSED(e);
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
	_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;
}

SPtr<RenderGpuBuffer>& 
RenderGpuMultiBuffer::nextBuffer(SizeType bufSize)
{
	// do acutal rotate in this function, then tsfFrame / ctx no need to update the frame separately
	auto nextIdx = (_iFrame + 1) % s_kFrameInFlightCount;
	if (_renderGpuBuffers.size() < s_kFrameInFlightCount)
	{
		return _renderGpuBuffers.emplace_back(_makeNewBuffer(bufSize));
	}
	else if (_renderGpuBuffers[nextIdx]->bufSize() < bufSize)
	{
		_renderGpuBuffers[nextIdx] = _makeNewBuffer(bufSize);
	}
	return _renderGpuBuffers[nextIdx];
}

SPtr<RenderGpuBuffer> 
RenderGpuMultiBuffer::_makeNewBuffer(SizeType bufSize)
{
	auto newCDesc = makeCDesc();
	newCDesc.typeFlags	= _desc.typeFlags;
	newCDesc.stride		= _desc.stride;
	newCDesc.bufSize	= bufSize;
	return RenderGpuBuffer::make(newCDesc);
}

#endif


}