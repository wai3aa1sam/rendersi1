#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

#include "rdsFluidSim2D_Config.h"
#include "rdsFluidSim2D_ParticleSpawner.h"
#include "rdsFluidSim2D_ParticleDisplay.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Base-Decl ---
#endif // 0
#if 1

class FluidSim2D_Base : public FluidSimDemo_Base
{
public:
	using Base = FluidSimDemo_Base;

	using Config			= FluidSim2D_Config;
	using ParticleSpawner	= FluidSim2D_ParticleSpawner;
	using ParticleDisplay	= FluidSim2D_ParticleDisplay;

public:
	virtual void onCreate(GraphicsDemo* parentDemo) override;

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

public:
	ParticleDisplay& particleDisplay() { return _ptcDisplay; }

protected:
	ParticleSpawner		_particleSpawner;
	ParticleDisplay		_ptcDisplay;
	Config				_simConfig;

	SPtr<Texture2D>		_texColorGradient;
};


#endif

}