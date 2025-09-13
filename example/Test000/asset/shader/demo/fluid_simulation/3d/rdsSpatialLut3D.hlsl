#ifndef __rdsFluidSim3D_SpatialLut_HLSL__
#define __rdsFluidSim3D_SpatialLut_HLSL__

#include "built-in/shader/rds_shader.hlsl"

// same as cpu impl

static const int  SpatialLut_cellOffsetCount = 27;
static const int3 SpatialLut_cellOffsets3D[SpatialLut_cellOffsetCount] =
{
	int3(-1, -1, -1),
	int3(+0, -1, -1),
	int3(+1, -1, -1),

	int3(-1, +0, -1),
	int3(+0, +0, -1),
	int3(+1, +0, -1),

	int3(-1, +1, -1),
	int3(+0, +1, -1),
	int3(+1, +1, -1),

	int3(-1, -1, +0),
	int3(+0, -1, +0),
	int3(+1, -1, +0),

	int3(-1, +0, +0),
	int3(+0, +0, +0),
	int3(+1, +0, +0),

	int3(-1, +1, +0),
	int3(+0, +1, +0),
	int3(+1, +1, +0),

	int3(-1, -1, +1),
	int3(+0, -1, +1),
	int3(+1, -1, +1),

	int3(-1, +0, +1),
	int3(+0, +0, +1),
	int3(+1, +0, +1),

	int3(-1, +1, +1),
	int3(+0, +1, +1),
	int3(+1, +1, +1)
};

static const uint SpatialLut_hashK1 = 15823;
static const uint SpatialLut_hashK2 = 9737333;
static const uint SpatialLut_hashK3 = 440817757;

int3 SpatialLut_toCell3D(float3 position, float radius)
{
	return (int3)floor(position / radius);
}

uint SpatialLut_hashCell3D(int3 cell)
{
	cell = (uint3)cell;
	uint a = cell.x * SpatialLut_hashK1;
	uint b = cell.y * SpatialLut_hashK2;
	uint c = cell.z * SpatialLut_hashK3;
	return (a + b + c);
}

uint SpatialLut_toKeyFromHash(uint hash, uint tableSize)
{
	return hash % tableSize;
}

#endif
