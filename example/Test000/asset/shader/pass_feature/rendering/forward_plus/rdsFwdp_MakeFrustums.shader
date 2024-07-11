#if 0
Shader {
	Properties {
		Float debugFrustumZ = -0.2
	}
	
	Pass {
		
		DepthTest	Always

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha

		CsFunc		makeFrustums_csMain
		VsFunc		makeFrustums_vsMain
		PsFunc		makeFrustums_psMain
	}

	Permutation
	{
		//BLOCK_SIZE 	= { 16, 32, 64, 128, 256, }
	}
}
#endif

/*
	references:
	~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "rdsFwdp_Common.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};


#if 0
#pragma mark --- fwpd_make_frustums_param ---
#endif // 0
#if 1

uint3 nThreads;
uint3 nThreadGroups;

RDS_RW_BUFFER(Frustum, oFrustums);
RDS_RW_BUFFER(DebugFrustrumPts, oDebugFrustumsPts);

float debugFrustumZ;

#endif

/*
	eg. resolution = {1280, 720}
	1 thread == 1 frustum
	, 1 block has 16 tiles, then 1280 / 16 = 80 frustum / block
	, 1 thread group = 16 block, 80 / 16 = 5 thread group 
*/

[numThreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void makeFrustums_csMain(ComputeIn input)
{
	bool isInBoundary = input.dispatchThreadId.x < nThreads.x && input.dispatchThreadId.y < nThreads.y;
	if (!isInBoundary)
	{
		return;
	}

	// view space
	const float3 posCamera = float3(0, 0, 0);

	float4 frustumFarVtxSs_topLeft		= float4(float2(input.dispatchThreadId.x, 		input.dispatchThreadId.y) 		* BLOCK_SIZE, -1.0, 1.0);
	float4 frustumFarVtxSs_topRight		= float4(float2(input.dispatchThreadId.x + 1, 	input.dispatchThreadId.y) 		* BLOCK_SIZE, -1.0, 1.0);
	float4 frustumFarVtxSs_bottomLeft	= float4(float2(input.dispatchThreadId.x, 		input.dispatchThreadId.y + 1) 	* BLOCK_SIZE, -1.0, 1.0);
	float4 frustumFarVtxSs_bottomRight	= float4(float2(input.dispatchThreadId.x + 1, 	input.dispatchThreadId.y + 1) 	* BLOCK_SIZE, -1.0, 1.0);
    
    DrawParam drawParam = rds_DrawParam_get();
	float3 frustumFarVtxVs_topLeft		= SpaceTransform_screenToView(frustumFarVtxSs_topLeft, 		drawParam).xyz;
	float3 frustumFarVtxVs_topRight		= SpaceTransform_screenToView(frustumFarVtxSs_topRight, 	drawParam).xyz;
	float3 frustumFarVtxVs_bottomLeft	= SpaceTransform_screenToView(frustumFarVtxSs_bottomLeft, 	drawParam).xyz;
	float3 frustumFarVtxVs_bottomRight	= SpaceTransform_screenToView(frustumFarVtxSs_bottomRight, 	drawParam).xyz;

	Frustum frustum;
	frustum.planes[0] = Geometry_makePlane(posCamera, frustumFarVtxVs_topLeft, 		frustumFarVtxVs_topRight);	 	/*topPlane*/
	frustum.planes[1] = Geometry_makePlane(posCamera, frustumFarVtxVs_bottomRight, 	frustumFarVtxVs_bottomLeft);	/*bottomPlane*/
	frustum.planes[2] = Geometry_makePlane(posCamera, frustumFarVtxVs_bottomLeft, 	frustumFarVtxVs_topLeft);	 	/*leftPlane*/
	frustum.planes[3] = Geometry_makePlane(posCamera, frustumFarVtxVs_topRight, 	frustumFarVtxVs_bottomRight); 	/*rightPlane*/

	// frustum.planes[0] = Geometry_makePlane(posCamera, frustumFarVtxVs_bottomLeft, 	frustumFarVtxVs_topLeft);	 	/*leftPlane*/
	// frustum.planes[1] = Geometry_makePlane(posCamera, frustumFarVtxVs_topRight, 	frustumFarVtxVs_bottomRight); 	/*rightPlane*/	
	// frustum.planes[2] = Geometry_makePlane(posCamera, frustumFarVtxVs_topLeft, 		frustumFarVtxVs_topRight);	 	/*topPlane*/
	// frustum.planes[3] = Geometry_makePlane(posCamera, frustumFarVtxVs_bottomRight, 	frustumFarVtxVs_bottomLeft);	/*bottomPlane*/
	
	uint idx = input.dispatchThreadId.x + (input.dispatchThreadId.y * nThreads.x);
	RDS_RW_BUFFER_STORE_I(Frustum, oFrustums, idx, frustum);

	// for debug purpose
	#if 1
	float  factor 	= 1.0;
	float2 step 	= float2(factor * input.dispatchThreadId.x, 		factor * input.dispatchThreadId.y);
	float2 stepNext = float2(factor * (input.dispatchThreadId.x + 1), 	factor * (input.dispatchThreadId.y + 1));

	DebugFrustrumPts debugFrustumPts;
	#if 0
	debugFrustumPts.pts[0].xyz = frustumFarVtxVs_topLeft		+ float3(step.x, 		step.y, 	0.0);
	debugFrustumPts.pts[1].xyz = frustumFarVtxVs_topRight		+ float3(stepNext.x, 	step.y,  	0.0);
	debugFrustumPts.pts[2].xyz = frustumFarVtxVs_bottomLeft		+ float3(step.x,  		stepNext.y, 0.0);
	debugFrustumPts.pts[3].xyz = frustumFarVtxVs_bottomRight	+ float3(stepNext.x, 	stepNext.y, 0.0);
	#else
	frustumFarVtxSs_topLeft		= float4(float2(input.dispatchThreadId.x, 		input.dispatchThreadId.y) 		* BLOCK_SIZE, 1.0, 1.0);
	frustumFarVtxSs_topRight	= float4(float2(input.dispatchThreadId.x + 1, 	input.dispatchThreadId.y) 		* BLOCK_SIZE, 1.0, 1.0);
	frustumFarVtxSs_bottomLeft	= float4(float2(input.dispatchThreadId.x, 		input.dispatchThreadId.y + 1) 	* BLOCK_SIZE, 1.0, 1.0);
	frustumFarVtxSs_bottomRight	= float4(float2(input.dispatchThreadId.x + 1, 	input.dispatchThreadId.y + 1) 	* BLOCK_SIZE, 1.0, 1.0);

	debugFrustumPts.pts[0].xyz = frustumFarVtxVs_topLeft.xyz;	
	debugFrustumPts.pts[1].xyz = frustumFarVtxVs_topRight.xyz;
	debugFrustumPts.pts[2].xyz = frustumFarVtxVs_bottomLeft.xyz;
	debugFrustumPts.pts[3].xyz = frustumFarVtxVs_bottomRight.xyz;
	#endif
	//debugFrustumPts.pts[4].xyz = posCamera;
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

PixelIn makeFrustums_vsMain(VertexIn input)
{
    PixelIn o;
	input.positionOs.z = debugFrustumZ;
    o.positionHcs = mul(RDS_MATRIX_PROJ, input.positionOs);
    //o.positionHcs = input.positionOs;

    return o;
}

float4 makeFrustums_psMain(PixelIn input) : SV_TARGET
{
	float4 o = float4(1, 0, 0, 1);
    return o;
}