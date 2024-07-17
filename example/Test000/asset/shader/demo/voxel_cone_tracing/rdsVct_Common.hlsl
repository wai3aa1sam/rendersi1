#ifndef __rdsVct_Common_HLSL__
#define __rdsVct_Common_HLSL__

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/

#include "built-in/shader/rds_shader.hlsl"
#include "rdsVct_ShaderInterop.hlsl"

#if 0
#pragma mark --- vct_common_param ---
#endif // 0
#if 1

//float3 	voxel_region_minPosWs;
//float3 	voxel_region_maxPosWs;
//float 	voxel_size;
//float3	clipmap_center;

uint 	voxel_resolution;
float 	voxel_sizeL0;

uint 	clipmap_level;
uint    clipmap_maxLevel;

RDS_BUFFER(VoxelClipmap, clipmaps);

#endif

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/

static const int    Vct_kVoxelBorder       = 0;
static const float3 Vct_kVoxelBorders       = float3(Vct_kVoxelBorder, Vct_kVoxelBorder, Vct_kVoxelBorder);
static const int    Vct_kClipmapFaceCount  = 6;

float Vct_computeVoxelSize(float voxelSizeL0, float level)
{
    return voxelSizeL0 * exp2(level);
}

VoxelClipmap Vct_getVoxelClipmap(float level)
{
	VoxelClipmap o;
	o = RDS_BUFFER_LOAD_I(VoxelClipmap, clipmaps, level);
	return o;
}

float3 Vct_worldToClipmapUvw(float3 posWs, VoxelClipmap clipmap, float voxelResolutionInv)
{
	float3 posVoxel = (posWs - clipmap.center) * voxelResolutionInv / clipmap.voxelSize;
	float3 uvw = remapNeg11To01_Inv_Y(posVoxel);
	return uvw;
}

float3 Vct_clipmapUvwToVoxel(float3 uvw, float voxelResolution)
{
	float3 posVoxel = remap01ToNeg11_Inv_Y(uvw) * voxelResolution;
	return posVoxel;
}

float3 Vct_clipmapUvwToWorld(float3 uvw, VoxelClipmap clipmap, float voxelResolution)
{
	float3 posWs = remap01ToNeg11_Inv_Y(uvw);
	posWs *= clipmap.voxelSize * voxelResolution;
	posWs += clipmap.center;
	return posWs;
}

uint Vct_computeVoxelResolutionWithBorder(uint voxelResolution)
{
	return voxelResolution + Vct_kVoxelBorder * 2;
}

float3 Vct_computeImageCoords(float3 imageCoords, float level, uint voxelResolution)
{
	float3 o = imageCoords;
	o += Vct_kVoxelBorders;
	o.y += level * voxelResolution;
	return o;
}

bool Vct_isOutsideVoxelRegion(float3 pos, float3 minPos, float3 maxPos)
{
    return any(pos < minPos) || any(pos > maxPos);
}

float3 Vct_getVoxelRegionExtent(float3 minPos, float3 maxPos)
{
    return maxPos - minPos;
}

float3 Vct_computeVoxelFaceIndices(float3 dir)
{
	float3 o = float3(dir.x > 0.0 ? 0.0 : 1.0
					, dir.y > 0.0 ? 2.0 : 3.0
					, dir.z > 0.0 ? 4.0 : 5.0);
	return o;
}

float3 Vct_computeVoxelFaceOffsets(float3 dir)
{
	float3 o = Vct_computeVoxelFaceIndices(dir);
	o = o / (Vct_kClipmapFaceCount);
	return o;
}

void Vct_VoxelClipmapAtomicStoreAvgRGBA8(RWTexture3D<uint> image, uint3 imageCoords, float3 value
										, float3 faceIndices, float3 weights, uint voxelResolution)
{
	AtomicOp_avgRGBA8(image, imageCoords + float3(faceIndices.x * voxelResolution, 0.0, 0.0), value.rgb * weights.x);
	AtomicOp_avgRGBA8(image, imageCoords + float3(faceIndices.y * voxelResolution, 0.0, 0.0), value.rgb * weights.y);
	AtomicOp_avgRGBA8(image, imageCoords + float3(faceIndices.z * voxelResolution, 0.0, 0.0), value.rgb * weights.z);
}

static const int s_kAxisX = 0;
static const int s_kAxisY = 1;
static const int s_kAxisZ = 2;

int getDominantAxisIdx(float3 v0, float3 v1, float3 v2)
{
    float3 aN = abs(cross(v1 - v0, v2 - v0));
    
    if (aN.x > aN.y && aN.x > aN.z)
        return s_kAxisX;
        
    if (aN.y > aN.z)
        return s_kAxisY;

    return s_kAxisZ;
}

#endif