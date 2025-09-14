#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim_ParticleSpawner.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim_ParticleSpawner-Impl ---
#endif // 0
#if 1

void FluidSim_ParticleSpawner::create2D(const Rect2f& spawnRegion_)
{
	spawnRegion = spawnRegion_;

	auto nParticlesPerAxis	= calcSpawnCountPerAxis2D();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y;
	particleCount			= nParticles;
}

void 
FluidSim_ParticleSpawner::create3D(const AABBox3f& spawnRegion_)
{
	spawnRegion3D = spawnRegion_;

	auto nParticlesPerAxis	= calcSpawnCountPerAxis3D();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y * nParticlesPerAxis.z;
	particleCount			= nParticles;
}

u32
FluidSim_ParticleSpawner::spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions
	, Vector<Vec2f>& outVelocities
	, Vector<float>& outDensities, Vector<Vec2f>& outDensityData)
{
	auto nParticles			= _spawnTo_Positions(outPositions);
	
	//auto& outPositions	= args.outPositions;
	//auto& outVelocities	= args.outVelocities;
	//auto& outDensities	= args.outDensities;

	outPredictedPositions.clear();
	outVelocities.clear();
	outDensities.clear();
	outDensityData.clear();

	outPredictedPositions.resize(nParticles);
	outVelocities.resize(nParticles);
	outDensities.resize(nParticles);
	outDensityData.resize(nParticles);

	for (size_t i = 0; i < outPositions.size(); i++)
	{
		outVelocities[i] = initVelocity;
	}

	return nParticles;
}

u32 
FluidSim_ParticleSpawner::spawnTo(RenderGpuBuffer* o_positions, RenderGpuBuffer* o_velocities, RenderGpuBuffer* o_predictedPositions)
{
	RDS_CORE_ASSERT(o_positions);
	Vector<Vec2f> v;
	_spawnTo_Positions(v);
	o_positions->uploadToGpu(makeByteSpan(v.span()));
	if (o_predictedPositions)
	{
		o_predictedPositions->uploadToGpu(makeByteSpan(v.span()));
	}
	if (o_velocities)
	{
		o_velocities->uploadToGpu(makeByteSpan(v.span()));
	}
	return sCast<u32>(v.size());
}

u32 
FluidSim_ParticleSpawner::spawnTo(RdgBufferHnd bufPos)
{
	return spawnTo(bufPos.renderResource(), nullptr, nullptr);
}

u32 
FluidSim_ParticleSpawner::spawnTo3D(RenderGpuBuffer* o_positions, RenderGpuBuffer* o_velocities, RenderGpuBuffer* o_predictedPositions)
{
	RDS_CORE_ASSERT(o_positions);
	Vector<Vec3f> v;
	_spawnTo_Positions(v);
	o_positions->uploadToGpu(makeByteSpan(v.span()));
	if (o_predictedPositions)
	{
		o_predictedPositions->uploadToGpu(makeByteSpan(v.span()));
	}
	if (o_velocities)
	{
		o_velocities->uploadToGpu(makeByteSpan(v.span()));
	}
	return sCast<u32>(v.size());
}

Vec2i 
FluidSim_ParticleSpawner::calcSpawnCountPerAxis2D() const
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

Vec3i 
FluidSim_ParticleSpawner::calcSpawnCountPerAxis3D() const
{
	auto size =  Vec3f{(spawnRegion3D.max - spawnRegion3D.min)};

	float area = size.x * size.y * size.z;
	int targetTotal = math::ceilToInt(area * spawnDensity);

	float lenSum = size.x + size.y + size.z;
	Vec3f t = size / lenSum;
	float m = math::sqrt(targetTotal / (t.x * t.y * t.z));
	int nx = math::ceilToInt(t.x * m);
	int ny = math::ceilToInt(t.y * m);
	int nz = math::ceilToInt(t.z * m);

	Vec3i o = { nx, ny, nz };
	return o;
}

u32 
FluidSim_ParticleSpawner::_spawnTo_Positions(Vector<Vec2f>& outPositions)
{
	auto nParticlesPerAxis	= calcSpawnCountPerAxis2D();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y;

	outPositions.clear();
	outPositions.reserve(nParticles);

	auto spawnRegionCenter	= Rect2T_center(spawnRegion);
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
	}

	particleCount = nParticles;
	return nParticles;
}

u32 
FluidSim_ParticleSpawner::_spawnTo_Positions(Vector<Vec3f>& outPositions)
{
	auto nParticlesPerAxis	= calcSpawnCountPerAxis3D();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y * nParticlesPerAxis.z;

	outPositions.clear();
	outPositions.reserve(nParticles);

	auto spawnRegionCenter	= AABBox3T_center(spawnRegion3D);
	auto spawnRegionSize	= AABBox3T_halfSize(spawnRegion3D);

	for (size_t x = 0; x < nParticlesPerAxis.x; x++)
	{
		for (size_t y = 0; y < nParticlesPerAxis.y; y++)
		{
			for (size_t z = 0; z < nParticlesPerAxis.z; z++)
			{
				float tx = x / (nParticlesPerAxis.x - 1.0f);
				float ty = y / (nParticlesPerAxis.y - 1.0f);
				float tz = z / (nParticlesPerAxis.x - 1.0f);

				float px = (tx - 0.5f) * spawnRegionSize.x + spawnRegionCenter.x;
				float py = (ty - 0.5f) * spawnRegionSize.y + spawnRegionCenter.y;
				float pz = (tz - 0.5f) * spawnRegionSize.z + spawnRegionCenter.z;

				outPositions.emplace_back(px, py, pz);
			}
		}
	}

	for (size_t i = 0; i < outPositions.size(); i++)
	{
		auto rng = Random::instance();
		//float angle		= (float)rng->range(0.0f, 1.0f) * 3.14f * 2;
		Vec3f dir		= rng->direction<Vec3f>();
		Vec3f jitter	= dir * jitterFct * (rng->range(0.0f, 1.0f) - 0.5f);

		outPositions[i]		= outPositions[i] + jitter;
	}

	particleCount = nParticles;
	return nParticles;
}

#endif

}