#ifndef __rdsVct_Common_HLSL__
#define __rdsVct_Common_HLSL__

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/

#include "built-in/shader/rds_shader.hlsl"

#if 0
#pragma mark --- vct_common_param ---
#endif // 0
#if 1

uint 	voxel_resolution;
float3 	voxel_region_minPosWs;
float3 	voxel_region_maxPosWs;
float 	voxel_size;
uint 	clipmap_level;
float3	clipmap_center;

#endif

static int s_kAxisX = 0;
static int s_kAxisY = 1;
static int s_kAxisZ = 2;

int getDominantAxisIdx(float3 v0, float3 v1, float3 v2)
{
    float3 aN = abs(cross(v1 - v0, v2 - v0));
    
    if (aN.x > aN.y && aN.x > aN.z)
        return s_kAxisX;
        
    if (aN.y > aN.z)
        return s_kAxisY;

    return s_kAxisZ;
}

bool Vct_isOutsideVoxelRegion(float3 pos, float3 minPos, float3 maxPos)
{
    return any(pos < minPos) || any(pos > maxPos);
}

float3 Vct_getVoxelRegionExtent(float3 minPos, float3 maxPos)
{
    return maxPos - minPos;
}

float3 Vct_worldToClipmapUvw(float3 posWs, float voxelResolutionInv, float voxelSize)
{
	float3 posVoxel = (posWs) * voxelResolutionInv / voxelSize;
	float3 uvw = remapNeg11To01_Inv_Y(posVoxel);
	return uvw;
}

float3 Vct_clipmapUvwToVoxel(float3 uvw, float voxelResolution)
{
	float3 posVoxel = remap01ToNeg11(uvw);
    posVoxel.y *= -1.0;
	posVoxel   *= voxelResolution;
	return posVoxel;
}

#if 0

float3 Vct_toImageUvw(float3 posWs, float extentWs)
{
    //return frac(posWs / extentWs);
    return remapNeg11To01((posWs / extentWs));
}

uint3 Vct_toImageCoords(float3 posWs, float extentWs, uint resolution, float level)
{
    float3  uvw         = Vct_toImageUvw(posWs, extentWs);
    uint3   imageCoords = (uvw * resolution);
    //imageCoords = (uvw * resolution) % resolution;

    return imageCoords;
}

#endif

#endif