#include "rds_engine-pch.h"
#include "rdsScene.h"
#include "rds_engine/rdsEngineContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsScene-Impl ---
#endif // 0
#if 1

Scene::Scene()
{

}

Scene::~Scene()
{
	destroy();
}

void 
Scene::create(EngineContext& egCtx)
{
	_egCtx = &egCtx;
	_entVecTable.create(&egCtx.entityAllocator());
}

void 
Scene::destroy()
{
	_entVecTable.destroy();
}

Entity*
Scene::addEntity()
{
	auto& id = ++_nextEntId;
	auto* ent = _entVecTable.newElement<Entity>(id);
	ent->create(*this, id);
	return ent;
}

Entity*
Scene::addEntity(StrView name)
{
	auto* ent = addEntity();
	ent->setName(name);
	return ent;
}

void 
Scene::removeEntity(Entity* entity)
{
	if (!entity)
		return;
	_entVecTable.removeElement(entity->id());
}


#endif


#if 0
#pragma mark --- rdsSceneView-Impl ---
#endif // 0
#if 1

void 
SceneView::drawScene(RenderRequest& rdReq, Material* mtl)
{
	_rdableSys->drawRenderables(rdReq, mtl);
}

void 
SceneView::drawScene(RenderRequest& rdReq)
{
	_rdableSys->drawRenderables(rdReq);
}

#endif

}