#include "rds_fluid_simulation-pch.h"

#include "rds_fluid_simulation.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSimulation-Impl ---
#endif // 0
#if 1

void 
FluidSimulation::onCreate()
{
	Base::onCreate();
	createMaterial(&_shaderFluidSimulation, &_mtlFluidSimulation, "asset/shader/demo/fluid_simulation/fluid_simulation.shader"
					, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });

	_fluidSim2D_Cpu = makeUPtr<FluidSim2D_Cpu>();
	_fluidSim2D_Cpu->onCreate(this);
}

void 
FluidSimulation::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	createDefaultScene(oScene, _shaderFluidSimulation, meshAssets().suzanne, Vec3u{1, 1, 1});
}

void 
FluidSimulation::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
	_fluidSim2D_Cpu->onPrepareRender(renderPassPipeline);
}

void 
FluidSimulation::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	_curDemo = _fluidSim2D_Cpu.ptr();

	if (_curDemo) _curDemo->onUpdate(1.0f / 120.0f);
	if (_curDemo) _curDemo->onExecuteRender(renderPassPipeline);
}

void 
FluidSimulation::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);
	if (_curDemo) _curDemo->onDrawGui(uiDrawReq);
}

void 
FluidSimulation::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
	if (_curDemo) _curDemo->onUiMouseEvent(ev);
}

void 
FluidSimulation::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
	if (_curDemo) _curDemo->onUiKeyboardEvent(ev);
}
#endif

}