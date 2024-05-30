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

	Entity* addEntity(bool hasDefaultName);
	Entity* addEntity(StrView name);

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


#if 0
#pragma mark --- rdsSceneView-Impl ---
#endif // 0
#if 1

class SceneView
{
public:
	void create(Scene* scene, CRenderableSystem* sys);

	void drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData);
	void drawScene(RenderRequest& rdReq, DrawData* drawData);

	Entity* findEntity(EntityId id);

public:
	CRenderableSystem& renderableSystem();

private:
	Scene*				_scene		= nullptr;
	CRenderableSystem*	_rdableSys	= nullptr;
};

inline CRenderableSystem& SceneView::renderableSystem() { return *_rdableSys; }


#endif

}