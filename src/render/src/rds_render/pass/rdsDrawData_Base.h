#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render_api_layer/graph/rdsRenderGraphResource.h"

namespace rds
{

struct DrawSettings;

#if 0
#pragma mark --- rdsDrawData_Base-Decl ---
#endif // 0
#if 1

class DrawData_Base : public NonCopyable
{
public:
	u32				drawParamIdx	= 0;
	float			deltaTime		= 0.0f;
	math::Camera3f*	camera			= nullptr;

	RdgTextureHnd	renderTargetColor;
	RdgTextureHnd	depthStencilBuffer;

public:
	virtual ~DrawData_Base() = default;

public:
	virtual void drawScene(RenderRequest& rdReq, Material* mtl)										= 0;
	virtual void drawScene(RenderRequest& rdReq, Material* mtl, const DrawSettings& drawSettings)	= 0;

	virtual void setupMaterial(Material* oMtl) = 0;

public:
	Tuple2f	resolution()	const;
	Tuple2u	resolution2u()	const;

protected:
	
};


#endif

}