#if 0
Shader {
	Properties {
		
	}
	
	Pass {	
		CsFunc		Cs_main
	}

	Permutation
	{
		//RDS_VCT_DIFFUSE_CONE_COUNT = {6, 16, 32}
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
RDS_IMAGE_3D(float4, voxel_tex_radiance);

void setAlpha(RWTexture3D<float4> image, uint3 imageCoords, float value)
{
	if (image[imageCoords].a != 0.0)
	{
		image[imageCoords].a = value;
	}
}

[numThreads(CS_BLOCK_SIZE, CS_BLOCK_SIZE, 1)]
void Cs_main(ComputeIn input)
{
	bool isInBoundary = Image_isInBoundary(input.dispatchThreadId, image_extent, image_extent_offset);
	if (!isInBoundary)
		 return;

	float3 imageCoords = (input.dispatchThreadId + image_extent_offset);

	uint voxelResolution = Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	imageCoords = Vct_computeImageCoords(imageCoords, clipmap_level, voxelResolution);
	
	RWTexture3D<float4> voxelTexRadiance = RDS_IMAGE_3D_GET(float4,  voxel_tex_radiance);
	setAlpha(voxelTexRadiance, imageCoords, 1.0);

	#if VCT_USE_6_FACES_CLIPMAP
	setAlpha(voxelTexRadiance, imageCoords + float3(1.0 * voxelResolution, 0.0, 0.0), 1.0);
	setAlpha(voxelTexRadiance, imageCoords + float3(2.0 * voxelResolution, 0.0, 0.0), 1.0);
	setAlpha(voxelTexRadiance, imageCoords + float3(3.0 * voxelResolution, 0.0, 0.0), 1.0);
	setAlpha(voxelTexRadiance, imageCoords + float3(4.0 * voxelResolution, 0.0, 0.0), 1.0);
	setAlpha(voxelTexRadiance, imageCoords + float3(5.0 * voxelResolution, 0.0, 0.0), 1.0);
	#endif
}
