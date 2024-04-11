#include "rds_engine-pch.h"
#include "rdsCRenderable.h"
#include "rds_engine/rdsEngineContext.h"
#include "rds_engine/ecs/system/rdsCRenderableSystem.h"
#include "rds_engine/ecs/rdsScene.h"

namespace rds
{

#if 0
#pragma mark --- rdsCRenderable-Impl ---
#endif // 0
#if 1

CRenderable::System&	CRenderable::getSystem(EngineContext& egCtx)	{ return egCtx.renderableSystem(); }


CRenderable::CRenderable()
{
	//engineContext()->
}

CRenderable::~CRenderable()
{
	renderableSystem().deleteComponent(id());
}

void 
CRenderable::onCreate(Entity* entity) 
{
	Base::onCreate(entity);
}

//void 
//CRenderable::onDestroy()
//{
//	
//}

void 
CRenderable::render(RenderRequest& rdReq, Material* mtl, DrawData* drawData)
{
	onRender(rdReq, mtl, drawData);
}

void			
CRenderable::render(RenderRequest& rdReq, DrawData* drawData)
{
	onRender(rdReq, nullptr, drawData);
}

void 
CRenderable::setMaterialCommonParam(Material* mtl, DrawData& drawData)
{
	drawData.setupMaterial(mtl);
}

CRenderableSystem&		CRenderable::renderableSystem()					{ return getSystem(engineContext()); }


#endif

}