#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCComponent.h"

namespace rds
{

class CRenderableSystem;
class SceneView;

struct DrawData;
struct DrawSettings;

#if 0
#pragma mark --- rdsCRenderable-Decl ---
#endif // 0
#if 1

class CRenderable : public CComponentT<CRenderable>
{
	friend class CRenderableSystem;
	friend class SceneView;
public:
	using Base		= CComponent;
	using System	= CRenderableSystem;

public:
	static System& getSystem(EngineContext& egCtx);

public:
	CRenderable();
	virtual ~CRenderable();

protected:
	void onCreate(Entity* entity)	override;
	//void onDestroy()				override;

	void			render	(RenderRequest& rdReq, DrawData* drawData, const DrawSettings& drawSettings);
	void			render	(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings);
	virtual void	onRender(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings) = 0;

	void setMaterialCommonParam(Material* mtl, DrawData& drawData);

protected:
	CRenderableSystem& renderableSystem();
};


#endif

}