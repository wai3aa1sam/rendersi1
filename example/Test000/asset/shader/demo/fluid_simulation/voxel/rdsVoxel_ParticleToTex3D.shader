#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_particleToTex3D }

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "../3d/rdsSpatialLut3D.hlsl"

#define RDS_NUM_THREADS 8

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

RDS_IMAGE_3D(half, 		u_tex3D);

RDS_RW_BUFFER(float3,  	u_positions);
RDS_RW_BUFFER(uint3, 	u_spatialLut);				// x: particle_index, y: hash, z: key
RDS_RW_BUFFER(uint,  	u_spatialLutKeyToStartIndex);

uint 	u_particleCount;
float 	u_smoothingRadius;
float3  u_boundingSize;
uint3 	u_voxelMapSize;

uint calcParticleCountAtPoint(float3 samplingPt, float radius, uint elemntCount)
{
	uint neighbour_particleCount = 0;

	// ---
	float3 	tarPos 		= samplingPt; // RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, tarPtcIdx);
	int3 	originCell 	= SpatialLut_toCell3D(tarPos, radius);
	float 	sqrRadius 	= radius * radius;

	// Neighbour search
	for (int i = 0; i < SpatialLut_cellOffsetCount; i ++)
	{
		uint hash 		= SpatialLut_hashCell3D(originCell + SpatialLut_cellOffsets3D[i]);
		uint key 		= SpatialLut_toKeyFromHash(hash, elemntCount);
		uint curIndex 	= RDS_RW_BUFFER_LOAD_I(uint, u_spatialLutKeyToStartIndex, key);

		while (curIndex < elemntCount)
		{
			uint3 spatialLut = RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, curIndex);
			curIndex++;

			if (spatialLut[2] != key) 		break;	  // not same key chunk

			uint neighbourIdx 	= spatialLut[0];
			float3 neighbourPos = RDS_RW_BUFFER_LOAD_I(float3, u_positions, neighbourIdx);

			if (spatialLut[1] != hash) 		continue; // not same hash
			//if (neighbourIdx == tarPtcIdx) 	continue; // skip self, except calcDesnsity
			if (all(neighbourPos == samplingPt)) 	continue; // skip self, except calcDesnsity

			float3 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float3 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float3(0, 1, 0);
			
			// calc sth
			neighbour_particleCount++;
		}
	}

	return neighbour_particleCount;
}

[numthreads(RDS_NUM_THREADS, RDS_NUM_THREADS, RDS_NUM_THREADS)]
void Cs_particleToTex3D(ComputeIn input)
{
	uint3 id 			= input.dispatchThreadId;
	bool isInBoundary 	= !(id.x >= u_voxelMapSize.x || id.y >= u_voxelMapSize.y || id.z >= u_voxelMapSize.z);
	if (!isInBoundary) return;

	float3 posTex 	= id / (u_voxelMapSize - 1.0);		// remap to [0, 1]
	//float3 posWs 	= (posTex - 0.5) * u_boundingSize;
	float3 posWs 	= (remap01ToNeg11(posTex)) * u_boundingSize;

	uint particleCount = calcParticleCountAtPoint(posWs, u_smoothingRadius, u_particleCount);
	
	// TODO: RDS_IMAGE_3D_STORE()
	RWTexture3D<half> tex = RDS_IMAGE_3D_GET(half,  u_tex3D);
	tex[id] = particleCount / (half)u_particleCount;
}

