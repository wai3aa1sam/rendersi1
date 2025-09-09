#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_caclExternalForce }


		// CsFunc		Cs_updateSpatialLut
		// CsFunc		Cs_caclDensityData
		// CsFunc		Cs_caclViscosity
		// CsFunc		Cs_caclPressureForce
		// CsFunc		Cs_updatePositon

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

#define RDS_NUM_THREADS 8

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

RDS_BUFFER(float2, u_positions);
RDS_BUFFER(float2, u_velocities);
RDS_BUFFER(float2, u_densityData);
RDS_BUFFER(float2, u_predictedPositions);

RDS_BUFFER(uint, u_spatialKeys);
RDS_BUFFER(uint, u_spatialOffsets);
RDS_BUFFER(uint, u_spatialSortedIdxs);

RDS_BUFFER(float2, u_sortedPositions);
RDS_BUFFER(float2, u_sortedVelocities);
RDS_BUFFER(float2, u_sortedPredictedPositions);

uint u_particleCount;

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_caclExternalForce(ComputeIn input)
{
	bool isInBoundary = input.dispatchThreadId.x < u_particleCount;
	if (!isInBoundary) return;


}