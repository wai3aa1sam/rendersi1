#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphResourcePool.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgResourcePool-Impl ---
#endif // 0
#if 1

RdgResourcePool::RdgResourcePool()
{

}

RdgResourcePool::~RdgResourcePool()
{
	destroy();
}

void 
RdgResourcePool::create (/*RenderGraph* rdGraph*/)
{

}

void 
RdgResourcePool::destroy()
{
	_texs.clear();
	_bufs.clear();
}

void 
RdgResourcePool::reset()
{
	destroy();
}

RdgResource_TextureT*	
RdgResourcePool::createTexture	(Texture_CreateDesc&	cDesc, RenderDevice* rdDev)
{
	auto& tex = _texs.emplace_back();
	tex = rdDev->createTexture2D(cDesc);
	return tex;
}

RdgResource_BufferT*	
RdgResourcePool::createBuffer	(Buffer_CreateDesc&		cDesc, RenderDevice* rdDev)
{
	auto& buf = _bufs.emplace_back();
	buf = rdDev->createRenderGpuBuffer(cDesc);
	return buf;
}

#endif

}