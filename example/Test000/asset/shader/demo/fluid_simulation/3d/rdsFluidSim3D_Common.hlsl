#ifndef __rdsFluidSim3D_Common_HLSL__
#define __rdsFluidSim3D_Common_HLSL__

#include "built-in/shader/rds_shader.hlsl"

// const float u_smoothingKernelPoly6;
// const float u_spikyKernelPow3;
// const float u_spikyKernelPow2;
// const float u_derivativeSpikyPow3;
// const float u_derivativeSpikyPow2;

static const uint 	SpatialLut_invalidIdx 	= UINT_MAX;
static const float3 s_kInvalid_position 	= float3(9999, 9999, 9999);

// 3d conversion: done
float smoothingKernelPoly6(float dist, float radius)
{
	if (dist < radius)
	{
		float scale = 315 / (64 * rds_pi * pow(abs(radius), 9));
		float v = radius * radius - dist * dist;
		return v * v * v * scale;
	}
	return 0;
}

float spikyKernelPow3(float dist, float radius)
{
	if (dist < radius)
	{
		float scale = 15 / (rds_pi * pow(radius, 6));
		float v = radius - dist;
		return v * v * v * scale;
	}
	return 0;
}

//Integrate[(h-r)^2 r^2 Sin[θ], {r, 0, h}, {θ, 0, π}, {φ, 0, 2*π}]
float spikyKernelPow2(float dist, float radius)
{
	if (dist < radius)
	{
		float scale = 15 / (2 * rds_pi * pow(radius, 5));
		float v = radius - dist;
		return v * v * scale;
	}
	return 0;
}

float derivativeSpikyPow3(float dist, float radius)
{
	if (dist <= radius)
	{
		float scale = 45 / (pow(radius, 6) * rds_pi);
		float v = radius - dist;
		return -v * v * scale;
	}
	return 0;
}

float derivativeSpikyPow2(float dist, float radius)
{
	if (dist <= radius)
	{
		float scale = 15 / (pow(radius, 5) * rds_pi);
		float v = radius - dist;
		return -v * scale;
	}
	return 0;
}

float densityKernel(float dist, float radius)
{
	//return smoothingKernelPoly6(dist, radius);
	return spikyKernelPow2(dist, radius);
}

float nearDensityKernel(float dist, float radius)
{
	return spikyKernelPow3(dist, radius);
}

float densityDerivative(float dist, float radius)
{
	return derivativeSpikyPow2(dist, radius);
}

float nearDensityDerivative(float dist, float radius)
{
	return derivativeSpikyPow3(dist, radius);
}

float viscosityKernel(float dist, float radius)
{
	return smoothingKernelPoly6(dist, radius);
}



#endif
