#ifndef __rdsPostProcessing_HLSL__
#define __rdsPostProcessing_HLSL__

/*
~ reference
- Post Processing Pipeline For Unity [https://github.com/GarrettGunnell/Post-Processing]
*/
/*
~ post processing pipeline
- fog
- bloom
- exposure
- white balancing
- contrast
- brightness
- color filtering
- saturation
- tonemapping
- gamma
*/

float3 PostProc_exposure(float3 v, float3 exposure)
{
	float3 o = v * exposure;
	return o;
}

float3 PostProc_whiteBalancing(float3 v)
{
	float3 o = v;
	return o;
}

float3 PostProc_contrast(float3 v, float3 contrast, float3 contrastOffset)
{
	float3 o = (v - contrastOffset) * contrast + contrastOffset;
	o = max(0.0, o);
	return o;
}

float3 PostProc_brightness(float3 v, float3 brightness)
{
	float3 o = v + brightness;
	return o;
}

float3 PostProc_colorFilter(float3 v, float4 colorFilter)
{
	float3 o = v * colorFilter.rgb;
	return o;
}

float3 PostProc_luminance(float3 v)
{
	float3 grayScale = float3(0.299, 0.587, 0.114);
	float3 o = dot(v, grayScale);
	return o;
}

float3 PostProc_saturation(float3 v, float3 luminance, float3 saturation)
{
	float3 o = lerp(luminance, v, saturation);
	return o;
}

float3 PostProc_gammaEncoding(float3 v, float gamma = 2.2) { return pow(v, 1.0 / gamma); }
float3 PostProc_gammaDecoding(float3 v, float gamma = 2.2) { return pow(v, gamma); }


#endif