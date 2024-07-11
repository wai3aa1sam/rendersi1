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
class DrawData : public DrawData_Base
{
public:
	SceneView*		sceneView	= nullptr;
	MeshAssets*		meshAssets	= nullptr;
	RdgTextureHnd	oTexPresent;

public:
	virtual void drawScene(RenderRequest& rdReq, Material* mtl)										override;
	virtual void drawScene(RenderRequest& rdReq, Material* mtl, const DrawSettings& drawSettings)	override;
	virtual void drawScene(RenderRequest& rdReq, const DrawSettings& drawSettings)					override;

			void drawScene(			RenderRequest& rdReq);
			void drawSceneAABBox(	RenderRequest& rdReq, const DrawSettings& drawSettings);

	virtual void setupMaterial(	Material*	oMtl) override;

	void setupDrawParam(DrawParam*	oDrawParam);

public:
	Material* mtlLine();

public:
	SPtr<Material> _mtlLine;
};


#endif

}