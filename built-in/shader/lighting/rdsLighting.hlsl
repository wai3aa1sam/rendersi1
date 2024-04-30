#ifndef __rdsLighting_HLSL__
#define __rdsLighting_HLSL__

#include "rdsLighting_Common.hlsl"
#include "rdsLight.hlsl"
#include "rdsPbrLighting.hlsl"

#if 0
#pragma mark --- Lighting-Impl ---
#endif
#if 1


float Lighting_computeSpotIntensity(Light light, float3 L)
{
	float spotAngle 		= Light_getSpotCosAngle(light);			// set cos radian in app side
	float spotInnerAngle 	= Light_getSpotInnerCosAngle(light);	// set cos radian in app side

	float cosAngle = dot(light.directionVs.xyz, -L);
	return smoothstep(spotAngle, spotInnerAngle, cosAngle);
}

LightingResult Lighting_computeLighting(Light light, Surface surface, float3 dirView)
{
	LightingResult o = (LightingResult)0;
	
	bool isEnabled 			= Light_isEnabled(light);
	bool isDirectionalLight	= light.type == rds_LightType_Directional;
	bool isSpotLight		= light.type == rds_LightType_Spot;

	float3 posVsLight 	= light.positionVs.xyz;
	float3 dirLight 	= normalize(-light.directionVs).xyz;
	float3 dirLightPos 	= posVsLight - surface.posVs;

	float3 L 			= lerp(dirLightPos, dirLight, isDirectionalLight);
	float  lightSqLen 	= dot(L, L) * (1 - isDirectionalLight);
	float  lightRange 	= light.range;

	if (!isEnabled)
	{
		return o;
	}

	if (!isDirectionalLight && lightSqLen > lightRange * lightRange)
	{
		return o;
	}

	float L_len = sqrt(lightSqLen);
	L = L / (L_len + rds_epsilon);

	float attenutaion 	= Light_computeAttenuation(light, L_len);
	float intensity		= light.intensity * attenutaion;
	intensity = lerp(intensity, intensity * Lighting_computeSpotIntensity(light, L), (float)isSpotLight);

	o = Pbr_basic_lighting(surface, dirView, L, light.color.xyz, intensity);
	return o;
}

#endif


#endif