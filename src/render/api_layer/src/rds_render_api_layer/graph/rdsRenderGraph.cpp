#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraph.h"
#include "rdsRenderGraphDrawer.h"
#include "rds_render_api_layer/rdsRenderContext.h"

namespace rds
{


#if 0
#pragma mark --- rdsRenderGraph::RenderGraphFrame-Impl ---
#endif // 0
#if 1

RenderGraphFrame::RenderGraphFrame()
{

}

RenderGraphFrame::~RenderGraphFrame()
{
	destroy();
}


void 
RenderGraphFrame::create(RenderGraph* rdGraph_)
{
	destroy();
	renderGraph = rdGraph_;
}

void 
RenderGraphFrame::destroy()
{
	reset();
	_passAlloc.destructAndClear<RdgPass>(_passAlloc.s_kDefaultAlign);
	renderGraph = nullptr;
}

void 
RenderGraphFrame::reset()
{
	resultPasses.clear();
	resultPassDepths.clear();

	//for (RdgPass* pass : passes)
	//{
	//	pass->destroy();
	//	//deleteT(pass);
	//}
	for (RdgResource* rsc : resources)
	{
		switch (rsc->type())
		{
			case RdgResourceType::Buffer:	{ deleteT(sCast<RdgBuffer*>(rsc)); }	break;
			case RdgResourceType::Texture:	{ deleteT(sCast<RdgTexture*>(rsc)); }	break;
			default: { RDS_THROW("invalid RdgResourceType"); } break;
		}
	}

	_freePasses = passes;
	passes.clear();
	resources.clear();

	resourcePool.reset();
	_alloc.clear();

	exportedTextures.clear();
	exportedBuffers.clear();
}

RenderGraphFrame::Pass* 
RenderGraphFrame::addPass(RenderGraph* rdGraph, StrView name, RdgPassTypeFlags typeFlag, RdgPassFlags flag)
{
	auto id = sCast<RdgId>(passes.size());
	Pass* pass = nullptr;
	if (!_freePasses.is_empty())
	{
		pass = _freePasses.moveBack();
	}
	else
	{
		pass = _passAlloc.newT<RdgPass>();
	}
	pass->create(rdGraph, name, id, typeFlag, flag);
	passes.emplace_back(pass);
	return pass;
}

RenderGraphFrame::Pass* 
RenderGraphFrame::addPass(RenderGraph* rdGraph, const SrcLocData* srcLocData, RdgPassTypeFlags typeFlag, RdgPassFlags flag)
{
	auto* pass = addPass(rdGraph, StrView{srcLocData->name}, typeFlag, flag);
	#if RDS_USE_GPU_PROFILER
	pass->_srcLocData = srcLocData;
	#endif // RDS_USE_GPU_PROFILER
	return pass;
}

#endif

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

void RenderGraph::create(StrView name, RenderContext* rdCtx, IAllocator* alloc)
{
	destroy();

	_name = name;
	_rdCtx = rdCtx;
	_alloc = alloc;

	auto& rdgFrame = _rdgFrames.emplace_back();
	rdgFrame.create(this);
}

void RenderGraph::destroy()
{
	#if 0
	for (auto& e : _resources)
	{
		// uesless now, moved the rsc to the base class
		#if 0
		using SRC = RdgResourceType;
		switch (e->type())
		{
			case SRC::Texture:	{ deleteT(sCast<RdgTexture*>(e)); } break;
			case SRC::Buffer:	{ deleteT(sCast<RdgBuffer*>(e)); }	break;
			default: { RDS_THROW("invalid RenderGraphResource type: {}", e->type()); } break;
		}
		#else
		deleteT(e);
		#endif // 0

	}
	_resources.clear();

	for (auto& e : _passes)
	{
		deleteT(e);
	}
	_passes.clear();
	#endif // 0
	
	for (auto& frame : _rdgFrames)
	{
		frame.destroy();
	}

	_alloc = nullptr;
	_rdCtx = nullptr;
}

void 
RenderGraph::reset() 
{ 
	RDS_TODO("temporary solution");
	rotateFrame();
	auto& rdgFrame = renderGraphFrame(frameIndex());
	rdgFrame.reset();
}

void 
RenderGraph::compile()
{
	checkMainThreadExclusive(RDS_SRCLOC);
	RDS_PROFILE_SCOPED();

	if (!_rdCtx->isValidFramebufferSize())
		return;

	/*
	since we have multi frame, all the RdgPass and RdgResource can not reuse (after hash and confirm same as last frame)
	, only the order (orderedPassId) can be reuse
	*/
	auto& rdgFrame	= renderGraphFrame(frameIndex());
	auto& resources = rdgFrame.resources;

	// cull no reference passes

	// topological sort and reverse order
	// set graph depth
	Passes&		sortedPasses	= rdgFrame.resultPasses;
	PassDepths&	passDepths		= rdgFrame.resultPassDepths;
	{
		class RdgPassSorter
		{
			using Pass			= RdgPass;
			using VisitedList	= Vector<bool, s_kPassLocalSize>;

			struct VisitData
			{
				Passes&			stack;
				VisitedList&	visitedList;
				RdgPass*		pass;
			};

		public:
			void topologicalSort(Passes& dst, Span<Pass*> src)
			{
				dst.clear();

				VisitedList visitedList;

				auto n = src.size();

				dst.reserve(n);
				visitedList.resize(n);
				
				for (Pass* pass : src)
				{
					auto visitData = VisitData{dst, visitedList, pass};
					if (!visitedList[pass->id()])
					{
						_visit(visitData);
					}
				}

				dst.reverse();
			}

			void getPassDepthsTo(PassDepths& dst, Span<Pass*> sortedPasses)
			{
				auto n = sortedPasses.size();

				dst.resize(n);

				for (Pass* pass : sortedPasses)
				{
					u32 parentDepth = dst[pass->id()];
					for (Pass* runAfter : pass->runAfterThis())
					{
						u32& curDepth = dst[runAfter->id()];
						if (curDepth < parentDepth + 1)
						{
							curDepth = parentDepth + 1;
						}
					}
				}
			}

		private:
			void _visit(VisitData& visitData)
			{
				Pass* pass	= visitData.pass;
				RdgId id	= visitData.pass->id();

				auto& visitedList = visitData.visitedList;
				visitedList[id] = true;

				for (Pass* runAfter : pass->runAfterThis())
				{
					bool isVisited = visitedList[runAfter->id()];
					if (isVisited)
						continue;

					auto next = VisitData{visitData.stack, visitData.visitedList, runAfter};
					_visit(next);
				}

				auto& stack = visitData.stack;
				stack.emplace_back(pass);
			}
		};

		RDS_PROFILE_SECTION("topological sort RdgPass");

		RdgPassSorter sorter;
		sorter.topologicalSort(sortedPasses, passes());
		sorter.getPassDepthsTo(passDepths, sortedPasses);
		RDS_CORE_ASSERT(sortedPasses.size() == passDepths.size(), "");
	}

	// reorder passes
	// TODO
	{

	}

	// create resources
	{
		RDS_PROFILE_SECTION("create resources");

		auto& rscPool = rdgFrame.resourcePool;

		for (auto& e : resources)
		{
			if (e->_pRdRsc || e->_spRdRsc)
				continue;

			StrView name = e->name(); RDS_UNUSED(name);

			switch (e->type())
			{
				case RdgResourceType::Texture:
			{
					auto* rsc = sCast<RdgTexture*>(e);
					auto cDesc = Texture2D::makeCDesc(RDS_SRCLOC);
					cDesc.create(rsc->desc());
					auto* rdRsc = rscPool.createTexture(cDesc, renderDevice());
					rdRsc->setDebugName(rsc->name());
					rsc->commitRenderResouce(rdRsc);
				} break;

				case RdgResourceType::Buffer:
			{
					auto* rsc = sCast<RdgBuffer*>(e);
					auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
					cDesc.create(rsc->desc());
					auto* rdRsc = rscPool.createBuffer(cDesc, renderDevice());
					rdRsc->setDebugName(rsc->name());
					rsc->commitRenderResouce(rdRsc);
				} break;

				default: { RDS_THROW("unknow Render Graph Resource"); } break;
			}
		}

	}

	// export resources
	{
		RDS_PROFILE_SECTION("export resources");
		auto& expBufs = rdgFrame.exportedBuffers;
		auto& expTexs = rdgFrame.exportedTextures;

		for (auto& expBuf : expBufs)
		{
			if (!expBuf.outRdRsc)
				continue;
			(*expBuf.outRdRsc).reset(sCast<RenderGpuBuffer*>(expBuf.rdgRsc->renderResource()));
		}

		for (auto& expTex : expTexs)
		{
			if (!expTex.outRdRsc)
				continue;
			(*expTex.outRdRsc).reset(sCast<Texture2D*>(expTex.rdgRsc->renderResource()));
		}
	}
}

void 
RenderGraph::execute()
{
	RDS_CORE_ASSERT(_rdCtx, "");

	RDS_PROFILE_SCOPED();

	auto	frameCount	= renderContext()->engineFrameCount();
	auto&	rdgFrame	= renderGraphFrame(frameIndex());

	for (auto& pass : rdgFrame.resultPasses)
	{
		const auto& name = pass->name(); RDS_UNUSED(name);
		RDS_PROFILE_DYNAMIC_FMT("{} i[{}] - engineFrame[{}]", name, frameIndex(), frameCount);

		if (pass->isCulled())
			continue;
		pass->execute();
		pass->_rdReq.uploadToGpu();
	}
}

void 
RenderGraph::commit(u32 frameIndex)
{
	// should call in render thread

	auto& rdgFrame	= renderGraphFrame(frameIndex);
	//auto& resources = rdgFrame.resources;

	Passes&		sortedPasses	= rdgFrame.resultPasses;
	PassDepths&	passDepths		= rdgFrame.resultPassDepths;
	{
		/*
		since commit() only called in render thread,
		we only touch (read and write) the RenderResourceState in render thread, all pass is ordered (alias resource may have some problem
			, as they may share the same state but they should not share),
		may rework when impl async compute
		*/
		_setResourcesState(sortedPasses, passDepths);
	}

	_rdCtx->commit(*this, frameIndex);
}

void 
RenderGraph::dumpGraphviz(StrView filename)
{
	RdgDrawer drawer;
	drawer.dump(filename, this);
}

RdgPass& 
RenderGraph::addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlags flag)
{
	SrcLocData srcLocData {RDS_SRCLOC, name.data()};
	return addPass(srcLocData, typeFlag, flag);
}

