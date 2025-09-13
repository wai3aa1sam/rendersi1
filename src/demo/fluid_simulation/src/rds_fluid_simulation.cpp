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
	RenderUtil::createMaterial(&_shaderFluidSimulation, &_mtlFluidSimulation, "asset/shader/demo/hello_triangle/hello_triangle.shader"
		, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });

	bool is3D = 1;
	is3D ? _curDemo = makeUPtr<FluidSim3D_Gpu>() : _curDemo = makeUPtr<FluidSim2D_Gpu>();
	_curDemo->onCreate(this);
	_fixedDt = 1.0f / 60.0f;
}

void 
FluidSimulation::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	createDefaultScene(oScene, _shaderFluidSimulation, meshAssets().sphere, Vec3u{1, 1, 1});
}

void 
FluidSimulation::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
	if (_curDemo) _curDemo->onPrepareRender(renderPassPipeline);
}

void 
FluidSimulation::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	float maxTimestepFPS = 120.0f;
	float dt = 1.0f / maxTimestepFPS;

	if (_curDemo) _curDemo->onUpdate(dt, renderPassPipeline);
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

void 
FluidSimulation::drawScene(RenderRequest& rdReq, DrawData* drawData) const
{
	drawData->drawScene(rdReq);
}

#endif

}