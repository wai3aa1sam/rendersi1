#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_debugSpatialLut }

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "rdsFluidSim3D_Common.hlsl"
#include "rdsSpatialLut3D.hlsl"

#define RDS_NUM_THREADS 32

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

RDS_RW_BUFFER(uint3, 	u_spatialLut);				// x: particle_index, y: hash, z: key
RDS_RW_BUFFER(uint,  	u_spatialLutKeyToStartIndex);
RDS_RW_BUFFER(float3,  	u_positions);

RDS_RW_BUFFER(float3,  	u_spatialLutDebugResultPositions);

uint 	u_particleCount;
float  	u_smoothingRadius;
float3  u_samplingPt;

[numThreads(1, 1, 1)]
void Cs_debugSpatialLut(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	uint neighbourCount = 0;

	// ---
	float3 	tarPos 		= u_samplingPt; // RDS_RW_BUFFER_LOAD_I(float2, u_predictedPositions, tarPtcIdx);
	int3 	originCell 	= SpatialLut_toCell3D(tarPos, u_smoothingRadius);
	float 	sqrRadius 	= u_smoothingRadius * u_smoothingRadius;

	// Neighbour search
	for (int i = 0; i < SpatialLut_cellOffsetCount; i ++)
	{
		uint hash 		= SpatialLut_hashCell3D(originCell + SpatialLut_cellOffsets3D[i]);
		uint key 		= SpatialLut_toKeyFromHash(hash, u_particleCount);
		uint curIndex 	= RDS_RW_BUFFER_LOAD_I(uint, u_spatialLutKeyToStartIndex, key);

		while (curIndex < u_particleCount)
		{
			uint3 spatialLut = RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, curIndex);
			curIndex++;
			
			uint neighbourIdx = spatialLut[0];
			if (spatialLut[2] != key) 		break;	  // not same key chunk
			if (spatialLut[1] != hash) 		continue; // not same hash
			//if (neighbourIdx == tarPtcIdx) 	continue; // skip self, except calcDesnsity

			float3 neighbourPos 	  = RDS_RW_BUFFER_LOAD_I(float3, u_positions, neighbourIdx);
			float3 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float3 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float3(0, 1, 0);
			
			// calc sth
			RDS_RW_BUFFER_STORE_I(float3, u_spatialLutDebugResultPositions,  neighbourCount, neighbourPos);
			neighbourCount++;
		}
	}

	while(neighbourCount < u_particleCount)
	{
		RDS_RW_BUFFER_STORE_I(float3, u_spatialLutDebugResultPositions,  neighbourCount, s_kInvalid_position);
		neighbourCount++;
	}
}