RdgPass& 
RenderGraph::addPass(const SrcLocData& srcLocData, RdgPassTypeFlags typeFlag, RdgPassFlags flag)
{
	auto* newSrcLocData = _rdCtx->addGpuProfileSection(srcLocData);

	RDS_ASSERT(_rdCtx, "");
	Pass* pass = renderGraphFrame().addPass(this, newSrcLocData, typeFlag, flag);

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
RenderGraph::importTexture(StrView name, TextureT* tex)
{
	RdgTexture_CreateDesc cDesc = {};
	cDesc.create(tex->desc());

	auto	hnd		= createTexture(name, cDesc);
	auto*	rdgTex	= sCast<RdgTexture*>(hnd._rdgRsc);

	// setName here will have sync problem btw
	// TODO: framed _debugName
	//tex->setDebugName(hnd.name());

	rdgTex->_desc = tex->desc();
	rdgTex->setImport(true);
	rdgTex->commitRenderResouce(tex);

	//rdgTex->_stateTrack.setCurrentUsageAccess(lastUsage, lastAccess);

	return hnd;
}

void 
RenderGraph::exportTexture(SPtr<Texture>* out, RdgTextureHnd hnd, TextureUsageFlags usageFlag, Access access)
{
	auto& rdgFrame	= renderGraphFrame(frameIndex());
	auto& exportRsc = rdgFrame.exportedTextures.emplace_back();
	exportRsc.rdgRsc = hnd.resource();
	exportRsc.rdgRsc->setExport(true);

	exportRsc.outRdRsc		= out;
	exportRsc.pendingState	= StateUtil::make(usageFlag, access);
}

void
RenderGraph::exportTexture(RdgTextureHnd hnd, TextureUsageFlags usageFlag, Access access)
{
	exportTexture(nullptr, hnd, usageFlag, access);
}

RdgBufferHnd 
RenderGraph::importBuffer(StrView name, Buffer* buf)
{
	RdgBuffer_CreateDesc cDesc = {};
	cDesc.create(buf->desc());

	auto	hnd		= createBuffer(name, cDesc);
	auto*	rdgBuf	= sCast<RdgBuffer*>(hnd._rdgRsc);

	rdgBuf->_desc = buf->desc();
	rdgBuf->setImport(true);
	rdgBuf->commitRenderResouce(buf);

	//rdgBuf->_stateTrack.setCurrentUsageAccess(lastUsage, lastAccess);

	return hnd;
}

void 
RenderGraph::exportBuffer(SPtr<Buffer>* out, RdgBufferHnd hnd, RenderGpuBufferTypeFlags usageFlag, Access access)
{
	auto& rdgFrame	= renderGraphFrame(frameIndex());
	auto& exportRsc = rdgFrame.exportedBuffers.emplace_back();
	exportRsc.rdgRsc = hnd.resource();
	exportRsc.rdgRsc->setExport(true);

	exportRsc.outRdRsc		= out;
	exportRsc.pendingState	= StateUtil::make(usageFlag, access);
}

RdgTextureHnd RenderGraph::findTexture(StrView name)
{
	RdgTextureHnd o = {};
	for (auto* rsc : resources())
	{
		if (StrUtil::isSame(rsc->name().data(), name.data()))
		{
			o._rdgRsc = rsc;
		}
	}
	return o;
}

RdgBufferHnd RenderGraph::findBuffer(StrView name)
{
	RdgBufferHnd o = {};
	for (auto* rsc : resources())
	{
		if (StrUtil::isSame(rsc->name().data(), name.data()))
		{
			o._rdgRsc = rsc;
		}
	}
	return o;
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
		return _alloc->free(p);
	}
	return DefaultAllocator::free(p);
}

