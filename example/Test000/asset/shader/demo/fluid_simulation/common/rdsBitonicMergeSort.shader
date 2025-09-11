#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_sort }

	Permutation
	{
	}
}
#endif

/*
references:
- https://github.com/SebLague/Fluid-Sim/blob/Episode-01/Assets/Scripts/Compute%20Helpers/GPU%20Sort/Resources/BitonicMergeSort.compute
*/


#include "built-in/shader/rds_shader.hlsl"

#define RDS_NUM_THREADS 128

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

#define T uint3
RDS_RW_BUFFER(T, u_list);

uint u_size;
uint u_groupWidth;
uint u_groupHeight;
uint u_stepIndex;

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_sort(ComputeIn input)
{
	uint i 				= input.dispatchThreadId.x;
	uint hIndex 		= i & (u_groupWidth - 1);
	uint indexLeft 		= hIndex + (u_groupHeight + 1) * (i / u_groupWidth);
	uint rightStepSize 	= u_stepIndex == 0 ? u_groupHeight - 2 * hIndex : (u_groupHeight + 1) / 2;
	uint indexRight 	= indexLeft + rightStepSize;

	// Exit if out of bounds (for non-power of 2 input sizes)
	if (indexRight >= u_size) return;

	T valueLeft 	= RDS_RW_BUFFER_LOAD_I(T, u_list, 	indexLeft);		
	T valueRight 	= RDS_RW_BUFFER_LOAD_I(T, u_list, 	indexRight);

	bool isDescending = valueLeft.z > valueRight.z;
	// Swap entries if value is descending
	if (isDescending)
	{
		T temp = valueLeft;
		RDS_RW_BUFFER_STORE_I(T, u_list, indexLeft, 	valueRight);
		RDS_RW_BUFFER_STORE_I(T, u_list, indexRight, 	temp);
	}
}
