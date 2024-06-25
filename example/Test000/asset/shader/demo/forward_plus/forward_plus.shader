#if 0
Shader {
	Properties {
		Float minDepthBias = 0.0
		Float maxDepthBias = 0.0
	}
	
	Pass {
		
		CsFunc		cullLights
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

#include "forward_plus_common.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};


#if 0
#pragma mark --- fwpd_cullLights_param ---
#endif // 0
#if 1

uint3 nThreads;
uint3 nThreadGroups;

RDS_TEXTURE_2D(texDepth);
RDS_BUFFER(Frustum, frustums);

/*
	lightGrid : uint2{lightIdxOffset, lightCount}
*/

RDS_RW_BUFFER(uint,  opaque_lightIndexCounter);		// size: 1
RDS_RW_BUFFER(uint,  opaque_lightIndexList);
RDS_IMAGE_2D( uint2, opaque_lightGrid);

RDS_RW_BUFFER(uint,  transparent_lightIndexCounter);	// size: 1
RDS_RW_BUFFER(uint,  transparent_lightIndexList);
RDS_IMAGE_2D( uint2, transparent_lightGrid);

//StructuredBuffer<uint> 	g_buffer[] 		: register(t0, space7);
//RWStructuredBuffer<uint> g_rwbuffer[] 	: register(u0, space8);

/* 
	1 block for 1 thread group, 1 tile for 1 thread, 1 block has 1 frustum
*/
// no +-*/ operation is ok
groupshared uint uMinDepth;
groupshared uint uMaxDepth;

groupshared Frustum groupFrustum;

groupshared uint opaque_lightCount;
groupshared uint opaque_lightIndexStartOffset;
groupshared uint opaque_lightList[LIGHT_LIST_LOCAL_SIZE];

groupshared uint transparent_lightCount;
groupshared uint transparent_lightIndexStartOffset;
groupshared uint transparent_lightList[LIGHT_LIST_LOCAL_SIZE];

#endif

float minDepthBias;
float maxDepthBias;

void 
opaque_appendLight(uint i)
{
	uint idx;
	InterlockedAdd(opaque_lightCount, 1, idx);
	if (idx < LIGHT_LIST_LOCAL_SIZE)
	{
		opaque_lightList[idx] = i;
	}
}

void 
transparent_appendLight(uint i)
{
	uint idx;
	InterlockedAdd(transparent_lightCount, 1, idx);
	if (idx < LIGHT_LIST_LOCAL_SIZE)
	{
		transparent_lightList[idx] = i;
	}
}

[numThreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void cullLights(ComputeIn input)
{
	int2  uv 	 = input.dispatchThreadId.xy;
    float depth  = RDS_TEXTURE_2D_GET(texDepth).Load(int3(uv, 0)).r;
	uint  uDepth = asuint(depth);
	
	bool isFirstThread = input.groupIndex == 0;
	if (isFirstThread)
	{
		uMinDepth = UINT_MAX;
		uMaxDepth = 0;
		opaque_lightCount 		= 0;
		transparent_lightCount 	= 0;
		groupFrustum = RDS_BUFFER_LOAD_I(Frustum, frustums, input.groupId.x + (input.groupId.y * nThreadGroups.x));
	}
	// ensure all threads in group reach this point, also flush the groupshared to all groupThreads
	GroupMemoryBarrierWithGroupSync();

	// compare and store to variable 
	InterlockedMin(uMinDepth, uDepth);
    InterlockedMax(uMaxDepth, uDepth);
    GroupMemoryBarrierWithGroupSync();

	float minDepth = asfloat(uMinDepth) - minDepthBias;
    float maxDepth = asfloat(uMaxDepth) - maxDepthBias;

	// view space
	float minDepthVs = SpaceTransform_clipToView(float4(0.0, 0.0, minDepth, 1.0)).z;	// for opaque
	float maxDepthVs = SpaceTransform_clipToView(float4(0.0, 0.0, maxDepth, 1.0)).z;
	float nearClipVs = SpaceTransform_clipToView(float4(0.0, 0.0, 0.0, 	    1.0)).z;	// for transparent

	Plane minPlane;
	minPlane.normal 	= float3(0.0, 0.0, -1.0);
	minPlane.distance 	= -minDepthVs;

	uint stepLight = BLOCK_SIZE * BLOCK_SIZE;
	// ensure the cull job is spread to all threads
	for (uint i = input.groupIndex; i < rds_nLights; i += stepLight)
	{
		Light light 	= rds_Lights_get(i);
		
		bool isEnabled 	= Light_isEnabled(light);
		if (!isEnabled)
			continue;


		switch (light.type)
		{
			case rds_LightType_Point:
			{
				Sphere sphere = Light_makeSphere(light);
				if (Geometry_isInside_sphereFrustum(sphere, groupFrustum, nearClipVs, maxDepthVs))
				{
					transparent_appendLight(i);
					if (!Geometry_isInside_spherePlane(sphere, minPlane))
					{
						opaque_appendLight(i);
					}
				}
			} break;
			
			case rds_LightType_Spot:
			{
				Cone cone = Light_makeCone(light);
				if (Geometry_isInside_coneFrustum(cone, groupFrustum, nearClipVs, maxDepthVs))
				{
					transparent_appendLight(i);
					if (!Geometry_isInside_conePlane(cone, minPlane))
					{
						opaque_appendLight(i);
					}
				}
			} break;

			case rds_LightType_Directional:
			{
				transparent_appendLight(i);
				opaque_appendLight(i);
			} break;
		}
	}
    GroupMemoryBarrierWithGroupSync();

	// update the global light grid
	if (isFirstThread)
	{
		RDS_RW_BUFFER_ATM_ADD_I(uint, opaque_lightIndexCounter, 	 0, opaque_lightCount, opaque_lightIndexStartOffset);
		RDS_IMAGE_2D_GET(uint2, opaque_lightGrid)[input.groupId.xy] 		= uint2(opaque_lightIndexStartOffset, opaque_lightCount);

		RDS_RW_BUFFER_ATM_ADD_I(uint, transparent_lightIndexCounter, 0, transparent_lightCount, transparent_lightIndexStartOffset);
		RDS_IMAGE_2D_GET(uint2, transparent_lightGrid)[input.groupId.xy] 	= uint2(transparent_lightIndexStartOffset, transparent_lightCount);
	}
	GroupMemoryBarrierWithGroupSync();

	// update the global light index list
	for (uint i = input.groupIndex; i < opaque_lightCount; i += stepLight)
	{
		RDS_RW_BUFFER_STORE_I(uint, opaque_lightIndexList, 		opaque_lightIndexStartOffset + i, 		opaque_lightList[i]);
	}
	for (uint i = input.groupIndex; i < transparent_lightCount; i += stepLight)
	{
		RDS_RW_BUFFER_STORE_I(uint, transparent_lightIndexList, transparent_lightIndexStartOffset + i, 	transparent_lightList[i]);
	}
}
