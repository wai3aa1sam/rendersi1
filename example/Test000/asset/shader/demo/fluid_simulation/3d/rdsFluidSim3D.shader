#if 0
Shader {
	Properties {
		
	}
	
	Pass { CsFunc		Cs_calcExternalForce }
	Pass { CsFunc		Cs_calcDensityData }
	Pass { CsFunc		Cs_calcPressureForce }
	Pass { CsFunc		Cs_calcViscosity }
	Pass { CsFunc		Cs_updatePosition }

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

RDS_RW_BUFFER(float3, u_positions);
RDS_RW_BUFFER(float3, u_velocities);
RDS_RW_BUFFER(float2, u_densityData);
RDS_RW_BUFFER(float3, u_predictedPositions);

RDS_RW_BUFFER(uint3, u_spatialLut);					// x: particle_index, y: hash, z: key
RDS_RW_BUFFER(uint,  u_spatialLutKeyToStartIndex);

//RDS_BUFFER(float2, u_sortedPositions);
//RDS_BUFFER(float2, u_sortedVelocities);
//RDS_BUFFER(float2, u_sortedPredictedPositions);

float	u_dt;
float 	u_gravity;
float3 	u_gravityDir;
float	u_collisionDamping;
float 	u_smoothingRadius;

float u_targetDensity;
float u_pressureMultiplier;
float u_nearPressureMultiplier;
float u_viscosityStrength;

float 	u_interactionInputStrength;
float 	u_interactionInputRadius;
float3 	u_interactionInputPoint;

float3 u_boundarySize;
//float3 u_obstacleCenter;
//float3 u_obstacleSize;

float 	u_particleMass;
uint 	u_particleCount;

float4x4 u_objToWorld;
float4x4 u_worldToObj;

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcExternalForce(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float3 position = RDS_RW_BUFFER_LOAD_I(float3, u_positions, 	tarPtcIdx);
	float3 velocity = RDS_RW_BUFFER_LOAD_I(float3, u_velocities, 	tarPtcIdx);

	// External forces (gravity and input interaction)
	float3 externalForces = 0;
	{
		// Gravity
		float3 gravityAccel = u_gravityDir * u_gravity;
		
		// Input interactions modify gravity
		if (u_interactionInputStrength != 0) 
		{
			float3 	inputPointOffset 	= u_interactionInputPoint - position;
			float 	sqrDist 			= dot(inputPointOffset, inputPointOffset);
			if (sqrDist < u_interactionInputRadius * u_interactionInputRadius)
			{
				float 	dist			 = sqrt(sqrDist);
				float 	edge			= (dist / u_interactionInputRadius);
				float 	center			= 1 - edge;
				float3 	dirToCenter		= inputPointOffset / dist;

				float 	gravityWeight 	= 1 - (center * saturate(u_interactionInputStrength / 10));
				float3 	accel 			= gravityAccel * gravityWeight + dirToCenter * center * u_interactionInputStrength;
				accel -= velocity * center;
				externalForces += accel * u_particleMass;
			}
		}
		else
		{
			externalForces += gravityAccel * u_particleMass;
		}
	}

	float3 vel = velocity + externalForces / u_particleMass * u_dt;
	RDS_RW_BUFFER_STORE_I(float3, u_velocities, tarPtcIdx, vel);

	const float predictionFactor = 1 / 120.0;
	float3 predPos = position + vel * predictionFactor;
	RDS_RW_BUFFER_STORE_I(float3, u_predictedPositions, tarPtcIdx, predPos);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcDensityData(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	= tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float density = 0;
	float nearDensity = 0;

	float3 	tarPos 		= RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, tarPtcIdx);
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

			float3 neighbourPos 	  = RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, neighbourIdx);
			float3 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float3 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float3(0, 1, 0);
			
			// calc sth
			density 	+= densityKernel(dist, u_smoothingRadius);
			nearDensity += nearDensityKernel(dist, u_smoothingRadius);
		}
	}

	RDS_RW_BUFFER_STORE_I(float2, u_densityData, tarPtcIdx, float2(density, nearDensity));
}

float pressureFromDensity(float density)
{
	return (density - u_targetDensity) * u_pressureMultiplier;
}

