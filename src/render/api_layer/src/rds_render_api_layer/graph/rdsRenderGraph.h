#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rdsRenderGraphResourcePool.h"
#include "rdsRenderGraphResource.h"
#include "rdsRenderGraphPass.h"

/*
references:
~ RenderGraph
	- https://github.com/simco50/D3D12_Research
	- https://github.com/skaarj1989/FrameGraph
	- https://github.com/acdemiralp/fg
*/

namespace rds
{

class RdgPass;
class RdgDrawer;

#if 0
#pragma mark --- rdsRenderGraph-Decl ---
#endif // 0
#if 1

class RenderGraph : public NonCopyable
{
	friend class RdgDrawer;
	RDS_RENDER_API_LAYER_COMMON_BODY();

public:
	using Texture	= RdgResource_TextureT;
	using Buffer	= RdgResource_BufferT;
	template<class T> using RdgResource_CreateDescT = typename RdgResourceTrait<T>::CreateDesc;
	using TextureCreateDesc = RdgResource_CreateDescT<Texture>;
	using BufferCreateDesc	= RdgResource_CreateDescT<Buffer >;

public:
	static constexpr SizeType s_kPassLocalSize		= 20;
	static constexpr SizeType s_kResourceLocalSize	= 32;
	static constexpr SizeType s_kAlign				= 16;

public:
	RenderGraph();
	~RenderGraph();

	void create(StrView name, RenderDevice* rdDev, RenderContext* rdCtx, IAllocator* alloc = nullptr);
	void destroy();

	void compile();
	void execute();
	void dumpGraphviz(StrView filename = "debug/render_graph");	// visualization in https://dreampuf.github.io/GraphvizOnline/

	RdgPass& addPass(StrView name, RdgPassTypeFlag typeFlag, RdgPassFlag flag = RdgPassFlag::None);

	RdgTextureHnd	createTexture	(StrView name, const TextureCreateDesc&	cDesc);
	RdgBufferHnd	createBuffer	(StrView name, const BufferCreateDesc&	cDesc);

	RdgTextureHnd	importTexture(StrView name, Texture* tex);
	void			exportTexture(Texture* out, RdgTextureHnd hnd);

	RdgBufferHnd	importBuffer(StrView name, Buffer* buf);
	void			exportBuffer(Buffer* out, RdgBufferHnd hnd);

	RdgTextureHnd	findTexture(StrView name);
	RdgBufferHnd	findBuffer (StrView name);

protected:
	template<class T> RdgResourceHndT<T> createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc);

	template<class T, class... ARGS>	T*		newT(ARGS&&... args);
	template<class T>					void	deleteT(T* p);

	void clearFrame();
	void rotateFrame();

	RdgResourcePool& resourcePool();

private:
	void* alloc(SizeType n, SizeType align);
	void  free(void* p, SizeType align);

protected:
	RenderContext*	_rdCtx = nullptr;
	IAllocator*		_alloc = nullptr;

	String _name;
	int _frameIdx = 0;

	Vector<RdgPass*,		s_kPassLocalSize>		_passes;
	Vector<RdgResource*,	s_kResourceLocalSize>	_resources;
	Vector<RdgResourcePool, s_kFrameInFlightCount>	_framedResourcePool;
};

template<class T> inline
RdgResourceHndT<T> 
RenderGraph::createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc)
{
	using Tratis = RdgResourceTrait<T>;

	RdgResourceHndT<T> out = {};
	auto id = sCast<RdgId>(_resources.size());
	out._p = newT<RdgResourceT<T> >(cDesc, name, id, false, false);
	_resources.emplace_back(out._p);
	return out;
}

template<class T, class... ARGS> inline
T* 
RenderGraph::newT(ARGS&&... args)
{
	auto* buf = alloc(sizeof(T), s_kAlign);
	return new(buf) T(nmsp::forward<ARGS>(args)...);
}

template<class T> inline 
void	
RenderGraph::deleteT(T* p)
{
	p->~T();
	free(p, s_kAlign);
}

inline RdgResourcePool& RenderGraph::resourcePool() { return _framedResourcePool[_frameIdx]; }

#endif

}