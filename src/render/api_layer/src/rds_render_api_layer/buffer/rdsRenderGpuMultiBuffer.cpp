#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuMultiBuffer.h"

#include "../rdsRenderer.h"
#include "../command/rdsTransferRequest.h"

namespace rds
{

SPtr<RenderGpuMultiBuffer> 
Renderer::createRenderGpuMultiBuffer(const RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = SPtr<RenderGpuMultiBuffer>(makeSPtr<RenderGpuMultiBuffer>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderGpuMultiBuffer-Impl ---
#endif // 0
#if 1

RenderGpuMultiBuffer::CreateDesc RenderGpuMultiBuffer::makeCDesc() { return CreateDesc{}; }

SPtr<RenderGpuMultiBuffer> RenderGpuMultiBuffer::make(const CreateDesc& cDesc) { return Renderer::instance()->createRenderGpuMultiBuffer(cDesc); }

RenderGpuMultiBuffer::RenderGpuMultiBuffer()
{
	iFrame = 0;
}

RenderGpuMultiBuffer::~RenderGpuMultiBuffer()
{
	destroy();
}

void RenderGpuMultiBuffer::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);

	onPostCreate(cDesc);
}

void RenderGpuMultiBuffer::destroy()
{
	onDestroy();
}

void 
RenderGpuMultiBuffer::uploadToGpu(ByteSpan data, SizeType offset)
{
	onUploadToGpu(data, offset);
}

void RenderGpuMultiBuffer::onCreate(const CreateDesc& cDesc)
{
	_renderGpuBuffers.reserve(s_kFrameInFlightCount);
	auto e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc));
}

void RenderGpuMultiBuffer::onPostCreate(const CreateDesc& cDesc)
{

}

void RenderGpuMultiBuffer::onDestroy()
{
	_renderGpuBuffers.clear();
}

void 
RenderGpuMultiBuffer::onUploadToGpu(ByteSpan data, SizeType offset)
{
	nextBuffer(data.size() - offset)->uploadToGpu(data, offset);
	RenderFrameContext::instance()->renderFrame().addUploadBufferParent(this);
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
		auto newCDesc = desc();
		newCDesc.bufSize = bufSize;
		_renderGpuBuffers.emplace_back(RenderGpuBuffer::make(newCDesc));
	}
	return _renderGpuBuffers[nextIdx];
}



#endif


}