#include "rds_engine-pch.h"
#include "rdsCRenderableSystem.h"
#include "rds_engine/ecs/component/rdsCRenderable.h"

#include "../rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsCRenderableSystem-Impl ---
#endif // 0
#if 1

//CRenderableSystem* CRenderableSystem::s_instance = nullptr;


CRenderableSystem::CRenderableSystem()
{

}

CRenderableSystem::~CRenderableSystem()
{
	destroy();
}

void 
CRenderableSystem::update()
{
	// transform system update
	{
		_objTransformBuf.resize(renderables().size());

		// or loop dirty transform only
		for (auto* rdable : renderables())
		{
			auto& ent		= rdable->entity();
			auto& transform = ent.transform();

			auto idx = ent.id();
			_objTransformBuf.setValue(idx, transform.localMatrix());
		}
		// for all materials and setParam
	}

	// record command
	{
		auto& rdGraph = renderGraph();

		rdGraph.reset();

		// update

		// present
		{
			//Material* mtl = nullptr;

			//auto* rdCtx = rdGraph.renderContext();
			//rdCtx->drawUI();
		}

		rdGraph.compile();
		rdGraph.execute();

		/*VectorMap<int, SPtr<int>> a;
		a.erase()*/
	}

	_objTransformBuf.uploadToGpu();
}

void
CRenderableSystem::render(RenderRequest& rdReq)
{
	auto& rdGraph = renderGraph();
	
	// rdGraph.execute(); // execute in render thread, then later need to copy struct renderableObject list to render thread, keep simple first
	rdGraph.commit();
}

void 
CRenderableSystem::drawRenderables(RenderRequest& rdReq, Material* mtl)
{
	for (auto* e : renderables())
	{
		e->render(rdReq, mtl);
	}
}

void 
CRenderableSystem::drawRenderables(RenderRequest& rdReq)
{
	for (auto* e : renderables())
	{
		e->render(rdReq);
	}
}


#endif

}