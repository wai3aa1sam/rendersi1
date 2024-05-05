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

float Lighting_computeAttenuation(Light light, float lightSqDist)
{
	float lightRange = light.range;
	return 1.0f - smoothstep(lightRange * 0.75, lightRange, lightSqDist);
	//return 1.0f - saturate(lightSqDist / ((lightRange * lightRange) + rds_epsilon));
}

LightingResult Lighting_computeLighting(Light light, Surface surface, float3 dirView)
{
	LightingResult o = (LightingResult)0;
	
	bool isEnabled 			= Light_isEnabled(light);
	bool isDirectionalLight	= Light_isDirectionalLight(light);
	bool isSpotLight		= Light_isSpotLight(light);

	float3 posVsLight 	= light.positionVs.xyz;
	float3 dirLight 	= normalize(-light.directionVs).xyz;
	float3 dirLightPos 	= posVsLight - surface.posVs;

	//o.diffuse.xyz = float3(0.1, 0.1, 0.1);
	//return o;

	float3 L 			= lerp(dirLightPos, dirLight, isDirectionalLight);
	float  lightSqLen 	= dot(L, L);
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

	float attenutaion 	= Lighting_computeAttenuation(light, lerp(L_len, 0.0, isDirectionalLight));
	float intensity		= light.intensity * attenutaion;
	intensity = lerp(intensity, intensity * Lighting_computeSpotIntensity(light, L), isSpotLight);

	o = Pbr_basic_lighting(surface, dirView, L, light.color.xyz, intensity);

	//o.diffuse.rgb = float3(lightSqLen, lightSqLen, lightSqLen);
	//o.diffuse.rgb = float3(intensity, intensity, intensity);
	//o.diffuse.rgb = light.color.xyz;
	//o.diffuse.rgb = L;
	//o.diffuse.rgb = float3(0.1, 0.1, 0.1);

	return o;
}

#endif


#endif