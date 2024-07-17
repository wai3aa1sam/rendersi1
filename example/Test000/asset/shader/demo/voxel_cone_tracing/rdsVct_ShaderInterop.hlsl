#ifndef __rdsVct_ShaderInterop_HLSL__
#define __rdsVct_ShaderInterop_HLSL__

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/


#define VCT_USE_CONSERVATIVE_RASTERIZATION 0
#define VCT_USE_6_FACES_CLIPMAP 1

#define VCT_MAX_CLIPMAP_LEVEL 6


struct VoxelClipmap
{
    float3  center;         // center of clipmap volume in world space
    float   voxelSize;      // half-extent of one voxel
};

#endif