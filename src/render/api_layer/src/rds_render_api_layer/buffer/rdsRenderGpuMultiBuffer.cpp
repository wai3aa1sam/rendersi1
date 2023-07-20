#include "rds_render_api_layer-pch.h"
#include "rdsRenderGpuMultiBuffer.h"

#include "../rdsRenderer.h"
#include "../command/rdsTransferRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGpuMultiBuffer-Impl ---
#endif // 0
#if 1

RenderGpuMultiBuffer::CreateDesc RenderGpuMultiBuffer::makeCDesc() { return CreateDesc{}; }

SPtr<RenderGpuMultiBuffer> RenderGpuMultiBuffer::make(const CreateDesc& cDesc) { return nullptr/*Renderer::instance()->createRenderGpuMultiBuffer(cDesc)*/; }

RenderGpuMultiBuffer::RenderGpuMultiBuffer()
{
}

RenderGpuMultiBuffer::~RenderGpuMultiBuffer()
{
}

void RenderGpuMultiBuffer::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);

	_renderGpuBuffers.reserve(s_kFrameInFlightCount);
	auto e = _renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc));
	
}

void RenderGpuMultiBuffer::destroy()
{
	onDestroy();
	_renderGpuBuffers.clear();
}

void RenderGpuMultiBuffer::onCreate(const CreateDesc& cDesc)
{

}

void RenderGpuMultiBuffer::onPostCreate(const CreateDesc& cDesc)
{

}

void RenderGpuMultiBuffer::onDestroy()
{

}

void 
RenderGpuMultiBuffer::onUploadToGpu(ByteSpan data, SizeType offset)
{
	nextBuffer()->uploadToGpu(data, offset);
}

void RenderGpuMultiBuffer::rotate()
{
	iFrame = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
}
SPtr<RenderGpuBuffer>& RenderGpuMultiBuffer::nextBuffer()
{
	auto nextIdx = math::modPow2Val(iFrame + 1, s_kFrameInFlightCount);
	if (_renderGpuBuffers.size() < s_kFrameInFlightCount)
	{
		_renderGpuBuffers.emplace_back(RenderGpuBuffer::make(cDesc()));
	}
	return _renderGpuBuffers[nextIdx];
}



#endif


}