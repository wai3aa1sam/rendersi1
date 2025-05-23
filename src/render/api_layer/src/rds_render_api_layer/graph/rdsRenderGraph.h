#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rdsRenderGraphResourcePool.h"
#include "rdsRenderGraphResource.h"
#include "rdsRenderGraphPass.h"
#include "../profiler/rdsGpuProfiler.h"

/*
references:
~ RenderGraph
	- https://github.com/simco50/D3D12_Research
	- https://github.com/skaarj1989/FrameGraph
	- https://github.com/acdemiralp/fg
*/

#if RDS_USE_GPU_PROFILER

#define RDS_RDG_EVENT_NAME(name, ...)			SrcLocData{RDS_SRCLOC, RDS_FMT(TempString, name, __VA_ARGS__).c_str()}
#define RDS_RDG_EVENT_NAME_C(name, color, ...)	SrcLocData{RDS_SRCLOC, RDS_FMT(TempString, name, __VA_ARGS__).c_str(), color}

#else

#define RDS_RDG_EVENT_NAME(name, ...)			RDS_FMT(TempString, name, __VA_ARGS__)
#define RDS_RDG_EVENT_NAME_C(name, color, ...)	RDS_FMT(TempString, name, __VA_ARGS__)

#endif // 	#if RDS_USE_GPU_PROFILER

namespace rds
{

class RdgPass;
class RdgDrawer;
class RenderGraph;
class RenderContext;


#if 0
#pragma mark --- rdsRenderGraph-Decl ---
#endif // 0
#if 1

class RenderGraphFrame : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class RenderGraph;
public:
	using StateUtil = RenderResourceStateFlagsUtil;
	using Pass		= RdgPass;
	using Access	= RdgAccess;

	using TextureT			= Texture;

	using Texture			= RdgResource_TextureT;
	using Buffer			= RdgResource_BufferT;

	template<class T> using RdgResource_CreateDescT = typename RdgResourceTraits<T>::CreateDesc;
	using TextureCreateDesc = RdgResource_CreateDescT<Texture>;
	using BufferCreateDesc	= RdgResource_CreateDescT<Buffer >;

	template<class T> using RdgResourceHndT = typename RdgResourceTraits<T>::Hnd;
	using RdgTextureHnd		= RdgResourceHndT<Texture>;
	using RdgBufferHnd		= RdgResourceHndT<Buffer >;

	template<class T>
	struct ExportedResourceT
	{
		using Usage		= RdgResourceUsage;
		using Access	= RdgAccess;
		using State		= RenderResourceStateFlags;

		RdgResource*	rdgRsc		= nullptr;
		SPtr<T>*		outRdRsc	= nullptr;
		State			pendingState;
	};
	using ExportedTexture	= ExportedResourceT<Texture>;
	using ExportedBuffer	= ExportedResourceT<Buffer >;

public:
	static constexpr SizeType s_kPassLocalSize		= 20;
	static constexpr SizeType s_kResourceLocalSize	= 32;
	static constexpr SizeType s_kAlign				= 16;

public:
	using Passes			= Vector<RdgPass*,			s_kPassLocalSize>;
	using Resources			= Vector<RdgResource*,		s_kResourceLocalSize>;
	using FramedRscPool		= Vector<RdgResourcePool,	s_kFrameInFlightCount>;
	using PassDepths		= Vector<u32,				s_kPassLocalSize>;

public:
	RenderGraph*	renderGraph = nullptr;

	Passes			resultPasses;
	PassDepths		resultPassDepths;

	Passes			passes;
	Resources		resources;
	RdgResourcePool resourcePool;

	#if 1
	
	Vector<ExportedTexture, 4> exportedTextures;
	Vector<ExportedBuffer , 4> exportedBuffers;

	/*

	struct ExportedResource
	{
	RdgResource*			rdgRsc		= nullptr;
	SPtr<RenderResource>*	outRdRsc	= nullptr;
	};
	Vector<ExportedResource> _exportedRscs;

	*/

