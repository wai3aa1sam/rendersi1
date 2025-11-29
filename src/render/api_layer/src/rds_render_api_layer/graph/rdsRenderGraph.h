#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
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

class RenderGraphFrame : public NC_RenderApiLayerCommon_Base
{
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
		State			srcState;
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
	using FramedRscPool		= Vector<RdgResourcePool,	s_kMaxFrameAheadCountHardLimit>;
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
	RdgResource* createRdgResouce(RDS_DebugLabel_PARAM, const RdgResource_CreateDescT<T>& cDesc, RenderGraph& rdGraph)
	{
		using Tratis	= RdgResourceTraits<T>;
		using ResourceT = typename Tratis::ResourceT;

		auto id		= sCast<RdgId>(resources.size());
		//auto* rdgRsc = newT<RdgResourceT<T> >(cDesc, name, id, false, false);
		auto* rdgRsc = sCast<ResourceT*>(resources.emplace_back(newRdgResource<ResourceT>()));
		rdgRsc->create(RDS_DebugLabel_ARG, rdGraph, cDesc, id, false, false);
		return rdgRsc;
	}

private:
	template<class T, class... ARGS>
	T* 
	newRdgResource(ARGS&&... args)
	{
		auto&	alloc	= getRdgAlloc<T>();
		void*	buf		= alloc.allocate(sizeof(T));
		T*		p		= new(buf) T(rds::forward<ARGS>(args)...);
		return p;
	}

	template<class T>
	void 
	deleteRdgResource(T* p)
	{
		p->~T();

		auto&	alloc	= getRdgAlloc<T>();
		alloc.free(p);
	}

	template<class T> 
	LinearAllocator& 
	getRdgAlloc()
	{
		static_assert(IsSame<T, RdgBuffer> || IsSame<T, RdgTexture>, "only support RdgBuffer/RdgTexture");
		if constexpr (IsSame<T, RdgBuffer>)
		{
			return _rdgBuf_alloc;
		}
		else if constexpr (IsSame<T, RdgTexture>)
		{
			return _rdgTex_alloc;
		}
	}

private:
	// since those Buffer and Texture has different size
	// , separate the allocator to ensure when access the "dangling" ptr (freed when reset, but user still holding the Hnd)
	// , it will not access to wrong member ptr
	LinearAllocator _rdgBuf_alloc;
	LinearAllocator _rdgTex_alloc;

	Passes			_freePasses;
	LinearAllocator _passAlloc;

protected:
	struct State
	{
		bool isExecuted : 1;
	} _state;

			State&	state()			{ checkMainThreadExclusive(RDS_SRCLOC); return _state; }
	const	State&	state() const	{ checkMainThreadExclusive(RDS_SRCLOC); return _state; }
};

#endif

#if 0
#pragma mark --- rdsRenderGraph-Decl ---
#endif // 0
#if 1

class RenderGraph : public NC_RenderApiLayerCommon_Base
{
	friend class RdgResourceHnd;
	friend class RdgResource;

	friend class RenderContext;
	friend class RdgDrawer;

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
	using RenderGraphFrames = Vector<RenderGraphFrame, s_kMaxFrameAheadCountHardLimit>;
	using State				= RenderGraphFrame::State;

public:
	static constexpr SizeType s_kPassLocalSize = RenderGraphFrame::s_kPassLocalSize;

public:
	RenderGraph();
	~RenderGraph();

	void create(StrView name, RenderContext* rdCtx, IAllocator* alloc = nullptr);
	void destroy();

	void reset(RenderContext* rdCtx);

	void compile();
	void execute();
	//void commit(u32 frameIndex);
	void commit();
	void dumpGraphviz(			StrView filename = "debug/render_graph");				// visualization in https://dreampuf.github.io/GraphvizOnline/
	void dumpResourceStateGraph(StrView filename = "debug/render_graph_rsc_state");		// visualization in https://dreampuf.github.io/GraphvizOnline/

	RdgPass& addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlags flag = RdgPassFlags::None);
	RdgPass& addPass(const SrcLocData& srcLocData, RdgPassTypeFlags typeFlag, RdgPassFlags flag = RdgPassFlags::None);

	RdgTextureHnd	createTexture	(RDS_DebugLabel_PARAM, const TextureCreateDesc&	cDesc);
	RdgBufferHnd	createBuffer	(RDS_DebugLabel_PARAM, const BufferCreateDesc&	cDesc);
	RdgTextureHnd	createTexture	(StrView name, const TextureCreateDesc&	cDesc);
	RdgBufferHnd	createBuffer	(StrView name, const BufferCreateDesc&	cDesc);

	RdgTextureHnd	importTexture(RDS_DebugLabel_PARAM, TextureT* tex);
	RdgTextureHnd	importTexture(StrView name, TextureT* tex);
	RdgTextureHnd	importTexture(TextureT* tex);
	void			exportTexture(SPtr<Texture>*		out, RdgTextureHnd hnd, TextureUsageFlags usageFlag, ShaderStageFlag stage = ShaderStageFlag::None, Access access = Access::Read);
	void			exportTexture(RdgTextureHnd hnd, TextureUsageFlags usageFlag, Access access = Access::Read);

	RdgBufferHnd	importBuffer(RDS_DebugLabel_PARAM, Buffer* buf);
	RdgBufferHnd	importBuffer(StrView name, Buffer* buf);
	RdgBufferHnd	importBuffer(Buffer* buf);
	void			exportBuffer(SPtr<Buffer>* out, RdgBufferHnd hnd, RenderGpuBufferTypeFlags usageFlags, Access access = Access::Read);

	RdgTextureHnd	findTexture(StrView name);
	RdgBufferHnd	findBuffer (StrView name);

