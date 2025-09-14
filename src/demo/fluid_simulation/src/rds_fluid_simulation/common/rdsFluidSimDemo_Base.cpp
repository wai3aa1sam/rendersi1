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
FluidSimDemo_Base::onCreateScene(Scene* oScene)
{
	auto fn_createObject = [&](StrView name, Shader* shader, MeshAsset* mesh)
		{
			auto* ent = oScene->addEntity(name);
			auto* rdableMesh = ent->addComponent<CRenderableMesh>();
			rdableMesh->material	= Renderer::renderDevice()->createMaterial(shader);
			rdableMesh->meshAsset	= mesh;
			return ent;
		};
	
	_ent_boundingBox		= fn_createObject("bounding_box",		_parentDemo->_shaderWire,	_parentDemo->meshAssets().box);
	_ent_interaction		= fn_createObject("interaction",		_parentDemo->_shaderWire,	_parentDemo->meshAssets().sphere);
	_ent_debugSpatialLut	= fn_createObject("debugSpatialLut",	_parentDemo->_shaderWire,	_parentDemo->meshAssets().sphere);
}

void 
FluidSimDemo_Base::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	if (_parentDemo)
	{
		_mousePosViewport	= _parentDemo->mousePosViewport;
		_mouseRayWorld		= _parentDemo->mouseRayWorldSpace;
	}
	
	dt = dt * _simConfig.timeMultiplier / _simConfig.simulationCountPerFrame;

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

CTransform* 
FluidSimDemo_Base::getDebugSpatialTransform()
{
	return _ent_debugSpatialLut->getComponent<CTransform>();
}

CTransform* 
FluidSimDemo_Base::getInteractionTransform()
{
	return _ent_interaction->getComponent<CTransform>();
}

CTransform* 
FluidSimDemo_Base::getBoundingBoxTransform()
{
	return _ent_boundingBox->getComponent<CTransform>();
}

#endif

}