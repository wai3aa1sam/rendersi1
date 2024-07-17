#ifndef __rdsToneMapping_HLSL__
#define __rdsToneMapping_HLSL__

#if 0
#pragma mark --- ToneMapping-Impl ---
#endif
#if 1

float3 ToneMapping_reinhard(float3 v) { return v / (v + float3(1.0, 1.0, 1.0)); }

// From http://filmicgames.com/archives/75
float3 ToneMapping_uncharted2(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

#endif

#endif