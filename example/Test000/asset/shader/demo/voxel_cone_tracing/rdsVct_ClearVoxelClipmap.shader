#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		CsFunc		cs_main
	}

	Permutation
	{
		
	}
}
#endif

#include "rdsVct_Common.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

uint3 image_extent;
uint3 image_extent_offset;
float4 clear_value;

RDS_IMAGE_3D(float4, image);

[numThreads(8, 8, 8)]
void cs_main(ComputeIn input)
{
	bool isInBoundary = Image_isInBoundary(input.dispatchThreadId, image_extent, image_extent_offset);
	if (!isInBoundary)
		 return;

	float3 imageCoords = (input.dispatchThreadId + image_extent_offset);

	#if 1
	// first face will not clear, although it is useless
	uint voxelResolution = Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	
	//for (int i = 0; i < clipmap_maxLevel; ++i)
	{
		imageCoords = Vct_computeImageCoords(imageCoords, clipmap_level, voxelResolution);
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(0.0 * voxelResolution, 0.0, 0.0)] = clear_value;
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(1.0 * voxelResolution, 0.0, 0.0)] = clear_value;
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(2.0 * voxelResolution, 0.0, 0.0)] = clear_value;
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(3.0 * voxelResolution, 0.0, 0.0)] = clear_value;
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(4.0 * voxelResolution, 0.0, 0.0)] = clear_value;
		RDS_IMAGE_3D_GET(float4, image)[imageCoords + float3(5.0 * voxelResolution, 0.0, 0.0)] = clear_value;
	}
	#else
	RDS_IMAGE_3D_GET(float4, image)[imageCoords] = clear_value;
	#endif
}
