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
	Entity* addEntityWithDefaultName();

	void removeEntity(Entity* entity);

			Entity* findEntity(EntityId id);
	const	Entity* findEntity(EntityId id) const;

public:
	EngineContext& engineContext();

	Span<		Entity*> entities();
	Span<const	Entity*> entities() const;

protected:
	EngineContext*	_egCtx = nullptr;
	EntityVecTable	_entVecTable;
	EntityId		_nextEntId;		// use a freelist to recycle the id
};

inline EngineContext& Scene::engineContext() { return *_egCtx; }

inline Span<		Entity*> Scene::entities()			{ return _entVecTable.elements(); }
inline Span<const	Entity*> Scene::entities() const	{ return spanConstCast<const Entity*>(_entVecTable.elements()); }


#endif

}