public:
	const String&	name() const;
	RenderContext*	renderContext();
	RenderDevice*	renderDevice();

	//u32 frameIndex() const;

protected:
	template<class T> typename RdgResourceTraits<T>::Hnd createRdgResource(RDS_DebugLabel_PARAM, const RdgResource_CreateDescT<T>& cDesc);

	template<class T, class... ARGS>	T*		newT(ARGS&&... args);
	template<class T>					void	deleteT(T* p);

	//void resetFrame();
	//void rotateFrame();

	void _setResourcesState(const Passes& sortedPasses);
	//void _setResourcesState(const Passes& sortedPasses, const PassDepths& passDepths);

public:
	//RenderGraphFrame&	renderGraphFrame(u32 frameIndex);
			RenderGraphFrame&	renderGraphFrame();
	const	RenderGraphFrame&	renderGraphFrame() const;

	Passes&		passes();
	Passes&		resultPasses();

	Resources&	resources();


	bool isExecuted() const;

private:
	void* alloc(SizeType n, SizeType align);
	void  free(void* p, SizeType align);

protected:
	String	_name;

	SPtr<RenderContext>	_rdCtx = nullptr;
	IAllocator*			_alloc = nullptr;

	RenderGraphFrame _rdgFrame;

protected:
	const	State& state() const;
			State& state();

	//u32		_frameIdx = 0;
	//RenderGraphFrames _rdgFrames;

protected:
	struct RdgHndPool
	{
	public:
		using T = RdgResource_WeakBlock;
	public:
		~RdgHndPool()
		{
			_alloc.destructAndClear<T>(_alloc.s_kDefaultAlign);
		}

		//void			reset();
		//template<class... TArgs>
		//T*				newObject(TArgs&&... args)
		T*				newObject()
		{
			if (!_freedObjs.is_empty())
			{
				auto obj = _freedObjs.moveBack();
				return obj;
			}
			else
			{
				auto* buf		= _alloc.alloc(sizeof(T));;
				auto* newObj	= new(buf) T();
				//_objs.emplace_back(newObj);
				return newObj;
			}
		}
		void deleteObject(T* obj)
		{
			_freedObjs.emplace_back(obj);
		}

	public:
		LinearAllocator _alloc;
		//Vector<T*> _objs;
		Vector<T*> _freedObjs;
		//Vector<T*, 16> _objs;
	};
	RdgHndPool _rdgHndPool;
};

template<class T> inline
typename RdgResourceTraits<T>::Hnd
RenderGraph::createRdgResource(RDS_DebugLabel_PARAM, const RdgResource_CreateDescT<T>& cDesc)
{
	using Tratis = RdgResourceTraits<T>;
	using HndT		= Tratis::Hnd;
	using ResourceT = typename Tratis::ResourceT;

	HndT out	= {};
	auto* rdgRsc = renderGraphFrame().createRdgResouce<T>(RDS_DebugLabel_ARG, cDesc, *this);
	out.reset(rdgRsc, this);
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

inline RenderContext*	RenderGraph::renderContext()	{ return _rdCtx; }
inline const String&	RenderGraph::name()		const	{ return _name; }

//inline u32					RenderGraph::frameIndex()	const				{ checkMainThreadExclusive(RDS_SRCLOC); return _frameIdx; }
//inline RenderGraphFrame&		RenderGraph::renderGraphFrame(u32 frameIndex)	{ return _rdgFrames[frameIndex]; }
inline			RenderGraphFrame&		RenderGraph::renderGraphFrame()					{ return _rdgFrame; }
inline const	RenderGraphFrame&		RenderGraph::renderGraphFrame()		const		{ return _rdgFrame; }

inline RenderGraph::Passes&		RenderGraph::passes()							{ return renderGraphFrame().passes; }
inline RenderGraph::Passes&		RenderGraph::resultPasses()						{ return renderGraphFrame().resultPasses; }
inline RenderGraph::Resources&	RenderGraph::resources()						{ return renderGraphFrame().resources; }

inline bool							RenderGraph::isExecuted() const {  return state().isExecuted; }
inline			RenderGraph::State&	RenderGraph::state()			{ checkMainThreadExclusive(RDS_SRCLOC); return renderGraphFrame().state(); }
inline const	RenderGraph::State&	RenderGraph::state() const		{ checkMainThreadExclusive(RDS_SRCLOC); return renderGraphFrame().state(); }

#endif

}