float nearPressureFromDensity(float nearDensity)
{
	return u_nearPressureMultiplier * nearDensity;
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcPressureForce(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	= tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;
	
	float3 pressureForce = 0;

	float2 densityData 	= RDS_RW_BUFFER_LOAD_I(float2, u_densityData, tarPtcIdx);
	float  density 		= densityData.x;
	float  densityNear 	= densityData.y;
	float  pressure 	= pressureFromDensity(density);
	float  nearPressure = nearPressureFromDensity(densityNear);

	float3 	tarPos 		= RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, tarPtcIdx);
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
			if (neighbourIdx == tarPtcIdx) 	continue; // skip self, except calcDesnsity

			float3 neighbourPos 	  = RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, neighbourIdx);
			float3 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float3 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float3(0, 1, 0);
			
			// calc sth
			float2 	neighbourDensityData 	= RDS_RW_BUFFER_LOAD_I(float2, u_densityData, neighbourIdx);
			float 	neighbourDensity		= neighbourDensityData.x;
			float 	neighbourNearDensity	= neighbourDensityData.y;
			float 	neighbourPressure		= pressureFromDensity(neighbourDensity);
			float 	neighbourNearPressure	= nearPressureFromDensity(neighbourNearDensity);

			float sharedPressure = (pressure + neighbourPressure) * 0.5;
			float sharedNearPressure = (nearPressure + neighbourNearPressure) * 0.5;

			pressureForce += dirToNeighbour * densityDerivative(dist, u_smoothingRadius) * sharedPressure / neighbourDensity;
			pressureForce += dirToNeighbour * nearDensityDerivative(dist, u_smoothingRadius) * sharedNearPressure / neighbourNearDensity;
		}
	}

	float3 accel = pressureForce / density;
	float3 vel = RDS_RW_BUFFER_LOAD_I(float3, u_velocities, tarPtcIdx);
	vel += accel * u_dt;
	RDS_RW_BUFFER_STORE_I(float3, u_velocities, tarPtcIdx, vel);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_calcViscosity(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	=  tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float3 viscosityForce 	= 0;
	const float3 velocity 	= RDS_RW_BUFFER_LOAD_I(float3, u_velocities, tarPtcIdx);

	float3 	tarPos 		= RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, tarPtcIdx);
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
			if (neighbourIdx == tarPtcIdx) 	continue; // skip self, except calcDesnsity

			float3 neighbourPos 	  = RDS_RW_BUFFER_LOAD_I(float3, u_predictedPositions, neighbourIdx);
			float3 offsetToNeighbour  = neighbourPos - tarPos;
			float  sqrDistToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

			// skip outside radius
			if (sqrDistToNeighbour > sqrRadius) continue;

			float  dist 			= sqrt(sqrDistToNeighbour);
			float3 dirToNeighbour 	= dist > 0 ? offsetToNeighbour / dist : float3(0, 1, 0);
			
			// calc sth
			float3 neighbourVelocity 	= RDS_RW_BUFFER_LOAD_I(float3, u_velocities, neighbourIdx);
			viscosityForce += (neighbourVelocity - velocity) * viscosityKernel(dist, u_smoothingRadius);
		}
	}

	float3 vel = velocity + viscosityForce * u_viscosityStrength * u_dt;
	RDS_RW_BUFFER_STORE_I(float3, u_velocities, tarPtcIdx, vel);
}

void handleCollisions(uint ptcIdx)
{
	float3 pos = RDS_RW_BUFFER_LOAD_I(float3, u_positions, 	ptcIdx);
	float3 vel = RDS_RW_BUFFER_LOAD_I(float3, u_velocities, ptcIdx);

	pos = mul(u_worldToObj, float4(pos, 1.0)).xyz;
	vel = mul(u_worldToObj, float4(vel, 0.0)).xyz;

	// Keep particle inside bounds
	const float3 halfSize 	= 1.0; //0.5; // local space // u_boundarySize * 0.5;
	float3 edgeDist 		= halfSize - abs(pos);

	if (edgeDist.x <= 0.0)
	{
		pos.x = halfSize.x * sign(pos.x);
		vel.x *= -1 * u_collisionDamping;
	}
	if (edgeDist.y <= 0.0)
	{
		pos.y = halfSize.y * sign(pos.y);
		vel.y *= -1 * u_collisionDamping;
	}
	if (edgeDist.z <= 0.0)
	{
		pos.z = halfSize.z * sign(pos.z);
		vel.z *= -1 * u_collisionDamping;
	}

	// Collide particle against the test obstacle
	#if 0
	const float3 	obstacleHalfSize 	= u_obstacleSize * 0.5;
	float3 			obstacleEdgeDist 	= obstacleHalfSize - abs(pos - u_obstacleCenter);

	if (obstacleEdgeDist.x >= 0 && obstacleEdgeDist.y >= 0 && obstacleEdgeDist.z >= 0)
	{
		if (obstacleEdgeDist.x <= 0) 
		{
			pos.x = obstacleHalfSize.x * sign(pos.x - u_obstacleCenter.x) + u_obstacleCenter.x;
			vel.x *= -1 * u_collisionDamping;
		}
		
		if (obstacleEdgeDist.y <= 0) 
		{
			pos.y = obstacleHalfSize.y * sign(pos.y - u_obstacleCenter.y) + u_obstacleCenter.y;
			vel.y *= -1 * u_collisionDamping;
		}

		if (obstacleEdgeDist.z <= 0) 
		{
			pos.z = obstacleHalfSize.z * sign(pos.z - u_obstacleCenter.z) + u_obstacleCenter.z;
			vel.z *= -1 * u_collisionDamping;
		}
	}
	#endif

	pos = mul(u_objToWorld, float4(pos, 1.0)).xyz;
	vel = mul(u_objToWorld, float4(vel, 0.0)).xyz;

	RDS_RW_BUFFER_STORE_I(float3, u_positions,  ptcIdx, pos);
	RDS_RW_BUFFER_STORE_I(float3, u_velocities, ptcIdx, vel);
}

[numThreads(RDS_NUM_THREADS, 1, 1)]
void Cs_updatePosition(ComputeIn input)
{
	uint tarPtcIdx 		= input.dispatchThreadId.x;
	bool isInBoundary 	= tarPtcIdx < u_particleCount;
	if (!isInBoundary) return;

	float3 position = RDS_RW_BUFFER_LOAD_I(float3, u_positions, 	tarPtcIdx);
	float3 velocity = RDS_RW_BUFFER_LOAD_I(float3, u_velocities, 	tarPtcIdx);

	float3 pos = position + velocity * u_dt;
	RDS_RW_BUFFER_STORE_I(float3, u_positions, tarPtcIdx, pos);

	handleCollisions(tarPtcIdx);
}

