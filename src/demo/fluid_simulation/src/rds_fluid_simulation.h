#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rds_fluid_simulation/2d/rdsFluidSim2D.h"
#include "rds_fluid_simulation/3d/rdsFluidSim3D.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSimulation-Decl ---
#endif // 0
#if 1
class FluidSimulation : public GraphicsDemo
{
public:
	using Base = GraphicsDemo;

public:
	virtual void onCreate()								override;
	virtual void onCreateScene(Scene* oScene)			override;
	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

protected:
	SPtr<Shader>	_shaderFluidSimulation;
	SPtr<Material>	_mtlFluidSimulation;
	SPtr<Texture2D>	_texUvChecker;

	//UPtr<FluidSim2D_Gpu>		_fluidSim2D;
	//UPtr<FluidSim3D_Gpu>		_fluidSim3D;
	UPtr<FluidSimDemo_Base>		_curDemo = nullptr;
	float _fixedDt = 0.0;
};
RDS_DEMO(FluidSimulation);
#endif

}