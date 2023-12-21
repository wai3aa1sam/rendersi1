#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraph.h"
#include "rdsRenderGraphDrawer.h"
#include "rds_render_api_layer/rdsRenderContext.h"

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
RenderGraph::compile()
{
	/*
	since we have multi frame, all the RdgPass and RdgResource can not reuse (after hash and confirm same as last frame)
	, only the order (orderedPassId) can be reuse
	*/
	auto& rdgFrame	= renderGraphFrame();
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

		RdgPassSorter sorter;
		sorter.topologicalSort(sortedPasses, passes());
		sorter.getPassDepthsTo(passDepths, sortedPasses);
		RDS_CORE_ASSERT(sortedPasses.size() == passDepths.size(), "");
	}

	// reorder passes

	// set rsc state
	_setResourcesState(sortedPasses, passDepths);

	// create resource (_commitResouces())
	// could loop _resources, but all rsc should have a _isCulled
	// could loop pass for rdTarget, depthStencil, writes...
	for (auto& e : resources)
	{
		switch (e->type())
		{
			case RdgResourceType::Texture:
			{
				auto* rsc = sCast<RdgTexture*>(e);
				auto cDesc = Texture2D_CreateDesc{rsc->desc()};
				rsc->commitResouce(resourcePool().createTexture(cDesc, renderDevice()));
			} break;

			case RdgResourceType::Buffer:
			{
				auto* rsc = sCast<RdgBuffer*>(e);
				auto cDesc = RenderGpuBuffer_CreateDesc{rsc->desc()};
				rsc->commitResouce(resourcePool().createBuffer(cDesc, renderDevice()));
			} break;

			default: { RDS_THROW("unknow Render Graph Resource"); } break;
		}
	}
}

void 
RenderGraph::execute()
{
	RDS_CORE_ASSERT(_rdCtx, "");

	for (auto& e : resultPasses())
	{
		if (e->isCulled())
			continue;
		e->execute();
	}
}

void 
RenderGraph::commit()
{
	_rdCtx->commit(*this);
	rotateFrame();
}

void 
RenderGraph::dumpGraphviz(StrView filename)
{
	RdgDrawer drawer;
	drawer.dump(filename, this);
}

RdgPass& 
RenderGraph::addPass(StrView name, RdgPassTypeFlags typeFlag, RdgPassFlag flag)
{
	Pass* pass = renderGraphFrame().addPass(name, typeFlag, flag);
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
	hnd._rdgRsc->setImport(true);

	return hnd;
}

void 
RenderGraph::exportTexture(Texture* out, RdgTextureHnd hnd)
{
	hnd._rdgRsc->setExport(true);
}

RdgBufferHnd 
RenderGraph::importBuffer(StrView name, Buffer* buf)
{
	RdgBuffer_CreateDesc cDesc;
	cDesc = buf->desc();
	cDesc.typeFlags |= RenderGpuBufferTypeFlags::Compute;
	
	auto hnd = createBuffer(name, cDesc);
	hnd._rdgRsc->setImport(true);

	return hnd;
}

void 
RenderGraph::exportBuffer(Buffer* out, RdgBufferHnd hnd)
{
	hnd._rdgRsc->setExport(true);
}

RdgTextureHnd RenderGraph::findTexture(StrView name)
{
	RDS_NOT_YET_SUPPORT();
	return RdgTextureHnd();
}

RdgBufferHnd RenderGraph::findBuffer(StrView name)
{
	RDS_NOT_YET_SUPPORT();
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

//void RenderGraph::resetFrame()
//{
//	//_passes.clear();
//	//_resources.clear();
//
//	renderGraphFrame().reset();
//}

void RenderGraph::rotateFrame()
{
	_frameIdx = (_frameIdx + 1) % s_kFrameInFlightCount;

	if (_rdgFrames.size() < _frameIdx + 1)
	{
		// use for first frame
		if (_rdgFrames.is_empty())
			_frameIdx = 0;
		auto& back = _rdgFrames.emplace_back();
		back.create(this);
	}
}

void 
RenderGraph::_setResourcesState(const Passes& sortedPasses, const PassDepths& passDepths)
{
	using StateTrack = RdgResourceStateTrack;

	//u32 curPassDepth = 0;
	auto n = renderGraphFrame().resources.size();

	PendingResources pendingRscs;
	pendingRscs.reserve(n);

	for (SizeType i = 0; i < sortedPasses.size(); i++)
	{
		auto*	pass		= sortedPasses[i];
		//auto	passDepth	= passDepths[i];
		auto&	rscAccesses	= pass->_rscAccesses;

		if (pass->isCulled())
			continue;

		/*if (curPassDepth != passDepth)
		{
			_flushResourcesState(pendingRscs);
			curPassDepth = passDepth;
		}*/

		for (auto& rscAccess : rscAccesses)
		{
			auto*		rsc			= rscAccess.rsc;
			//auto		id			= rsc->id();
			StateTrack&	stateTrack	= rsc->stateTrack();

			//RDS_TODO("modify only loop one rscAccesses instead of read write");
			//RdgResourceAccess& rscAccess = rscAccesses.emplace_back();		
			rscAccess.state.srcAccess	= stateTrack.currentAccess();
			rscAccess.state.srcUsage	= stateTrack.currentUsage();

			/*
			currently, this solution only works work non-async queue, later study async compute to rework this.
			*/

			bool isValid = stateTrack.setPendingUsageAccess(rscAccess.state.dstUsage, rscAccess.state.dstAccess);
			throwIf(!isValid, "cannot change another access type while the pending access is Write");
			pendingRscs.emplace_back(rsc);

			// temp soulution
			rsc->stateTrack().commit();
		}

		/*for (auto* write : pass->writes())
		{
			auto*				rsc			= write;
			RdgResourceState&	curState	= rsc->state();

			RDS_TODO("modify only loop one rscAccesses instead of read write");
			auto& rscAccess = rscAccesses.emplace_back();		
			rscAccess.srcAccess = curState.currentAccess();
			rscAccess.dstAccess = RdgAccess::Write;

			bool isValid = curState.setPendingAccess(rscAccess.dstAccess);
			throwIf(!isValid, "cannot change another access type while the pending access is Write");
			pendingRscs.emplace_back(rsc);
		}*/
	}

	//_flushResourcesState(pendingRscs);
}

void 
RenderGraph::_flushResourcesState(PendingResources& pendingRscs)
{
	for (auto& rsc : pendingRscs)
	{
		rsc->stateTrack().commit();
	}
	pendingRscs.clear();
}

RenderDevice* RenderGraph::renderDevice() { return _rdCtx->renderDevice(); }

#endif

}