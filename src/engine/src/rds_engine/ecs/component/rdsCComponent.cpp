#include "rds_engine-pch.h"
#include "rdsCComponent.h"
#include "../rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsCComponent-Impl ---
#endif // 0
#if 1

CComponent::~CComponent()
{

}

void 
CComponent::create(Entity* entity)
{
	onCreate(entity);
}

//void 
//CComponent::destroy()
//{
//	onDestroy();
//}

void 
CComponent::onCreate(Entity* entity)
{
	_entity = entity;
	//_engCtx = &entity->engineContext();
}

//void 
//CComponent::onDestroy()
//{
//
//}

EngineContext&	CComponent::engineContext() { return entity().engineContext(); }
EntityId		CComponent::id()			{ return entity().id(); }

#endif

}