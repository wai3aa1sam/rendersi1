#if 0
Shader {
	Properties {
		Float debugFrustumZ = -0.2
	}
	
	Pass {
		
		DepthTest	Always

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha

		RenderPrimitiveType Line	// TODO: remove

		CsFunc		makeGridFrustums_csMain
		VsFunc		makeGridFrustums_vsMain
		PsFunc		makeGridFrustums_psMain
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

RDS_RW_BUFFER(Frustum, oFrustum);
RDS_RW_BUFFER(DebugFrustrumPts, oDebugFrustumsPts);

float debugFrustumZ;

#define TILE_COUNT 16
#define GRID_SIZE TILE_COUNT

/*
	eg. resolution = {1280, 720}
	1 thread == 1 frustum
	, 1 grid has 16 tiles, then 1280 / 16 = 80 frustum / grid
	, 1 thread group = 16 grid, 80 / 16 = 5 thread group 
*/

[numThreads(GRID_SIZE, GRID_SIZE, 1)]
void makeGridFrustums_csMain(ComputeIn i)
{
	bool isInBoundary = i.dispatchThreadId.x < nThreads.x && i.dispatchThreadId.y < nThreads.y;
	if (!isInBoundary)
	{
		return;
	}

	// view space
	const float3 cameraPos = float3(0, 0, 0);

	float4 screenSpaceFrustumFarVtx_topLeft		= float4(float2(i.dispatchThreadId.x, 		i.dispatchThreadId.y) 		* GRID_SIZE, -1.0, 1.0);
	float4 screenSpaceFrustumFarVtx_topRight	= float4(float2(i.dispatchThreadId.x + 1, 	i.dispatchThreadId.y) 		* GRID_SIZE, -1.0, 1.0);
	float4 screenSpaceFrustumFarVtx_bottomLeft	= float4(float2(i.dispatchThreadId.x, 		i.dispatchThreadId.y + 1) 	* GRID_SIZE, -1.0, 1.0);
	float4 screenSpaceFrustumFarVtx_bottomRight	= float4(float2(i.dispatchThreadId.x + 1, 	i.dispatchThreadId.y + 1) 	* GRID_SIZE, -1.0, 1.0);
    
    DrawParam drawParam = rds_DrawParam_get();
	float3 viewSpaceFrustumFarVtx_topLeft		= SpaceTransformUtil_screenToView(screenSpaceFrustumFarVtx_topLeft, 		drawParam).xyz;
	float3 viewSpaceFrustumFarVtx_topRight		= SpaceTransformUtil_screenToView(screenSpaceFrustumFarVtx_topRight, 		drawParam).xyz;
	float3 viewSpaceFrustumFarVtx_bottomLeft	= SpaceTransformUtil_screenToView(screenSpaceFrustumFarVtx_bottomLeft, 		drawParam).xyz;
	float3 viewSpaceFrustumFarVtx_bottomRight	= SpaceTransformUtil_screenToView(screenSpaceFrustumFarVtx_bottomRight, 	drawParam).xyz;

	Frustum frustum;
	frustum.planes[0] = GeometryUtil_makePlane(cameraPos, viewSpaceFrustumFarVtx_topLeft, 		viewSpaceFrustumFarVtx_topRight);	 /*topPlane*/
	frustum.planes[1] = GeometryUtil_makePlane(cameraPos, viewSpaceFrustumFarVtx_bottomRight, 	viewSpaceFrustumFarVtx_bottomLeft);	 /*bottomPlane*/
	frustum.planes[2] = GeometryUtil_makePlane(cameraPos, viewSpaceFrustumFarVtx_bottomLeft, 	viewSpaceFrustumFarVtx_topLeft);	 /*leftPlane*/
	frustum.planes[3] = GeometryUtil_makePlane(cameraPos, viewSpaceFrustumFarVtx_topRight, 		viewSpaceFrustumFarVtx_bottomRight); /*rightPlane*/	

	uint idx = i.dispatchThreadId.x + (i.dispatchThreadId.y * nThreads.x);
	RDS_RW_BUFFER_STORE_I(Frustum, oFrustum, idx, frustum);

	// for debug purpose
	#if 1
	float  factor 	= 1.0;
	float2 step 	= float2(factor * i.dispatchThreadId.x, 		factor * i.dispatchThreadId.y);
	float2 stepNext = float2(factor * (i.dispatchThreadId.x + 1), 	factor * (i.dispatchThreadId.y + 1));

	DebugFrustrumPts debugFrustumPts;
	#if 0
	debugFrustumPts.pts[0].xyz = viewSpaceFrustumFarVtx_topLeft		+ float3(step.x, 		step.y, 	0.0);
	debugFrustumPts.pts[1].xyz = viewSpaceFrustumFarVtx_topRight	+ float3(stepNext.x, 	step.y,  	0.0);
	debugFrustumPts.pts[2].xyz = viewSpaceFrustumFarVtx_bottomLeft	+ float3(step.x,  		stepNext.y, 0.0);
	debugFrustumPts.pts[3].xyz = viewSpaceFrustumFarVtx_bottomRight	+ float3(stepNext.x, 	stepNext.y, 0.0);
	#else
	screenSpaceFrustumFarVtx_topLeft		= float4(float2(i.dispatchThreadId.x, 		i.dispatchThreadId.y) 		* GRID_SIZE, 1.0, 1.0);
	screenSpaceFrustumFarVtx_topRight		= float4(float2(i.dispatchThreadId.x + 1, 	i.dispatchThreadId.y) 		* GRID_SIZE, 1.0, 1.0);
	screenSpaceFrustumFarVtx_bottomLeft		= float4(float2(i.dispatchThreadId.x, 		i.dispatchThreadId.y + 1) 	* GRID_SIZE, 1.0, 1.0);
	screenSpaceFrustumFarVtx_bottomRight	= float4(float2(i.dispatchThreadId.x + 1, 	i.dispatchThreadId.y + 1) 	* GRID_SIZE, 1.0, 1.0);

	debugFrustumPts.pts[0].xyz = viewSpaceFrustumFarVtx_topLeft.xyz;	
	debugFrustumPts.pts[1].xyz = viewSpaceFrustumFarVtx_topRight.xyz;
	debugFrustumPts.pts[2].xyz = viewSpaceFrustumFarVtx_bottomLeft.xyz;
	debugFrustumPts.pts[3].xyz = viewSpaceFrustumFarVtx_bottomRight.xyz;
	#endif
	//debugFrustumPts.pts[4].xyz = cameraPos;
	for (uint i = 0; i < 4; ++i)
	{
		debugFrustumPts.pts[i].w = 1.0;
	}
	RDS_RW_BUFFER_STORE_I(DebugFrustrumPts, oDebugFrustumsPts, idx, debugFrustumPts);
	#endif
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
	i.positionOs.z = debugFrustumZ;
    o.positionHcs = mul(RDS_MATRIX_PROJ, i.positionOs);
    //o.positionHcs = i.positionOs;

    return o;
}

float4 makeGridFrustums_psMain(PixelIn i) : SV_TARGET
{
	float4 o = float4(1, 0, 0, 1);
    return o;
}