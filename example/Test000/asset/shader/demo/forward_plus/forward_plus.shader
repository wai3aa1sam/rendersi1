#if 0
Shader {
	Properties {
		Float debugFrustumZ = -0.2
	}
	
	Pass {
		
		CsFunc		csMain
		//VsFunc		makeGridFrustums_vsMain
		//PsFunc		makeGridFrustums_psMain
	}

	Permutation
	{
		//TILE_COUNT 	= { 16, 32, 64, 128, 256, }
	}
}
#endif

/*
	references:
	~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "built-in/shader/common/rdsCommon.hlsl"
#include "built-in/shader/common/geometry/rdsGeometry.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

uint3 nThreads;
uint3 nThreadGroups;

RDS_IMAGE_2D(float4, oImage);

#define TILE_COUNT 16
#define GRID_SIZE TILE_COUNT

/*
	eg. resolution = {1280, 720}
	1 thread == 1 frustum
	, 1 grid has 16 tiles, then 1280 / 16 = 80 frustum / grid
	, 1 thread group = 16 grid, 80 / 16 = 5 thread group 
*/

[numThreads(8, 8, 1)]
void csMain(ComputeIn i)
{
	uint2 id = i.dispatchThreadId.xy;
	uint factor = 15;
	RDS_IMAGE_2D_GET(float4, oImage)[id.xy] = float4(id.x & id.y, (id.x & factor) / float(factor), (id.y & factor) / float(factor), 1.0);
}

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
};

PixelIn makeGridFrustums_vsMain(VertexIn i)
{
    PixelIn o;
	//i.positionOs.z = debugFrustumZ;
    //o.positionHcs = mul(RDS_MATRIX_PROJ, i.positionOs);
    o.positionHcs = i.positionOs;

    return o;
}

float4 makeGridFrustums_psMain(PixelIn i) : SV_TARGET
{
	float4 o = float4(1, 0, 0, 1);
    return o;
}