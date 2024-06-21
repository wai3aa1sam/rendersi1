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

#include "built-in/shader/rds_shader.hlsl"

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
	RDS_IMAGE_3D_GET(float4, image)[imageCoords] = clear_value;
}
