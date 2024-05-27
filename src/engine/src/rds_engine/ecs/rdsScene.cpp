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

	#if 0
	TempString buf;
	fmtTo(buf, "Entity-{}", sCast<u64>(id));
	ent->setName(buf);
	#endif // 0

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

Entity* 
Scene::findEntity(EntityId id)
{
	return _entVecTable.findElement(id);

}

const Entity* 
Scene::findEntity(EntityId id) const
{
	return constCast(_entVecTable).findElement(id);
}

#endif


#if 0
#pragma mark --- rdsSceneView-Impl ---
#endif // 0
#if 1

void 
SceneView::SceneView::create(Scene* scene, CRenderableSystem* sys) 
{
	_scene		= scene;
	_rdableSys	= sys; 
}

void 
SceneView::drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData)
{
	RDS_CORE_ASSERT(mtl && drawData, "draw scene fail");
	auto& rdableSys = renderableSystem();

	drawData->setupMaterial(mtl);
	for (auto* e : rdableSys.renderables())
	{
		e->render(rdReq, mtl, nullptr);
	}
}

void 
SceneView::drawScene(RenderRequest& rdReq, DrawData* drawData)
{
	RDS_CORE_ASSERT(drawData, "draw scene fail");

	auto& rdableSys = renderableSystem();

	for (auto* e : rdableSys.renderables())
	{
		e->render(rdReq, drawData);
	}
}

Entity* 
SceneView::findEntity(EntityId id)
{
	return _scene->findEntity(id);
}


#endif

}