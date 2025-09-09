#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_ParticleSpawner-Decl ---
#endif // 0
#if 1

class FluidSim2D_ParticleSpawner
{
public:
	Vec2f initVelocity		= Vec2f{0.0, 0.0};
	float jitterFct			= 0.03f;
	float spawnDensity		= 160.0f; // 128.0;

	Rect2f spawnRegion;

public:
	struct SpawnArgs
	{
		Vector<Vec2f>& outPositions;
		Vector<Vec2f>& outVelocities;
		Vector<float>& outDensities;
	};
	u32		spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities, Vector<Vec2f>& outDensityData);
	Vec2i	calcSpawnCountPerAxis() const;
};


#endif

}