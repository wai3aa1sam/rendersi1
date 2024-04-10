#include "rds_demo-pch.h"
#include "rdsGraphicsDemo.h"
#include "editor/rdsDemoEditorLayer.h"

namespace rds
{

#if 0
#pragma mark --- rdsGraphicsDemo-Impl ---
#endif // 0
#if 1

GraphicsDemo::~GraphicsDemo()
{

}

void 
GraphicsDemo::onCreate()
{

}

void 
GraphicsDemo::onCreateScene(Scene* oScene)
{
	#if 0
	{
		auto& egCtx = _egCtx;
		egCtx.create();
		_scene.create(egCtx);
		auto n = 1;
		for (size_t i = 0; i < n; i++)
		{
			auto* ent			= _scene.addEntity("");
			ent->id();
			//RDS_CORE_LOG_ERROR("=========");

			auto* rdableMesh	= ent->addComponent<CRenderableMesh>();
			rdableMesh->material;
			rdableMesh->meshAsset->rdMesh;

			auto* transform		= ent->getComponent<CTransform>();
			transform->setLocalPosition(1, 1, 1);
		}
		//RDS_CORE_LOG_ERROR("=========");
	}
	#endif // 0
}

void 
GraphicsDemo::onPrepareRender(RenderGraph* oRdGraph, RenderData& rdData)
{

}

void 
GraphicsDemo::onExecuteRender(RenderGraph* oRdGraph, RenderData& rdData)
{
	//auto& rdGraph = *oRdGraph;
}

void 
GraphicsDemo::onUiMouseEvent(UiMouseEvent& ev)
{

}

void 
GraphicsDemo::onUiKeyboardEvent(UiKeyboardEvent& ev)
{

}

EngineContext& GraphicsDemo::engineContext() { return _demoLayer->engineContext(); }


#endif


}