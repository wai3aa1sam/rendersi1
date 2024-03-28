#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsScene-Decl ---
#endif // 0
#if 1

class Scene : public NonCopyable
{
	RDS_ENGINE_COMMON_BODY();
public:
	using EntityVecTable = EcsVectorTable<Entity, SPtr<Entity/*, DefaultDestructor<Entity>*/ > >;

public:
	Scene();
	~Scene();

	void create(EngineContext& egCtx);
	void destroy();

	Entity* addEntity();
	Entity* addEntity(StrView name);

	void removeEntity(Entity* entity);

public:
	EngineContext& engineContext();

protected:
	EngineContext*	_egCtx = nullptr;
	EntityVecTable	_entVecTable;
	EntityId		_nextEntId;		// use a freelist to recycle the id
};

inline EngineContext& Scene::engineContext() { return *_egCtx; }


#endif

}