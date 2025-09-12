#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSimDemo_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSimDemo_Base-Impl ---
#endif // 0
#if 1

void 
FluidSimDemo_Base::onCreate(GraphicsDemo* parentDemo)
{
	_parentDemo = parentDemo;
}

void 
FluidSimDemo_Base::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	if (_parentDemo)
	{
		_mousePosViewport	= _parentDemo->mousePosViewport;
		_mouseRayWorld		= _parentDemo->mouseRayWorldSpace;
	}
	
	if (!_simState.isStop)
	{
		simulate(dt, renderPassPipeline);
		_simState.hasSimulated = true;
	}
	else
	{
		auto newDt = dt;
		if (_simState.isStepBackward) newDt = -newDt;

		bool shdSim = _simState.isStepForward || _simState.isStepBackward;
		if (shdSim) simulate(newDt, renderPassPipeline);

		_simState.hasSimulated		= shdSim;
		_simState.isStepForward		= false;
		_simState.isStepBackward	= false;
	}
}

void 
FluidSimDemo_Base::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{

}

void 
FluidSimDemo_Base::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{

}

void 
FluidSimDemo_Base::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{

}

void 
FluidSimDemo_Base::onUiMouseEvent(UiMouseEvent& ev)
{

}

void 
FluidSimDemo_Base::onUiKeyboardEvent(UiKeyboardEvent& ev)
{

	if (ev.isUp(UiKeyboardEventButton::Space))
	{
		_simState.isStop = !_simState.isStop;
	}

	if (_simState.isStop)
	{
		if (ev.isUp(UiKeyboardEventButton::E))
		{
			_simState.isStepForward = !_simState.isStepForward;
		}

		if (ev.isUp(UiKeyboardEventButton::Q))
		{
			_simState.isStepBackward = !_simState.isStepBackward;
		}
	}
}

void 
FluidSimDemo_Base::simulate(float dt, RenderPassPipeline* renderPassPipeline)
{
}


#endif

}