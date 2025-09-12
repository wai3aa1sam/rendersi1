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
#include "rdsSpatialLut2D.hlsl"

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
RDS_RW_BUFFER(float2,  	u_positions);

RDS_RW_BUFFER(float2,  	u_spatialLutDebugResultPositions);
//RDS_RW_BUFFER(float2,  	u_spatialLutDebugVelocities);

uint 	u_particleCount;
float  	u_smoothingRadius;
float2  u_samplingPt;

[numThreads(1, 1, 1)]
void Cs_debugSpatialLut(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	uint neighbourCount = 0;

	// ---
	float2 	tarPos 		= u_samplingPt; // RDS_RW_BUFFER_LOAD_I(float2, u_predictedPositions, tarPtcIdx);
	int2 	originCell 	= SpatialLut_toCell2D(tarPos, u_smoothingRadius);
	float 	sqrRadius 	= u_smoothingRadius * u_smoothingRadius;

	// Neighbour search
	for (int i = 0; i < SpatialLut_cellOffsetCount; i ++)
	{
		uint hash 		= SpatialLut_hashCell2D(originCell + SpatialLut_cellOffsets2D[i]);
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

			float2 neighbourPos 	  = RDS_RW_BUFFER_LOAD_I(float2, u_positions, neighbourIdx);
			float2 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float2 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float2(0, 1);
			
			// calc sth
			RDS_RW_BUFFER_STORE_I(float2, u_spatialLutDebugResultPositions,  neighbourCount, neighbourPos);
			neighbourCount++;
		}
	}

	while(neighbourCount < u_particleCount)
	{
		RDS_RW_BUFFER_STORE_I(float2, u_spatialLutDebugResultPositions,  neighbourCount, float2(9999, 9999));
		neighbourCount++;
	}
}

