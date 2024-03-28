#include "rds_engine-pch.h"
#include "rdsCRenderable.h"
#include "rds_engine/rdsEngineContext.h"
#include "rds_engine/ecs/system/rdsCRenderableSystem.h"

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
CRenderable::render(RenderRequest& rdReq, Material* mtl)
{
	onRender(rdReq, mtl);
}

void			
CRenderable::render(RenderRequest& rdReq)
{
	onRender(rdReq, nullptr);
}

void 
CRenderable::setMaterialCommonParam(Material* mtl)
{
	mtl->setParam("rds_localTransforms", &renderableSystem()._objTransformBuf.gpuBuffer());
}

CRenderableSystem&		CRenderable::renderableSystem()					{ return getSystem(engineContext()); }


#endif

}