	#endif // 1

public:
	RenderGraphFrame();
	~RenderGraphFrame();

	RenderGraphFrame(	RenderGraphFrame&&)	{ throwIf(true, ""); };
	void operator=(		RenderGraphFrame&&)	{ throwIf(true, ""); };

public:
	void create(RenderGraph* rdGraph_);
	void destroy();

	void reset();

	Pass* addPass(RenderGraph* rdGraph, StrView name, RdgPassTypeFlags typeFlag, RdgPassFlags flag);
	Pass* addPass(RenderGraph* rdGraph, const SrcLocData* srcLocData, RdgPassTypeFlags typeFlag, RdgPassFlags flag);

	template<class T>
	RdgResource* createRdgResouce(StrView name, const RdgResource_CreateDescT<T>& cDesc, const RenderGraph& rdGraph)
	{
		using Tratis	= RdgResourceTraits<T>;
		using ResourceT = typename Tratis::ResourceT;

		auto id		= sCast<RdgId>(resources.size());
		//auto* rdgRsc = newT<RdgResourceT<T> >(cDesc, name, id, false, false);
		auto* rdgRsc = sCast<ResourceT*>(resources.emplace_back(newT<ResourceT>()));
		rdgRsc->create(rdGraph, cDesc, name, id, false, false);
		return rdgRsc;
	}

private:
	template<class T, class... ARGS>
	T* newT(ARGS&&... args)
	{
		void*	buf = _alloc.allocate(sizeof(T));
		T*		p	= new(buf) T(rds::forward<ARGS>(args)...);
		return p;
	}

	template<class T>
	void deleteT(T* p)
	{
		p->~T();
		_alloc.free(p);
	}

private:
	LinearAllocator _alloc;

	Passes			_freePasses;
	LinearAllocator _passAlloc;
};

#endif

#if 0
#pragma mark --- rdsRenderGraph-Decl ---
#endif // 0
#if 1

class RenderGraph : public NonCopyable
{
	friend class RenderContext;
	friend class RdgDrawer;
	RDS_RENDER_API_LAYER_COMMON_BODY();

public:
	using StateUtil			= RenderGraphFrame::StateUtil;
	using Pass				= RenderGraphFrame::Pass;
	using Access			= RenderGraphFrame::Access;

	using TextureT			= RenderGraphFrame::TextureT;

	using Texture			= RenderGraphFrame::Texture;
	using Buffer			= RenderGraphFrame::Buffer;

	template<class T> using RdgResource_CreateDescT = typename RenderGraphFrame::RdgResource_CreateDescT<T>;
	using TextureCreateDesc = RenderGraphFrame::TextureCreateDesc;
	using BufferCreateDesc	= RenderGraphFrame::BufferCreateDesc;

	template<class T> using RdgResourceHndT = typename RenderGraphFrame::RdgResourceHndT<T>;
	using RdgTextureHnd		= RenderGraphFrame::RdgTextureHnd;
	using RdgBufferHnd		= RenderGraphFrame::RdgBufferHnd;

	template<class T>
	struct ExportedResourceT
	{
		using Usage		= RdgResourceUsage;
		using Access	= RdgAccess;
		using State		= RenderResourceStateFlags;

		RdgResource*	rdgRsc		= nullptr;
		SPtr<T>*		outRdRsc	= nullptr;
		State			pendingState;
	};
	using ExportedTexture	= ExportedResourceT<Texture>;
	using ExportedBuffer	= ExportedResourceT<Buffer >;

	using Passes			= RenderGraphFrame::Passes;
	using Resources			= RenderGraphFrame::Resources;
	using FramedRscPool		= RenderGraphFrame::FramedRscPool;
	using PassDepths		= RenderGraphFrame::PassDepths;
	using RenderGraphFrames = Vector<RenderGraphFrame, s_kFrameInFlightCount>;

public:
	static constexpr SizeType s_kPassLocalSize = RenderGraphFrame::s_kPassLocalSize;

public:
	RenderGraph();
	~RenderGraph();

