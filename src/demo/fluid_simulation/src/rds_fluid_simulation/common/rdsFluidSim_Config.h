#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

class FluidSimDemo_Base;

#if 0
#pragma mark --- rdsFluidSim_SimState-Decl ---
#endif // 0
#if 1

struct FluidSim_SimState
{
	bool isStop			= true;		// : 1
	bool isStepForward	= false;
	bool isStepBackward	= false;
	bool hasSimulated	= false;

	bool isPullInteraction = false;
	bool isPushInteraction = false;

	float interactionInputStrength = 0.0f;
};

#endif

#if 0
#pragma mark --- rdsFluidSim_Config-Decl ---
#endif // 0
#if 1

struct FluidSim_Config
{
public:
	int			simulationCountPerFrame		= 2;

	float		particleMass		= 1.0f;
	float		particleSize		= 0.1f / 2.0f;
	Color4f		particleColor		= Color4f{0.2f, 1.0f, 1.0f, 1.0f};

	Color4f		colorGradientKey0	= Color4f{0.075f, 0.584f, 0.827f, 1.0f};
	Color4f		colorGradientKey1	= Color4f{0.0f, 1.0f, 0.0f, 1.0f};
	Color4f		colorGradientKey2	= Color4f{1.0f, 1.0f, 0.0f, 1.0f};
	Color4f		colorGradientKey3	= Color4f{1.0f, 0.0f, 0.0f, 1.0f};

	float		smoothingRadius			= 0.35f; // 3.0f; //0.35f;
	float		collisionDamping		= 0.95f;
	float		targetDensity			= 55.0f;
	float		pressureMultiplier		= 500.0f;
	float		nearPressureMultiplier	= 5.0f;
	float		viscosityStrength		= 0.03f;

	float		interactionRadius	= 2.0f;
	float		interactionStrength = 90.0f;

	float		timeMultiplier		= 1.0f;
	float		gravity				= 12.0f;
	Vec3f		gravityDir			= Vec3f::s_down();

	Rect2f		boundingRegion		= { Vec2f{0.0, 0.0},	Vec2f{5.0,	2.5} * 2.0f };
	Rect2f		spawnRegion			= { Vec2f{0.0, 0.0},	Vec2f{boundingRegion.size} / 3.0f };
	Rect2f		obstacle			= { Vec2f{0.0, 0.0},	Vec2f{5.0,	2.5} * 2.0f };
	
	AABBox3f		boundingRegion3D		= {  Vec3f{0.0, 0.0, 0.0},	Vec3f{5.0,	2.5, 2.5f} * 1.0f };
	AABBox3f		spawnRegion3D			= {  Vec3f{0.0, 0.0, 0.0},	Vec3f{5.0,	2.5, 2.5f} / 4.0f };
	AABBox3f		obstacle3D				= {  Vec3f{0.0, 0.0, 0.0},	Vec3f{5.0,	2.5, 2.5} * 4.0f };

	bool useSpatialOptimization = 1;
	bool isInvalidateColorMap	= 1;

	bool useDebugLog			= 0;
	bool useDebugSpatial		= 1;
	bool useDebugSmoothRadius	= 1;

public:
	FluidSim_Config();

public:
	void create(FluidSimDemo_Base* sim);

	ColorGradient makeColorGradient();

public:
	float calcPressureByDensity(	float dens);
	float calcNearPressureByDensity(float nearDens);
	Vec2f calcPressureByDensityData(const Vec2f& densData);

public:
	float		debugDensity			= 128.0f;
	Vec2f		debugMousePosViewport	= Vec2f::s_zero();

	Vec3f		debugMousePosWorld		= Vec3f::s_zero();
	Vec3f		debugMouseDirWorld		= Vec3f::s_zero();

	u32			debugParticleCount		= 0;
	u32			debugWithinRadiusCount	= 0;

public:
	void drawGui(EditorUiDrawRequest& uiDrawReq);

private:
	FluidSimDemo_Base* _fluSim = nullptr;
};

#endif



}