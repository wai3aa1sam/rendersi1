#include "rds_render_api_layer-pch.h"

#include "rdsRenderGpuBuffer.h"

#include "../rdsRenderer.h"


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
}

void RenderGpuBuffer::destroy()
{
	onDestroy();
}

void RenderGpuBuffer::onCreate(const CreateDesc& cDesc)
{

}

void RenderGpuBuffer::onPostCreate(const CreateDesc& cDesc)
{

}

void RenderGpuBuffer::onDestroy()
{

}

#endif


}