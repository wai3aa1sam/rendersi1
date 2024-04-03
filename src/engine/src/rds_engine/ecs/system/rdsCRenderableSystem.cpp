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
CRenderableSystem::create(EngineContext* egCtx)
{
	Base::create(&engineContext());
	_framedRdReq.resize(s_kFrameInFlightCount);
}

void 
CRenderableSystem::destroy()
{
	_framedRdReq.clear();
	Base::destroy();
}

void 
CRenderableSystem::update()
{
	// transform system update
	{
		_objTransformBuf.resize(renderables().size() + 1);

		// or loop dirty transform only
		for (auto* rdable : renderables())
		{
			auto& ent		= rdable->entity();
			auto& transform = ent.transform();

			auto idx = ent.id();
			_objTransformBuf.setValue(idx, transform.localMatrix());
		}
		// for all materials and setParam
		_objTransformBuf.uploadToGpu();
	}

	// record command
	{
		auto& rdGraph = renderGraph();

		//rdGraph.reset();
		// update
		// present

		rdGraph.compile();
		rdGraph.execute();
	}

}

void
CRenderableSystem::render(RenderContext* rdCtx_, RenderMesh& fullScreenTriangle, Material* mtlPresent)
{
	auto& rdGraph = renderGraph();
	
	// rdGraph.execute(); // execute in render thread, then later need to copy struct renderableObject list to render thread, keep simple first
	rdGraph.commit();

	// present
	{
		auto* rdCtx = rdGraph.renderContext();
		auto& rdReq = renderRequest();
		RDS_CORE_ASSERT(rdCtx == rdCtx_, "");

		rdReq.reset(rdCtx);
		auto* clearValue = rdReq.clearFramebuffers();
		clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
		clearValue->setClearColor(Color4f{0.0f, 0.0f, 0.0f, 1.0f});
		clearValue->setClearDepth(1.0f);

		if (mtlPresent)
		{
			rdReq.drawMesh(RDS_SRCLOC, fullScreenTriangle, mtlPresent);
			rdReq.swapBuffers();
		}
		rdCtx->drawUI(rdReq);		
		rdCtx->commit(rdReq);
	}
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