#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim_ParticleSpawner-Decl ---
#endif // 0
#if 1

class FluidSim_ParticleSpawner
{
public:
	Vec2f initVelocity		= Vec2f{0.0, 0.0};
	float jitterFct			= 0.03f;
	float spawnDensity		= 128.0f; // 2.0f // 128.0;

	Rect2f spawnRegion;
	AABBox3f spawnRegion3D;

	u32 particleCount = 0;

public:
	void create(const Rect2f& spawnRegion_);
	void create(const AABBox3f& spawnRegion_);

	struct SpawnArgs
	{
		Vector<Vec2f>& outPositions;
		Vector<Vec2f>& outVelocities;
		Vector<float>& outDensities;
	};
	u32		spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities, Vector<Vec2f>& outDensityData);
	u32		spawnTo(RenderGpuBuffer* o_positions, RenderGpuBuffer* o_velocities, RenderGpuBuffer* o_predictedPositions);
	u32		spawnTo(RdgBufferHnd bufPos);

	u32		spawnTo3D(RenderGpuBuffer* o_positions, RenderGpuBuffer* o_velocities, RenderGpuBuffer* o_predictedPositions);

public:
	Vec2i	calcSpawnCountPerAxis2D() const;
	Vec3i	calcSpawnCountPerAxis3D() const;

private:
	u32 _spawnTo_Positions(Vector<Vec2f>& outPositions);
	u32 _spawnTo_Positions(Vector<Vec3f>& outPositions);
};


#endif

}