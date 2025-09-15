#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rds_fluid_simulation/common/rdsFluidSimDemo_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim3D_Base-Decl ---
#endif // 0
#if 1

class FluidSim3D_Base : public FluidSimDemo_Base
{
public:
	using Base = FluidSimDemo_Base;

public:
	virtual void onCreate(GraphicsDemo* parentDemo) override;
	virtual void onCreateScene(Scene* oScene) override;

	virtual void onUpdate(float dt, RenderPassPipeline* renderPassPipeline) override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
	void debug_drawBoundary(RenderRequest& rdReq);
	void debug_drawSpatialGrid(RenderRequest& rdReq);
	void debug_drawMouseInteraction(RenderRequest& rdReq);
	void debug_drawSmoothRadius(RenderRequest& rdReq);
};


#endif

}