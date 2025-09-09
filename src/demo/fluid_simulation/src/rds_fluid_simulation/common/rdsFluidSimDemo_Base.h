#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Decl ---
#endif // 0
#if 1

class FluidSimDemo_Base : public NonCopyable
{
public:
	virtual void onCreate(GraphicsDemo* parentDemo)							{ _parentDemo = parentDemo; };

	virtual void onUpdate(float dt)
	{
		if (_parentDemo)
		{
			_mousePosViewport	= _parentDemo->mousePosViewport;
			_mouseRayWorld		= _parentDemo->mouseRayWorldSpace;
		}
	};

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline)	{};
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline)	{};

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) {};

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) {};
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) {};

protected:
			DemoEditorLayer* demoLayer()		{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }
	const	DemoEditorLayer* demoLayer() const	{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }

protected:
	bool isFocusOnEditorViewport()	const	{ return demoLayer() ? demoLayer()->isFocusOnEditorViewport() : false; }

protected:
	GraphicsDemo*	_parentDemo = nullptr;
	Vec2f			_mousePosViewport;
	Ray3f			_mouseRayWorld;
};

#endif

}