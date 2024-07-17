#ifndef __rdsColor_HLSL__
#define __rdsColor_HLSL__


#if 0
#pragma mark --- ColorUtil-Impl ---
#endif
#if 1

#if 0

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

#else

float3 Color_Linear_to_sRGB(float3 x) { return select(x < 0.0031308, 12.92 * x, 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719); }
float3 Color_sRGB_to_Linear(float3 x) { return select(x < 0.04045, x / 12.92, -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864); }

#endif


#endif

#endif