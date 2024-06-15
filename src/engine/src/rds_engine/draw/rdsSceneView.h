#pragma once

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{

class Scene;
class CRenderableSystem;

struct DrawSettings;

#if 0
#pragma mark --- rdsSceneView-Decl ---
#endif // 0
#if 1

class SceneView
{
public:
	void create(Scene* scene, CRenderableSystem* sys);

	void drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData);
	void drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings);
	void drawScene(RenderRequest& rdReq, DrawData* drawData, const DrawSettings& drawSettings);

	void drawSceneAABBox(RenderRequest& rdReq, DrawData* drawData, const DrawSettings& drawSettings);

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