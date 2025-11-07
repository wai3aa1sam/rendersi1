#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_sort }
	Pass { CsFunc		Cs_bubbleSort }
	

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
RDS_RW_BUFFER(uint3, u_list);

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

	uint3 valueLeft 	= RDS_RW_BUFFER_LOAD_I(uint3, u_list, 	indexLeft);		
	uint3 valueRight 	= RDS_RW_BUFFER_LOAD_I(uint3, u_list, 	indexRight);

	bool isDescending = valueLeft.z > valueRight.z;
	// Swap entries if value is descending
	if (isDescending)
	{
		uint3 temp = valueLeft;
		RDS_RW_BUFFER_STORE_I(uint3, u_list, indexLeft, 	valueRight);
		RDS_RW_BUFFER_STORE_I(uint3, u_list, indexRight, 	temp);
	}
}

// workaround first, later debug the bitonic_merge_sort
[numThreads(1, 1, 1)]
void Cs_bubbleSort(ComputeIn input)
{
	uint n = u_size;
	for (int i = 0; i < n - 1; i++) 
	{
		bool flag = false;
        for (int j = 0; j < n - i - 1; j++) 
		{
			T valueLeft 	= RDS_RW_BUFFER_LOAD_I(T, u_list, 	j);		
			T valueRight 	= RDS_RW_BUFFER_LOAD_I(T, u_list, 	j + 1);
            if (valueLeft.z > valueRight.z)
			{
				T temp = valueLeft;
				RDS_RW_BUFFER_STORE_I(T, u_list, j, 		valueRight);
				RDS_RW_BUFFER_STORE_I(T, u_list, j + 1, 	temp);
				flag = true;
			}
        }
		if (!flag)
			break;
    }
}
