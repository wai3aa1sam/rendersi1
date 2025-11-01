#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

#include "rds_fluid_simulation/common/rdsFluidSim_ParticleDisplay.h"
#include "rds_fluid_simulation/common/rdsFluidSim_ParticleSpawner.h"
#include "rds_fluid_simulation/common/rdsFluidSim_Config.h"

namespace rds
{


#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Decl ---
#endif // 0
#if 1

class FluidSimDemo_Base : public NonCopyable
{
public:
	using SimState			= FluidSim_SimState;
	using ParticleDisplay	= FluidSim_ParticleDisplay;
	using ParticleSpawner   = FluidSim_ParticleSpawner;
	using Config			= FluidSim_Config;

public:
	FluidSimDemo_Base();
	virtual ~FluidSimDemo_Base();

public:
	virtual void onCreate(GraphicsDemo* parentDemo);
	virtual void onCreateScene(Scene* oScene);

	virtual void onUpdate(float dt, RenderPassPipeline* renderPassPipeline);

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline);
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline);

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq);

	virtual void onUiMouseEvent(	UiMouseEvent&		ev);
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev);

public:
	virtual void simulate(float dt, RenderPassPipeline* renderPassPipeline);

protected:
			DemoEditorLayer* demoLayer()		{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }
	const	DemoEditorLayer* demoLayer() const	{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }

protected:
	bool isFocusOnEditorViewport()	const	{ return demoLayer() ? demoLayer()->isFocusOnEditorViewport() : false; }

public:
	ParticleDisplay&	particleDisplay() { return _ptcDisplay; }

	CTransform*			getDebugSpatialTransform();
	CTransform*			getInteractionTransform();
	CTransform*			getForceFieldTransform();
	CTransform*			getBoundingBoxTransform();

protected:
	GraphicsDemo*	_parentDemo = nullptr;
	Vec2f			_mousePosViewport;
	Ray3f			_mouseRayWorld;

	Entity* _ent_boundingBox		= nullptr;
	Entity* _ent_interaction		= nullptr;
	Entity* _ent_forceField			= nullptr;
	Entity* _ent_debugSpatialLut	= nullptr;

protected:
	SPtr<Texture2D>		_texColorGradient;

	ParticleDisplay		_ptcDisplay;
	ParticleSpawner		_particleSpawner;
	Config				_simConfig;

protected:
	SimState _simState;
};

#endif

}