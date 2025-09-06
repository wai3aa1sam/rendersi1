#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{


struct FluidSim2DConfig
{
public:
	float		particleMass		= 1.0f;
	float		particleSize		= 0.1f;
	Color4f		particleColor		= Color4f{0.2f, 1.0f, 0.2f, 1.0f};

	float		smoothingRadius		= 1.2f;
	float		collisionDamping	= 0.95f;
	float		targetDensity		= 2.7f;
	float		pressureMultiplier	= 500.0f;

	float		timeMultiplier		= 1.0f;
	float		gravity				= 9.81f;
	Vec2f		gravityDir			= Vec2f::s_down();

	Rect2f		boundingRegion		= { Vec2f{0.0, 0.0},	Vec2f{5.0,	2.5} * 2.0f };
	Rect2f		spawnRegion			= { Vec2f{0.0, 0.0},	Vec2f{boundingRegion.size} / 4.0f };

public:
	FluidSim2DConfig()
	{

	}

public:
	float calcPressureByDensity(float dens);

public:
	float		debugDensity			= 1.0f;
	Vec2f		debugMousePosViewport	= Vec2f::s_zero();

	Vec3f		debugMousePosWorld		= Vec3f::s_zero();
	Vec3f		debugMouseDirWorld		= Vec3f::s_zero();

public:
	void drawGui(EditorUiDrawRequest& uiDrawReq);
};

class ParticleSpawner2D
{
public:
	Vec2f initVelocity		= Vec2f{0.0, 0.0};
	float jitterFct			= 1.0;
	float spawnDensity		= 128.0;

	Rect2f spawnRegion;

public:
	struct SpawnArgs
	{
		Vector<Vec2f>& outPositions;
		Vector<Vec2f>& outVelocities;
		Vector<float>& outDensities;
	};
	u32		spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities);
	Vec2i	calcSpawnCountPerAxis() const;
};

#if 0
#pragma mark --- rdsFluidSimulation-Decl ---
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
	GraphicsDemo*	_parentDemo = nullptr;
	Vec2f			_mousePosViewport;
	Ray3f			_mouseRayWorld;
};

class FluidSim2D_Cpu : public FluidSimDemo_Base
{
public:
	using Base = FluidSimDemo_Base;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)						override;

	virtual void onUpdate(float dt)										override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
	using DimT	= Vec2f;
	using SizeT = DemoTraits::SizeType;

	void	update(float dt);
	void	simulate(float dt);
	void	resolveCollisions(DimT& outPos, DimT& outVel);

	float	smoothingKernel(float radius, float dist);
	float	smoothingKernelDerivative(float radius, float dist);
	float	calcSharedPressure(float densityA, float densityB);

	float	calcDensity(	const DimT& samplingPt);
	DimT	calcGradient(		SizeT tarParticleIdx);
	DimT	calcPressureForce(	SizeT tarParticleIdx);

private:
	Vector<DimT>	_positions;
	Vector<DimT>	_velocities;
	Vector<float>	_densities;

protected:
	SPtr<Shader>	_shaderFluidSimulation;
	SPtr<Material>	_mtlFluidSimulation;

	ParticleSpawner2D	_particleSpawner;
	FluidSim2DConfig	_simConfig;
};
#endif

}