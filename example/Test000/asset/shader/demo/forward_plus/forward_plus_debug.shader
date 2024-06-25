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

/*
	lightGrid : uint2{lightIdxOffset, lightCount}
*/

RDS_RW_BUFFER(uint,  counter);		// size: 1
RDS_RW_BUFFER(uint,  offsets);

//StructuredBuffer<uint> 	g_buffer[] 		: register(t0, space7);
//RWStructuredBuffer<uint> g_rwbuffer[] 	: register(u0, space8);

groupshared uint opaque_lightCount;
groupshared uint opaque_lightIndexStartOffset;

#endif


[numThreads(1024, 1, 1)]
void cullLights(ComputeIn input)
{
	uint count 	= 1;
	uint offset = 0;
	RDS_RW_BUFFER_ATM_ADD_I(uint, counter, 	 0, count, offset);
	RDS_RW_BUFFER_STORE_I(	uint, offsets, 	 input.dispatchThreadId.x, offset);
}
