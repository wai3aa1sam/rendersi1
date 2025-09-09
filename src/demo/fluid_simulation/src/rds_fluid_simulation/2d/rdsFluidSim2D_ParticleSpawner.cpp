#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim2D_ParticleSpawner.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_ParticleSpawner-Impl ---
#endif // 0
#if 1

u32 
FluidSim2D_ParticleSpawner::spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions
	, Vector<Vec2f>& outVelocities
	, Vector<float>& outDensities, Vector<Vec2f>& outDensityData)
{
	auto nParticlesPerAxis	= calcSpawnCountPerAxis();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y;

	//auto& outPositions	= args.outPositions;
	//auto& outVelocities	= args.outVelocities;
	//auto& outDensities	= args.outDensities;

	outPositions.clear();
	outPredictedPositions.clear();
	outVelocities.clear();
	outDensities.clear();
	outDensityData.clear();

	outPositions.reserve(nParticles);

	outPredictedPositions.resize(nParticles);
	outVelocities.resize(nParticles);
	outDensities.resize(nParticles);
	outDensityData.resize(nParticles);

	auto spawnRegionCenter	= Vec2f{spawnRegion.pos} + Vec2f{spawnRegion.size} / 2.0f;
	auto spawnRegionSize	= Vec2f{spawnRegion.size};
	for (size_t y = 0; y < nParticlesPerAxis.y; y++)
	{
		for (size_t x = 0; x < nParticlesPerAxis.x; x++)
		{
			float tx = x / (nParticlesPerAxis.x - 1.0f);
			float ty = y / (nParticlesPerAxis.y - 1.0f);

			float px = (tx - 0.5f) * spawnRegionSize.x + spawnRegionCenter.x;
			float py = (ty - 0.5f) * spawnRegionSize.y + spawnRegionCenter.y;

			outPositions.emplace_back(px, py);
		}
	}

	for (size_t i = 0; i < outPositions.size(); i++)
	{
		auto rng = Random::instance();
		float angle		= (float)rng->range(0.0f, 1.0f) * 3.14f * 2;
		Vec2f dir		= Vec2f{math::cos(angle), math::sin(angle)};
		Vec2f jitter	= dir * jitterFct * (rng->range(0.0f, 1.0f) - 0.5f);

		outPositions[i]		= outPositions[i] + jitter;
		outVelocities[i]	= initVelocity;
	}

	return nParticles;
}

Vec2i 
FluidSim2D_ParticleSpawner::calcSpawnCountPerAxis() const
{
	auto size = Vec2f{spawnRegion.size};

	float area = size.x * size.y;
	int targetTotal = math::ceilToInt(area * spawnDensity);

	float lenSum = size.x + size.y;
	Vec2f t = size / lenSum;
	float m = math::sqrt(targetTotal / (t.x * t.y));
	int nx = math::ceilToInt(t.x * m);
	int ny = math::ceilToInt(t.y * m);

	return { nx, ny };
}

#endif

}