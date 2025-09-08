#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

struct FluidSim2DConfig
{
public:
	float		particleMass		= 1.0f;
	float		particleSize		= 0.1f / 2.0f;
	Color4f		particleColor		= Color4f{0.2f, 1.0f, 1.0f, 1.0f};

	float		smoothingRadius		= 2.f;
	float		collisionDamping	= 0.95f;
	float		targetDensity		= 2.7f;
	float		pressureMultiplier	= 500.0f;

	float		timeMultiplier		= 1.0f;
	float		gravity				= 9.81f;
	Vec2f		gravityDir			= Vec2f::s_down();

	Rect2f		boundingRegion		= { Vec2f{0.0, 0.0},	Vec2f{5.0,	2.5} * 2.0f };
	Rect2f		spawnRegion			= { Vec2f{0.0, 0.0},	Vec2f{boundingRegion.size} / 4.0f };

	bool useSpatialOptimization = 1;
	bool useDebugLog			= 0;
	bool useDebugSpatial		= 0;

public:
	FluidSim2DConfig()
	{

	}

public:
	float calcPressureByDensity(float dens);

public:
	float		debugDensity			= 128.0f;
	Vec2f		debugMousePosViewport	= Vec2f::s_zero();

	Vec3f		debugMousePosWorld		= Vec3f::s_zero();
	Vec3f		debugMouseDirWorld		= Vec3f::s_zero();

	u32			debugParticleCount		= 0;
	u32			debugWithinRadiusCount	= 0;

public:
	void drawGui(EditorUiDrawRequest& uiDrawReq);
};

class ParticleSpawner2D
{
public:
	Vec2f initVelocity		= Vec2f{0.0, 0.0};
	float jitterFct			= 1.0;
	float spawnDensity		= 128; // 128.0;

	Rect2f spawnRegion;

public:
	struct SpawnArgs
	{
		Vector<Vec2f>& outPositions;
		Vector<Vec2f>& outVelocities;
		Vector<float>& outDensities;
	};
	u32		spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities);
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
	
	struct SimState
	{
		bool isStop			= true;
		bool isStepForward	= false;
		bool isStepBackward	= false;
	};
	SimState _simState;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)						override;

	virtual void onUpdate(float dt)										override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
	using DimT		= Vec2f;
	using DimT_i	= Vec2i;
	using DimT_u	= Vec2u;
	using IdxT		= u32;
	using HashT		= IdxT;

	using SizeT = DemoTraits::SizeType;

	void	update(float dt);
	void	simulate(float dt);
	void	resolveCollisions(DimT& outPos, DimT& outVel);

	float	smoothingKernel(float radius, float dist);
	float	smoothingKernelDerivative(float radius, float dist);
	float	calcSharedPressure(float densityA, float densityB);

	float	calcDensity(		SizeT tarParticleIdx);
	DimT	calcGradient(		SizeT tarParticleIdx);
	DimT	calcPressureForce(	SizeT tarParticleIdx);

	float	_calcDensity_Spatial(		SizeT tarParticleIdx);
	DimT	_calcGradient_Spatial(		SizeT tarParticleIdx);
	DimT	_calcPressureForce_Spatial(	SizeT tarParticleIdx);

	float	_calcDensity_Raw(		SizeT tarParticleIdx);
	DimT	_calcGradient_Raw(		SizeT tarParticleIdx);
	DimT	_calcPressureForce_Raw(	SizeT tarParticleIdx);

public:
	void logDebugSpatial();

public:
	struct NeighbourInfo
	{
		IdxT	index;
		float	distance;
		DimT	direction;
	};
	void	foreachPointWithinRadius(SizeT tarParticleIdx, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback);
	void	debugForeachPointWithinRadius(const DimT& samplingPt, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback);

	void	updateSpatialLut(const Vector<DimT>& pts, float radius);
	DimT_i	positionToCellCoord(const DimT& pt, float radius);
	IdxT	calcCellKeyByCellCoord(const DimT_i& cellCoord);
	HashT	hashCellCoord(const DimT_i& cellCoord);

private:
	Vector<DimT>	_positions;
	Vector<DimT>	_predictedPositions;

	Vector<DimT>	_velocities;
	Vector<float>	_densities;

	Vector<DimT_u>	_spatialLut;
	Vector<IdxT>	_cellKeyStartIndices;
	Vector<DimT_i>	_debugCellCoords;

protected:
	SPtr<Shader>	_shaderFluidSimulation;
	SPtr<Material>	_mtlFluidSimulation;

	ParticleSpawner2D	_particleSpawner;
	FluidSim2DConfig	_simConfig;
};
#endif

}