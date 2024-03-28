#include "rds_engine-pch.h"
#include "rdsEntity.h"
#include "rdsScene.h"

#include "rds_engine/rdsEngineContext.h"
#include "system/rdsCTransformSystem.h"
#include "system/rdsCRenderableSystem.h"


namespace rds
{

#if 0
#pragma mark --- rdsEntity-Impl ---
#endif // 0
#if 1

Entity::Entity()
{

}

Entity::~Entity()
{
	destroy();
}

void 
Entity::create(Scene& scene, EntityId id)
{
	_scene	= &scene;
	_id		= id;

	addComponent<CTransform>();
}

void 
Entity::destroy()
{
	_components.clear();
}

void 
Entity::setName(StrView name)
{
	_name = name;
}

EngineContext&	Entity::engineContext() { return scene().engineContext(); }


#endif

}