//void RenderGraph::resetFrame()
//{
//	//_passes.clear();
//	//_resources.clear();
//
//	renderGraphFrame().reset();
//}

void RenderGraph::rotateFrame()
{
	auto& frameIdx = _frameIdx;
	frameIdx = RenderApiLayerTraits::rotateFrame(frameIdx + 1);

	if (_rdgFrames.size() < frameIdx + 1)
	{
		// use for first frame
		if (_rdgFrames.is_empty())
			frameIdx = 0;
		auto& back = _rdgFrames.emplace_back();
		back.create(this);
	}
}

void 
RenderGraph::_setResourcesState(const Passes& sortedPasses, const PassDepths& passDepths)
{
	using StateTrack = RdgResourceStateTrack;

	/*
	currently, this solution only works work non-async queue, later study async compute to rework this.
	*/

	for (SizeType i = 0; i < sortedPasses.size(); i++)
	{
		auto*	pass		= sortedPasses[i];
		//auto	passDepth	= passDepths[i];
		auto&	rscAccesses	= pass->_rscAccesses;

		if (pass->isCulled())
			continue;

		for (RdgResourceAccess& rscAccess : rscAccesses)
		{
			auto*		rsc			= rscAccess.rsc;
			//auto		id			= rsc->id();

			#if 0

			StateTrack&	stateTrack	= rsc->stateTrack();

			//RDS_TODO("modify only loop one rscAccesses instead of read write");
			//RdgResourceAccess& rscAccess = rscAccesses.emplace_back();		
			rscAccess.state.srcAccess	= stateTrack.currentAccess();
			rscAccess.state.srcUsage	= stateTrack.currentUsage();

			RDS_TODO("check valid usage");


			bool isValid = stateTrack.setPendingUsageAccess(rscAccess.state.dstUsage, rscAccess.state.dstAccess);
			throwIf(!isValid, "cannot change another access type while the pending access is Write");
			//
			//// temp soulution
			rsc->stateTrack().commit();

			#else

			auto* rdRsc = rsc->renderResource();
			rscAccess.srcState = rdRsc->renderResourceStateFlags();
			rdRsc->_internal_Render_setRenderResourceState(rscAccess.dstState);

			RenderResourceStateFlagsUtil::debugWatcher(rscAccess.srcState);
			RenderResourceStateFlagsUtil::debugWatcher(rscAccess.dstState);

			#endif // 0
		}
	}
}

RenderDevice* RenderGraph::renderDevice() { return _rdCtx->renderDevice(); }

#endif

}