	void create(StrView name, RenderContext* rdCtx, IAllocator* alloc = nullptr);
	void destroy();

	void reset();

	void compile();
	void execute();
	void commit(u32 frameIndex);
	void dumpGraphviz(			StrView filename = "debug/render_graph");				// visualization in https://dreampuf.github.io/GraphvizOnline/
	void dumpResourceStateGraph(StrView filename = "debug/render_graph_rsc_state");		// visualization in https://dreampuf.github.io/GraphvizOnline/

	RdgPass& addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlags flag = RdgPassFlags::None);
	RdgPass& addPass(const SrcLocData& srcLocData, RdgPassTypeFlags typeFlag, RdgPassFlags flag = RdgPassFlags::None);

	RdgTextureHnd	createTexture	(StrView name, const TextureCreateDesc&	cDesc);
	RdgBufferHnd	createBuffer	(StrView name, const BufferCreateDesc&	cDesc);

	RdgTextureHnd	importTexture(StrView name, TextureT* tex);
	void			exportTexture(SPtr<Texture>*		out, RdgTextureHnd hnd, TextureUsageFlags usageFlag, Access access = Access::Read);
	void			exportTexture(RdgTextureHnd hnd, TextureUsageFlags usageFlag, Access access = Access::Read);

	RdgBufferHnd	importBuffer(StrView name, Buffer* buf);
	void			exportBuffer(SPtr<Buffer>* out, RdgBufferHnd hnd, RenderGpuBufferTypeFlags usageFlags, Access access = Access::Read);

	RdgTextureHnd	findTexture(StrView name);
	RdgBufferHnd	findBuffer (StrView name);

public:
	RenderContext* renderContext();

	const String& name() const;

	u32 frameIndex() const;
	RenderDevice* renderDevice();

protected:
	template<class T> typename RdgResourceTraits<T>::Hnd createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc);

	template<class T, class... ARGS>	T*		newT(ARGS&&... args);
	template<class T>					void	deleteT(T* p);

	//void resetFrame();
	void rotateFrame();

	void _setResourcesState(const Passes& sortedPasses, const PassDepths& passDepths);

	RenderGraphFrame&	renderGraphFrame(u32 frameIndex);
	RenderGraphFrame&	renderGraphFrame();
	RdgResourcePool&	resourcePool(	 u32 frameIndex);
	
	Passes&		passes();
	Resources&	resources();


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

template<class T> inline
typename RdgResourceTraits<T>::Hnd
RenderGraph::createRdgResource(StrView name, const RdgResource_CreateDescT<T>& cDesc)
{
	using Tratis = RdgResourceTraits<T>;
	using HndT		= Tratis::Hnd;
	using ResourceT = typename Tratis::ResourceT;

	HndT out	= {};
	auto* rdgRsc = renderGraphFrame().createRdgResouce<T>(name, cDesc, *this);
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

inline RenderContext* RenderGraph::renderContext() { return _rdCtx; }

inline const String&	RenderGraph::name()		const { return _name; }

inline u32				RenderGraph::frameIndex()	const { checkMainThreadExclusive(RDS_SRCLOC); return _frameIdx; }

inline RenderGraphFrame&		RenderGraph::renderGraphFrame(u32 frameIndex)	{ return _rdgFrames[frameIndex]; }
inline RenderGraphFrame&		RenderGraph::renderGraphFrame()					{ return _rdgFrames[frameIndex()]; }
inline RdgResourcePool&			RenderGraph::resourcePool(u32 frameIndex)		{ return renderGraphFrame(frameIndex).resourcePool; }

inline RenderGraph::Passes&		RenderGraph::passes()							{ return renderGraphFrame().passes; }
inline RenderGraph::Resources&	RenderGraph::resources()						{ return renderGraphFrame().resources; }

#endif

}