#ifndef __rdsFunction_Common_HLSL__
#define __rdsFunction_Common_HLSL__

#include "built-in/shader/interop/rdsShaderInterop.hlsl"

/*
reference:
~ https://github.com/turanszkij/WickedEngine
*/
uint3 unflatten3D(uint idx, uint3 dimensions)
{
	const uint z = idx / (dimensions.x * dimensions.y);
	idx -= (z * dimensions.x * dimensions.y);
	const uint y = idx / dimensions.x;
	const uint x = idx % dimensions.x;
	return uint3(x, y, z);
}

float2 remapNeg11To01(float2 v)
{
    return 0.5 * v + 0.5;
}

float3 remapNeg11To01(float3 v)
{
    return 0.5 * v + 0.5;
}

float3 remapNeg11To01_Inv_Y(float3 v)
{
    return float3(0.5f, -0.5f, 0.5f) * v + 0.5;
}

float2 remap01ToNeg11(float2 v)
{
    return 2.0 * v - 1.0;
}

float3 remap01ToNeg11(float3 v)
{
    return 2.0 * v - 1.0;
}

float3 remap01ToNeg11_Inv_Y(float3 v)
{
    float3 o = remap01ToNeg11(v);
	o.y = -o.y;
    return o;
}

bool isInBoundary01(float a)  { return a == saturate(a); }
bool isInBoundary01(float2 a) { return all(a == saturate(a)); }
bool isInBoundary01(float3 a) { return all(a == saturate(a)); }
bool isInBoundary01(float4 a) { return all(a == saturate(a)); }


#endif