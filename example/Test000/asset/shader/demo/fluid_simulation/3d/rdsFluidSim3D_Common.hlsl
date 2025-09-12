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
float smoothingKernelPoly6(float dst, float radius)
{
	if (dst < radius)
	{
		float scale = 315 / (64 * rds_pi * pow(abs(radius), 9));
		float v = radius * radius - dst * dst;
		return v * v * v * scale;
	}
	return 0;
}

// 3d conversion: done
float spikyKernelPow3(float dst, float radius)
{
	if (dst < radius)
	{
		float scale = 15 / (rds_pi * pow(radius, 6));
		float v = radius - dst;
		return v * v * v * scale;
	}
	return 0;
}

//Integrate[(h-r)^2 r^2 Sin[θ], {r, 0, h}, {θ, 0, π}, {φ, 0, 2*π}]
float spikyKernelPow2(float dst, float radius)
{
	if (dst < radius)
	{
		float scale = 15 / (2 * rds_pi * pow(radius, 5));
		float v = radius - dst;
		return v * v * scale;
	}
	return 0;
}

float derivativeSpikyPow3(float dst, float radius)
{
	if (dst <= radius)
	{
		float scale = 45 / (pow(radius, 6) * rds_pi);
		float v = radius - dst;
		return -v * v * scale;
	}
	return 0;
}

float derivativeSpikyPow2(float dst, float radius)
{
	if (dst <= radius)
	{
		float scale = 15 / (pow(radius, 5) * rds_pi);
		float v = radius - dst;
		return -v * scale;
	}
	return 0;
}

float densityKernel(float dst, float radius)
{
	//return smoothingKernelPoly6(dst, radius);
	return spikyKernelPow2(dst, radius);
}

float nearDensityKernel(float dst, float radius)
{
	return spikyKernelPow3(dst, radius);
}

float densityDerivative(float dst, float radius)
{
	return derivativeSpikyPow2(dst, radius);
}

float nearDensityDerivative(float dst, float radius)
{
	return derivativeSpikyPow3(dst, radius);
}

float viscosityKernel(float dst, float radius)
{
	return smoothingKernelPoly6(dst, radius);
}



#endif
