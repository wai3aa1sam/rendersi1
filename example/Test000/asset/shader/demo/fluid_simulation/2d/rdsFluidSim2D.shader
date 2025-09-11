#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_calcExternalForce }
	Pass { CsFunc		Cs_updateSpatialLut }
	Pass { CsFunc		Cs_updateSpatialLutKeyToStartIndex }
	Pass { CsFunc		Cs_calcDensityData }
	Pass { CsFunc		Cs_calcViscosity }
	Pass { CsFunc		Cs_calcPressureForce }
	Pass { CsFunc		Cs_updatePosition }

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "rdsFluidSim2D_SpatialLut.hlsl"

#define RDS_NUM_THREADS 32

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

RDS_RW_BUFFER(float2, u_positions);
RDS_RW_BUFFER(float2, u_velocities);
RDS_RW_BUFFER(float2, u_densityData);
RDS_RW_BUFFER(float2, u_predictedPositions);

RDS_RW_BUFFER(uint3, u_spatialLut);				// x: particle_index, y: hash, z: key
RDS_RW_BUFFER(uint,  u_spatialLutKeyToStartIndex);

//RDS_BUFFER(uint, u_spatialKeys);
//RDS_BUFFER(uint, u_spatialOffsets);
//RDS_BUFFER(uint, u_spatialSortedIdxs);

//RDS_BUFFER(float2, u_sortedPositions);
//RDS_BUFFER(float2, u_sortedVelocities);
//RDS_BUFFER(float2, u_sortedPredictedPositions);

float	u_dt;
float 	u_gravity;
float2 	u_gravityDir;
float	u_collisionDamping;
float 	u_smoothingRadius;

float 	u_interactionInputStrength;
float 	u_interactionInputRadius;
float2 	u_interactionInputPoint;

float2 u_boundarySize;
float2 u_obstacleCenter;
float2 u_obstacleSize;

float 	u_particleMass;
uint 	u_particleCount;

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcExternalForce(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float2 position = RDS_RW_BUFFER_LOAD_I(float2, u_positions, 	tarPtcIdx);
	float2 velocity = RDS_RW_BUFFER_LOAD_I(float2, u_velocities, 	tarPtcIdx);

	// External forces (gravity and input interaction)
	float2 externalForces = float2(0, 0);
	{
		// Gravity
		float2 gravityAccel = u_gravityDir * u_gravity;
		
		// Input interactions modify gravity
		if (u_interactionInputStrength != 0) 
		{
			float2 	inputPointOffset 	= u_interactionInputPoint - position;
			float 	sqrDist 			= dot(inputPointOffset, inputPointOffset);
			if (sqrDist < u_interactionInputRadius * u_interactionInputRadius)
			{
				float 	dist			 = sqrt(sqrDist);
				float 	edge			= (dist / u_interactionInputRadius);
				float 	center			= 1 - edge;
				float2 	dirToCenter		= inputPointOffset / dist;

				float 	gravityWeight 	= 1 - (center * saturate(u_interactionInputStrength / 10));
				float2 	accel 			= gravityAccel * gravityWeight + dirToCenter * center * u_interactionInputStrength;
				accel -= velocity * center;
				externalForces += accel * u_particleMass;
			}
		}
		else
		{
			externalForces += gravityAccel * u_particleMass;
		}
	}

	float2 vel = velocity + externalForces / u_particleMass * u_dt;
	RDS_RW_BUFFER_STORE_I(float2, u_velocities, tarPtcIdx, vel);

	const float predictionFactor = 1 / 120.0;
	float2 predPos = position + vel * predictionFactor;
	RDS_RW_BUFFER_STORE_I(float2, u_predictedPositions, tarPtcIdx, predPos);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_updateSpatialLut(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	// init
	RDS_RW_BUFFER_STORE_I(uint, u_spatialLutKeyToStartIndex, tarPtcIdx, SpatialLut_invalidIdx);

	float2 pos = RDS_RW_BUFFER_LOAD_I(float2, u_predictedPositions, tarPtcIdx);
	// Update index buffer
	uint index 	= tarPtcIdx;
	int2 cell 	= SpatialLut_toCell2D(pos, u_smoothingRadius);
	uint hash 	= SpatialLut_hashCell2D(cell);
	uint key 	= SpatialLut_toKeyFromHash(hash, u_particleCount);
	
	uint3 spatialLut = uint3(index, hash, key);
	RDS_RW_BUFFER_STORE_I(uint3, u_spatialLut, tarPtcIdx, spatialLut);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_updateSpatialLutKeyToStartIndex(ComputeIn input)
{
	uint i 				= input.dispatchThreadId.x;
	bool isInBoundary 	=  i < u_particleCount;
	if (!isInBoundary) return;

	uint3 spatialLutEntry 		= RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, i);
	// this must not be access when i == 0
	uint3 prevSpatialLutEntry 	= RDS_RW_BUFFER_LOAD_I(uint3, u_spatialLut, i - 1);

	uint key 		= spatialLutEntry.z;
	uint keyPrev 	= i == 0 ? SpatialLut_invalidIdx : prevSpatialLutEntry.z;
	if (key != keyPrev)
	{
		RDS_RW_BUFFER_STORE_I(uint, u_spatialLutKeyToStartIndex, key, i);
	}
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcDensityData(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;


}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcViscosity(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;


}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcPressureForce(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;


}

void handleCollisions(uint ptcIdx)
{
	float2 pos = RDS_RW_BUFFER_LOAD_I(float2, u_positions, 	ptcIdx);
	float2 vel = RDS_RW_BUFFER_LOAD_I(float2, u_velocities, ptcIdx);

	// Keep particle inside bounds
	const float2 halfSize 	= u_boundarySize * 0.5;
	float2 edgeDist 		= halfSize - abs(pos);

	if (edgeDist.x <= 0)
	{
		pos.x = halfSize.x * sign(pos.x);
		vel.x *= -1 * u_collisionDamping;
	}
	if (edgeDist.y <= 0)
	{
		pos.y = halfSize.y * sign(pos.y);
		vel.y *= -1 * u_collisionDamping;
	}

	// Collide particle against the test obstacle
	#if 0
	const float2 	obstacleHalfSize 	= u_obstacleSize * 0.5;
	float2 			obstacleEdgeDist 	= obstacleHalfSize - abs(pos - u_obstacleCenter);

	if (obstacleEdgeDist.x >= 0 && obstacleEdgeDist.y >= 0)
	{
		if (obstacleEdgeDist.x < obstacleEdgeDist.y) 
		{
			pos.x = obstacleHalfSize.x * sign(pos.x - u_obstacleCenter.x) + u_obstacleCenter.x;
			vel.x *= -1 * u_collisionDamping;
		}
		else 
		{
			pos.y = obstacleHalfSize.y * sign(pos.y - u_obstacleCenter.y) + u_obstacleCenter.y;
			vel.y *= -1 * u_collisionDamping;
		}
	}
	#endif

	RDS_RW_BUFFER_STORE_I(float2, u_positions,  ptcIdx, pos);
	RDS_RW_BUFFER_STORE_I(float2, u_velocities, ptcIdx, vel);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_updatePosition(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float2 position = RDS_RW_BUFFER_LOAD_I(float2, u_positions, 	tarPtcIdx);
	float2 velocity = RDS_RW_BUFFER_LOAD_I(float2, u_velocities, 	tarPtcIdx);

	float2 pos = position + velocity * u_dt;
	RDS_RW_BUFFER_STORE_I(float2, u_positions, tarPtcIdx, pos);

	handleCollisions(tarPtcIdx);
}

