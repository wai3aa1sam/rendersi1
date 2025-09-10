#ifndef __rdsFluidSim2D_SpatialLut_HLSL__
#define __rdsFluidSim2D_SpatialLut_HLSL__

#include "built-in/shader/rds_shader.hlsl"

// same as cpu impl

static const int2 SpatialLut_cellOffsets2D[9] =
{
	int2(-1, +1),
	int2(+0, +1),
	int2(+1, +1),
	int2(-1, +0),
	int2(+0, +0),
	int2(+1, +0),
	int2(-1, -1),
	int2(+0, -1),
	int2(+1, -1),
};

static const uint SpatialLut_hashK1 = 15823;
static const uint SpatialLut_hashK2 = 9737333;

int2 SpatialLut_toCell2D(float2 position, float radius)
{
	return (int2)floor(position / radius);
}

uint SpatialLut_hashCell2D(int2 cell)
{
	cell = (uint2)cell;
	uint a = cell.x * SpatialLut_hashK1;
	uint b = cell.y * SpatialLut_hashK2;
	return (a + b);
}

uint SpatialLut_toKeyFromHash(uint hash, uint tableSize)
{
	return hash % tableSize;
}


#endif
