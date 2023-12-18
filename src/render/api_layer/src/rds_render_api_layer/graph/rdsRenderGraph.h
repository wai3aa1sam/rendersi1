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
class RenderGraph;
#if 0
#pragma mark --- rdsRenderGraph-Decl ---
#endif // 0
#if 1

class RenderGraph : public NonCopyable
{
	friend class RdgDrawer;
	RDS_RENDER_API_LAYER_COMMON_BODY();

public:
	using Texture			= RdgResource_TextureT;
	using Buffer			= RdgResource_BufferT;

	template<class T> using RdgResource_CreateDescT = typename RdgResourceTraits<T>::CreateDesc;
	using TextureCreateDesc = RdgResource_CreateDescT<Texture>;
	using BufferCreateDesc	= RdgResource_CreateDescT<Buffer >;

	template<class T> using RdgResourceHndT = typename RdgResourceTraits<T>::Hnd;
	using RdgTextureHnd		= RdgResourceHndT<Texture>;
	using RdgBufferHnd		= RdgResourceHndT<Buffer >;


public:
	static constexpr SizeType s_kPassLocalSize		= 20;
	static constexpr SizeType s_kResourceLocalSize	= 32;
	static constexpr SizeType s_kAlign				= 16;

public:
	using Pass		= RdgPass;
	using Access	= RdgAccess;

	using Passes			= Vector<RdgPass*,			s_kPassLocalSize>;
	using Resources			= Vector<RdgResource*,		s_kResourceLocalSize>;
	using FramedRscPool		= Vector<RdgResourcePool,	s_kFrameInFlightCount>;

	using PassDepths		= Vector<u32, s_kPassLocalSize>;
	using PendingResources	= Vector<RdgResource*, 128>;

public:
	struct RenderGraphFrame : public NonCopyable
	{
		RenderGraphFrame()
		{

		}

		~RenderGraphFrame()
		{
			destroy();
		}

		RenderGraphFrame(RenderGraphFrame&&)	{ throwIf(true, ""); };
		void operator=(RenderGraphFrame&&)		{ throwIf(true, ""); };

		void create(RenderGraph* rdGraph_)
		{
			destroy();
			rdGraph = rdGraph_;
		}

		void destroy()
		{
			reset();
			rdGraph = nullptr;
		}

		void reset()
		{
			resultPasses.clear();
			resultPassDepths.clear();

			RDS_TODO("change to linear allocator instead");

			for (RdgPass* pass : passes)
			{
				RDS_DELETE(pass);
			}
			for (RdgResource* rsc : resources)
			{
				RDS_DELETE(rsc);
			}
			passes.clear();
			resources.clear();
			rscPool.reset();
		}

		Pass* addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlag flag)
		{
			auto id = sCast<RdgId>(passes.size());
			Pass*& pass = passes.emplace_back(RDS_NEW(RdgPass)(rdGraph, name, id, typeFlag, flag));
			return pass;
		}

		template<class T>
		RdgResource* createRdgResouce(StrView name, const RdgResource_CreateDescT<T>& cDesc)
		{
			using Tratis	= RdgResourceTraits<T>;
			using ResourceT = typename Tratis::ResourceT;

			auto id		= sCast<RdgId>(resources.size());
			//auto* rdgRsc = newT<RdgResourceT<T> >(cDesc, name, id, false, false);
			auto* rdgRsc = sCast<ResourceT*>(resources.emplace_back(RDS_NEW(ResourceT)));
			rdgRsc->create(cDesc, name, id, false, false);
			return rdgRsc;
		}

		RenderGraph*	rdGraph = nullptr;

		Passes			resultPasses;
		PassDepths		resultPassDepths;

		Passes			passes;
		Resources		resources;
		RdgResourcePool rscPool;
	};
	using RenderGraphFrames = Vector<RenderGraphFrame, s_kFrameInFlightCount>;

public:
	RenderGraph();
	~RenderGraph();

	void create(StrView name, RenderContext* rdCtx, IAllocator* alloc = nullptr);
	void destroy();

	void compile();
	void execute();
	void commit();
	void dumpGraphviz(StrView filename = "debug/render_graph");	// visualization in https://dreampuf.github.io/GraphvizOnline/

	RdgPass& addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlag flag = RdgPassFlag::None);

	RdgTextureHnd	createTexture	(StrView name, const TextureCreateDesc&	cDesc);
	RdgBufferHnd	createBuffer	(StrView name, const BufferCreateDesc&	cDesc);

	RdgTextureHnd	importTexture(StrView name, Texture* tex);
	void			exportTexture(Texture* out, RdgTextureHnd hnd);

	RdgBufferHnd	importBuffer(StrView name, Buffer* buf);
	void			exportBuffer(Buffer* out, RdgBufferHnd hnd);

	RdgTextureHnd	findTexture(StrView name);
	RdgBufferHnd	findBuffer (StrView name);

	Span<Pass*>		resultPasses();
	Span<Pass*>		allPasses();

protected:
	template<class T> typename RdgResourceTraits<T>::Hnd createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc);

	template<class T, class... ARGS>	T*		newT(ARGS&&... args);
	template<class T>					void	deleteT(T* p);

	//void resetFrame();
	void rotateFrame();

	void _setResourcesState(const Passes& sortedPasses, const PassDepths& passDepths);
	void _flushResourcesState(PendingResources& pendingRscs);

	RdgResourcePool&	resourcePool();
	RenderGraphFrame&	renderGraphFrame();
	
	Passes&		passes();
	Resources&	resources();

	RenderDevice* renderDevice();

private:
	void* alloc(SizeType n, SizeType align);
	void  free(void* p, SizeType align);

protected:
	String	_name;
	u32		_frameIdx = 0;

	RenderContext*	_rdCtx = nullptr;
	IAllocator*		_alloc = nullptr;

	RenderGraphFrames _rdgFrames;
};

inline Span<RenderGraph::Pass*>	RenderGraph::allPasses()	{ return renderGraphFrame().passes; }
inline Span<RenderGraph::Pass*> RenderGraph::resultPasses() { return renderGraphFrame().resultPasses; }

template<class T> inline
typename RdgResourceTraits<T>::Hnd
RenderGraph::createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc)
{
	using Tratis = RdgResourceTraits<T>;
	using HndT		= Tratis::Hnd;
	using ResourceT = typename Tratis::ResourceT;

	HndT out	= {};
	auto* rdgRsc = renderGraphFrame().createRdgResouce<T>(name, cDesc);
	out._rdgRsc = rdgRsc;
	return out;
}

template<class T, class... ARGS> inline
T* 
RenderGraph::newT(ARGS&&... args)
{
	auto* buf = alloc(sizeof(T), s_kAlign);
	return new(buf) T(rds::forward<ARGS>(args)...);
}

template<class T> inline 
void	
RenderGraph::deleteT(T* p)
{
	p->~T();
	free(p, s_kAlign);
}

inline RdgResourcePool& RenderGraph::resourcePool()						{ return renderGraphFrame().rscPool; }
inline RenderGraph::RenderGraphFrame& RenderGraph::renderGraphFrame()	{ return _rdgFrames[_frameIdx]; }

inline RenderGraph::Passes&		RenderGraph::passes()		{ return renderGraphFrame().passes; }
inline RenderGraph::Resources&	RenderGraph::resources()	{ return renderGraphFrame().resources; }


#endif

}