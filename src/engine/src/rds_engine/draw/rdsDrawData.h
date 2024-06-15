#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include <rds_render_api_layer/graph/rdsRenderGraphResource.h>

namespace rds
{

class	SceneView;
struct	MeshAssets;
struct	DrawSettings;

#if 0
#pragma mark --- rdsDrawData-Decl ---
#endif // 0
#if 1

/*
* TODO: should put on rds_render later, drawScene then need to be virtual
*/
struct DrawData
{
	u32 drawParamIdx = 0;

	float			deltaTime   = 0.0f;
	SceneView*		sceneView	= nullptr;
	math::Camera3f*	camera		= nullptr;

	MeshAssets*		meshAssets	= nullptr;

	RdgTextureHnd oTexPresent;

public:
	void drawScene(			RenderRequest& rdReq, Material* mtl);
	void drawScene(			RenderRequest& rdReq);
	void drawScene(			RenderRequest& rdReq, Material* mtl, const DrawSettings& drawSettings);
	void drawScene(			RenderRequest& rdReq, const DrawSettings& drawSettings);
	void drawSceneAABBox(	RenderRequest& rdReq, const DrawSettings& drawSettings);

	void setupDrawParam(DrawParam*	oDrawParam);
	void setupMaterial(	Material*	oMtl);

public:
	const Tuple2f& resolution() const;

	Material* mtlLine();

public:
	SPtr<Material> _mtlLine;
};


#endif

}