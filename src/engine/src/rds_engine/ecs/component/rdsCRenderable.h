#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCComponent.h"

namespace rds
{

class CRenderableSystem;

#if 0
#pragma mark --- rdsCRenderable-Decl ---
#endif // 0
#if 1

class CRenderable : public CComponentT<CRenderable>
{
	friend class CRenderableSystem;
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

	void			render	(RenderRequest& rdReq);
	void			render	(RenderRequest& rdReq, Material* mtl);
	virtual void	onRender(RenderRequest& rdReq, Material* mtl) = 0;

	void setMaterialCommonParam(Material* mtl);

protected:
	CRenderableSystem& renderableSystem();
};


#endif

}