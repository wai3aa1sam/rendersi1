#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraph.h"
#include "rdsRenderGraphDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderGraph-Impl ---
#endif // 0
#if 1

RenderGraph::RenderGraph()
{
}

RenderGraph::~RenderGraph()
{
	destroy();
}

void RenderGraph::create(StrView name, Renderer* rdr, RenderContext* rdCtx, IAllocator* alloc)
{
	destroy();

	_name = name;
	_rdCtx = rdCtx;
	_alloc = alloc;
}

void RenderGraph::destroy()
{
	_rdCtx = nullptr;
	_alloc = nullptr;


	for (auto& e : _resources)
	{
		deleteT(e);
	}
	_resources.clear();

	for (auto& e : _passes)
	{
		deleteT(e);
	}
	_passes.clear();
}

void 
RenderGraph::compile()
{
	clearFrame();

	rotateFrame();
}

void 
RenderGraph::execute()
{
	_notYetSupported();
}

void 
RenderGraph::dumpGraphviz(StrView filename)
{
	RdgDrawer drawer;
	drawer.dump(filename, this);
}

RdgPass& 
RenderGraph::addPass(StrView name, RdgPassTypeFlag typeFlag, RdgPassFlag flag)
{
	auto id = sCast<RdgId>(_passes.size());
	auto& pass = _passes.emplace_back(newT<RdgPass>(this, name, id, typeFlag, flag));
	return *pass;
}

RdgTextureHnd 
RenderGraph::createTexture(StrView name, const TextureCreateDesc& cDesc)
{
	return createRdgResource<RdgResource_TextureT>(name, cDesc);
}

RdgBufferHnd 
RenderGraph::createBuffer(StrView name, const BufferCreateDesc& cDesc)
{
	return createRdgResource<RdgResource_BufferT>(name, cDesc);
}

RdgTextureHnd 
RenderGraph::importTexture(StrView name, Texture* tex)
{
	RdgTexture_CreateDesc cDesc;
	//cDesc
	auto hnd = createTexture(name, cDesc);
	hnd._p->setImport(true);

	return hnd;
}

void 
RenderGraph::exportTexture(Texture* out, RdgTextureHnd hnd)
{
	hnd._p->setExport(true);
}

RdgBufferHnd 
RenderGraph::importBuffer(StrView name, Buffer* buf)
{
	RdgBuffer_CreateDesc cDesc;
	cDesc = buf->desc();
	cDesc.typeFlags |= RenderGpuBufferTypeFlags::Compute;
	
	auto hnd = createBuffer(name, cDesc);
	hnd._p->setImport(true);

	return hnd;
}

void 
RenderGraph::exportBuffer(Buffer* out, RdgBufferHnd hnd)
{
	hnd._p->setExport(true);
}

RdgTextureHnd RenderGraph::findTexture(StrView name)
{
	_notYetSupported();
	return RdgTextureHnd();
}

RdgBufferHnd RenderGraph::findBuffer(StrView name)
{
	_notYetSupported();
	return RdgBufferHnd();
}

void* RenderGraph::alloc(SizeType n, SizeType align)
{
	if (_alloc)
	{
		auto* p = _alloc->alloc(n, align);
		return p;
	}
	return DefaultAllocator::alloc(n, align);
}

void RenderGraph::free(void* p, SizeType align)
{
	if (_alloc)
	{
		return _alloc->free(p, align);
	}
	return DefaultAllocator::free(p, align);
}

void RenderGraph::clearFrame()
{
	//resourcePool().clear();
}

void RenderGraph::rotateFrame()
{
	_frameIdx = (_frameIdx + 1) % s_kFrameInFlightCount;
}

#endif

}