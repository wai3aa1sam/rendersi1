#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_quickSort }
	Pass { CsFunc		Cs_bubbleSort }
	

	Permutation
	{
	}
}
#endif

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

[numThreads(1, 1, 1)]
void Cs_quickSort(ComputeIn input)
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
