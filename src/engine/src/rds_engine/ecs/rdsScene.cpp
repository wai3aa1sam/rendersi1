#include "rds_engine-pch.h"
#include "rdsScene.h"
#include "rds_engine/rdsEngineContext.h"

#include "component/rdsCRenderableMesh.h"
#include "system/rdsCRenderableSystem.h"


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

Entity*
Scene::addEntityWithDefaultName()
{
	auto* ent = addEntity();

	TempString buf;
	fmtTo(buf, "Entity-{}", sCast<u64>(ent->id()));
	ent->setName(buf);

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

}