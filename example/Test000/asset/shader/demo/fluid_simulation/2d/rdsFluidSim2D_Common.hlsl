#ifndef __rdsFluidSim2D_Common_HLSL__
#define __rdsFluidSim2D_Common_HLSL__

#include "built-in/shader/rds_shader.hlsl"

const float Poly6ScalingFactor;
const float SpikyPow3ScalingFactor;
const float SpikyPow2ScalingFactor;
const float SpikyPow3DerivativeScalingFactor;
const float SpikyPow2DerivativeScalingFactor;

static const uint SpatialLut_invalidIdx = UINT_MAX;

float smoothingKernelPoly6(float dist, float radius)
{
	if (dist < radius)
	{
		float v = radius * radius - dist * dist;
		return v * v * v * Poly6ScalingFactor;
	}
	return 0;
}

float spikyKernelPow3(float dist, float radius)
{
	if (dist < radius)
	{
		float v = radius - dist;
		return v * v * v * SpikyPow3ScalingFactor;
	}
	return 0;
}

float spikyKernelPow2(float dist, float radius)
{
	if (dist < radius)
	{
		float v = radius - dist;
		return v * v * SpikyPow2ScalingFactor;
	}
	return 0;
}

float derivativeSpikyPow3(float dist, float radius)
{
	if (dist <= radius)
	{
		float v = radius - dist;
		return -v * v * SpikyPow3DerivativeScalingFactor;
	}
	return 0;
}

float derivativeSpikyPow2(float dist, float radius)
{
	if (dist <= radius)
	{
		float v = radius - dist;
		return -v * SpikyPow2DerivativeScalingFactor;
	}
	return 0;
}

float densityKernel(float dist, float radius)
{
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
