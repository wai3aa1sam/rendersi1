#if 0
Shader {
	Properties {
		
	}

	Pass { CsFunc		Cs_initSpatialLut }
	// Pass { CsFunc		Cs_sort }	// must sort the lut before update startIndex
	Pass { CsFunc		Cs_updateSpatialLutKeyToStartIndex }

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "rdsSpatialLut3D.hlsl"

#define RDS_NUM_THREADS 32

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

#define DimT float3

RDS_RW_BUFFER(DimT, 	u_positions);
RDS_RW_BUFFER(uint3, 	u_spatialLut);				// x: particle_index, y: hash, z: key
RDS_RW_BUFFER(uint,  	u_spatialLutKeyToStartIndex);

float 	u_radius;
uint 	u_elementCount;

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_initSpatialLut(ComputeIn input)
{
	uint tarIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarIdx < u_elementCount;
	if (!isInBoundary) return;

	// init
	RDS_RW_BUFFER_STORE_I(uint, u_spatialLutKeyToStartIndex, tarIdx, u_elementCount);

	DimT pos = RDS_RW_BUFFER_LOAD_I(DimT, u_positions, tarIdx);
	// Update index buffer
	uint index 	= tarIdx;
	int3 cell 	= SpatialLut_toCell3D(pos, u_radius);
	uint hash 	= SpatialLut_hashCell3D(cell);
	uint key 	= SpatialLut_toKeyFromHash(hash, u_elementCount);
	
	uint3 spatialLut = uint3(index, hash, key);
	RDS_RW_BUFFER_STORE_I(uint3, u_spatialLut, tarIdx, spatialLut);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_updateSpatialLutKeyToStartIndex(ComputeIn input)
{
	uint i 				= input.dispatchThreadId.x;
	bool isInBoundary 	=  i < u_elementCount;
	if (!isInBoundary) return;

	uint3 spatialLutEntry 		= RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, i);
	// this must not be access when i == 0
	uint3 prevSpatialLutEntry 	= RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, i - 1);

	uint key 		= spatialLutEntry.z;
	uint keyPrev 	= i == 0 ? u_elementCount : prevSpatialLutEntry.z;
	if (key != keyPrev)
	{
		RDS_RW_BUFFER_STORE_I(uint, u_spatialLutKeyToStartIndex, key, i);
	}
}

