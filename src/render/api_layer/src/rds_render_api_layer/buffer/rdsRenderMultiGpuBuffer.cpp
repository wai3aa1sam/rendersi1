#include "rds_render_api_layer-pch.h"
#include "rdsRenderMultiGpuBuffer.h"

#include "../rdsRenderer.h"
#include "../rdsRenderDevice.h"
#include "../transfer/command/rdsTransferRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderMultiGpuBuffer-Impl ---
#endif // 0
#if 1

RenderMultiGpuBuffer::CreateDesc 
RenderMultiGpuBuffer::makeCDesc(RDS_DEBUG_SRCLOC_PARAM) 
{ 
	return CreateDesc{}; 
}

SPtr<RenderMultiGpuBuffer> 
RenderMultiGpuBuffer::make(CreateDesc& cDesc) 
{ 
	return Renderer::renderDevice()->createRenderMultiGpuBuffer(cDesc); 
}

RenderMultiGpuBuffer::RenderMultiGpuBuffer()
{
	_iFrame = 0;
}

RenderMultiGpuBuffer::~RenderMultiGpuBuffer()
{
	destroy();
}

void RenderMultiGpuBuffer::create(CreateDesc& cDesc)
{
	RDS_CORE_ASSERT(_renderGpuBuffers.is_empty(), "already create");

	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void RenderMultiGpuBuffer::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
RenderMultiGpuBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	/*
	* correct framed resource impl, only rotate when *commit, just like copy on write
	* , we can get the exact resource, only rotate when write 
	*/
	rotate();

	#if 0
	bool isFirstCreated = _renderGpuBuffers.size() == 1;
	if (!isFirstCreated)
	{
		rotate();
	}
	#endif // 0

	transferRequest().uploadBuffer(makeBufferOnDemand(data.size() - offset), data, offset);
	//nextBuffer(data.size() - offset)->uploadToGpu(data, offset);
	//onUploadToGpu(data, offset);
}

void 
RenderMultiGpuBuffer::onCreate(CreateDesc& cDesc)
{
	_renderGpuBuffers.resize(s_kFrameAheadCount);
	_desc = cDesc;
	//auto& e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc)); RDS_UNUSED(e);	
}

void 
RenderMultiGpuBuffer::onPostCreate(CreateDesc& cDesc)
{

}

void RenderMultiGpuBuffer::onDestroy()
{
	_renderGpuBuffers.clear();
}

void RenderMultiGpuBuffer::rotate()
{
	_iFrame = (_iFrame + 1) % s_kMaxFrameAheadCountHardLimit;
}

void 
RenderMultiGpuBuffer::setDebugName(StrView name)
{
	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	_debugName = name;
	#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME
	if (auto* p = renderGpuBuffer())
	{
		p->setDebugName(name);
	}
}

SPtr<RenderGpuBuffer>& 
RenderMultiGpuBuffer::makeBufferOnDemand(SizeType bufSize)
{
	RDS_CORE_ASSERT(StrUtil::len(debugName()) > 0, "set a debug name for gpu buffer");

	// do acutal rotate in this function, then tsfFrame / ctx no need to update the frame separately
	//auto idx = (_iFrame + 1) % s_kMaxFrameAheadCountHardLimit;
	auto idx = _iFrame % s_kMaxFrameAheadCountHardLimit;;

	if (!_renderGpuBuffers[idx] || _renderGpuBuffers[idx]->bufSize() < bufSize) // _renderGpuBuffers.size() < (idx + 1) || 
	{
		//_renderGpuBuffers.resize(s_kMaxFrameAheadCountHardLimit);
		_renderGpuBuffers[idx] = _makeNewBuffer(bufSize);
		_renderGpuBuffers[idx]->setDebugName(fmtAs_T<TempString>("{}-{}", debugName(), idx));
		//RDS_LOG_ERROR("created: {}-{}", debugName(), idx);
	}
	return _renderGpuBuffers[idx];
}

SPtr<RenderGpuBuffer> 
RenderMultiGpuBuffer::_makeNewBuffer(SizeType bufSize)
{
	auto newCDesc = makeCDesc(RDS_DEBUG_SRCLOC);
	newCDesc.typeFlags	= _desc.typeFlags;
	newCDesc.stride		= _desc.stride;
	newCDesc.bufSize	= bufSize;
	return RenderGpuBuffer::make(newCDesc);
}

#endif


}