#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rds_fluid_simulation/rdsFluidSim2D_Cpu.h"

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

	UPtr<FluidSim2D_Cpu>	_fluidSim2D_Cpu;
	FluidSimDemo_Base*		_curDemo = nullptr;
};
RDS_DEMO(FluidSimulation);
#endif

}