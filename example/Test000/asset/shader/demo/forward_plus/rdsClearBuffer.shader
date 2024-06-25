#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		CsFunc		Cs_main
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

uint buffer_size;
uint clear_value;

RDS_RW_BUFFER(uint, buffer);

[numThreads(8, 1, 1)]
void Cs_main(ComputeIn input)
{
	bool isInBoundary = input.dispatchThreadId.x < buffer_size;
	if (!isInBoundary)
		 return;
	
	uint bufferIndex = input.dispatchThreadId.x;
	RDS_RW_BUFFER_STORE_I(uint, buffer, bufferIndex, clear_value);
}
