#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSimDemo_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSimDemo_Base-Impl ---
#endif // 0
#if 1

FluidSimDemo_Base::FluidSimDemo_Base()
{
}

FluidSimDemo_Base::~FluidSimDemo_Base()
{
}

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
			rdableMesh->material	= Renderer::renderDevice()->createMaterial(RDS_DebugLabel(), shader);
			rdableMesh->meshAsset	= mesh;
			return ent;
		};
	
	_ent_boundingBox		= fn_createObject("bounding_box",		_parentDemo->_shaderWire,	_parentDemo->meshAssets().box);
	_ent_interaction		= fn_createObject("interaction",		_parentDemo->_shaderWire,	_parentDemo->meshAssets().sphere);
	_ent_forceField			= fn_createObject("forceField",			_parentDemo->_shaderWire,	_parentDemo->meshAssets().box);
	_ent_debugSpatialLut	= fn_createObject("debugSpatialLut",	_parentDemo->_shaderWire,	_parentDemo->meshAssets().sphere);

	auto light_ents = _parentDemo->createLights(oScene, Vec3u{ 2, 1, 1 }, Vec3f::s_zero(), Vec3f::s_one(), Quat4f::s_eulerDeg(Vec3f{66.099f, 26.20f, 0.0f}));

	// set default value for lights in d
	{
		light_ents[0]->getComponent<CTransform>()->setLocalPosition(Vec3f{ 19.6f, 0.0f, -3.8f });
		light_ents[0]->getComponent<CLight>()->setIntensity(2.1f);

		light_ents[1]->getComponent<CTransform>()->setLocalPosition(Vec3f{0.0f, 0.6f, -1.0f});
		light_ents[1]->getComponent<CLight>()->setColor(Color4f{ 0.0f, 0.1f, 1.0f, 1.0f });
		light_ents[1]->getComponent<CLight>()->setRange(19.7f);
		light_ents[1]->getComponent<CLight>()->setIntensity(6.1f);
	}
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
FluidSimDemo_Base::getForceFieldTransform()
{
	return _ent_forceField->getComponent<CTransform>();
}

CTransform* 
FluidSimDemo_Base::getBoundingBoxTransform()
{
	return _ent_boundingBox->getComponent<CTransform>();
}

#endif

}