#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuMultiBuffer.h"

#include "../rdsRenderer.h"
#include "../transfer/command/rdsTransferRequest.h"

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
RenderGpuMultiBuffer::makeCDesc(RDS_DEBUG_SRCLOC_PARAM) 
{ 
	return CreateDesc{}; 
}

SPtr<RenderGpuMultiBuffer> 
RenderGpuMultiBuffer::make(CreateDesc& cDesc) 
{ 
	return Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc); 
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
	/*
	* correct framed resource impl, only rotate when *commit, just like copy on write
	* , we can get the exact resource, only rotate when write 
	*/
	rotate();

	transferRequest().uploadBuffer(makeBufferOnDemand(data.size() - offset), data, offset);
	//nextBuffer(data.size() - offset)->uploadToGpu(data, offset);
	//onUploadToGpu(data, offset);
}

void 
RenderGpuMultiBuffer::onCreate(CreateDesc& cDesc)
{
	_renderGpuBuffers.resize(s_kFrameInFlightCount);
	_desc = cDesc;
	//auto& e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc)); RDS_UNUSED(e);	
}

void 
RenderGpuMultiBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderGpuMultiBuffer::onDestroy()
{
	_renderGpuBuffers.clear();
}

void RenderGpuMultiBuffer::rotate()
{
	_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;
}

SPtr<RenderGpuBuffer>& 
RenderGpuMultiBuffer::makeBufferOnDemand(SizeType bufSize)
{
	RDS_CORE_ASSERT(StrUtil::len(debugName()) > 0, "set a debug name for gpu buffer");

	// do acutal rotate in this function, then tsfFrame / ctx no need to update the frame separately
	//auto idx = (_iFrame + 1) % s_kFrameInFlightCount;
	auto idx = _iFrame % s_kFrameInFlightCount;;

	if (!_renderGpuBuffers[idx] || _renderGpuBuffers[idx]->bufSize() < bufSize)
	{
		_renderGpuBuffers[idx] = _makeNewBuffer(bufSize);
		_renderGpuBuffers[idx]->setDebugName(fmtAs_T<TempString>("{}-{}", debugName(), idx));
		//RDS_LOG_ERROR("created: {}-{}", debugName(), idx);
	}
	return _renderGpuBuffers[idx];
}

SPtr<RenderGpuBuffer> 
RenderGpuMultiBuffer::_makeNewBuffer(SizeType bufSize)
{
	auto newCDesc = makeCDesc(RDS_DEBUG_SRCLOC);
	newCDesc.typeFlags	= _desc.typeFlags;
	newCDesc.stride		= _desc.stride;
	newCDesc.bufSize	= bufSize;
	return RenderGpuBuffer::make(newCDesc);
}

#endif


}