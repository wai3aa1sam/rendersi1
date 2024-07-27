#ifndef __rdsLighting_HLSL__
#define __rdsLighting_HLSL__

/*
~ reference:
- Lighting Model - [https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf]
*/

#include "rdsLighting_Common.hlsl"
#include "rdsLight.hlsl"
#include "rdsPbrLighting.hlsl"
#include "rdsPhongLighting.hlsl"

#if 0
#pragma mark --- Lighting-Impl ---
#endif
#if 1

float3 Lighting_computeViewDir_Ws(float3 pos, float3 cameraPos)
{
	float3 viewPos;
	viewPos = cameraPos;
	float3 viewDir = normalize(viewPos - pos);
	return viewDir;
}

float3 Lighting_computeViewDir_Vs(float3 pos, float3 cameraPos)
{
	float3 viewPos;
	viewPos = float3(0.0, 0.0, 0.0);
	float3 viewDir = normalize(viewPos - pos);
	return viewDir;
}

float Lighting_computeSpotIntensity(Light light, float3 lightDir, float3 L)
{
	float spotAngle 		= Light_getSpotCosAngle(light);			// set cos radian in app side
	float spotInnerAngle 	= Light_getSpotInnerCosAngle(light);	// set cos radian in app side

	float cosAngle = dot(lightDir.xyz, L);
	return smoothstep(spotAngle, spotInnerAngle, cosAngle);
}

/*
~ reference:
- Lighting Model - [https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf]
*/
float Lighting_computeAttenuation(Light light, float lightSqDist)
{
	float lightRange = light.range;
	float denom = 1 / (lightSqDist + 1);
	float falloff = sqaure(saturate(1.0 - sqaure(lightSqDist * sqaure(rcp(lightRange)))));
	falloff *= denom;
	return falloff;

	//return 1.0f - smoothstep(lightRange * 0.75, lightRange, lightSqDist);
	//return 1.0f - saturate(lightSqDist / ((lightRange * lightRange) + rds_epsilon));
}

LightingResult Lighting_computeLighting_Impl(Light light, float3 lightPos, float3 lightDir, Surface surface, float3 viewDir)
{
	LightingResult o = (LightingResult)0;
	
	bool isEnabled 			= Light_isEnabled(light);
	bool isDirectionalLight	= Light_isDirectionalLight(light);
	bool isSpotLight		= Light_isSpotLight(light);

	float3 lightPosDir 	= lightPos - surface.pos;
	float3 L 			= lerp(lightPosDir, -lightDir, isDirectionalLight);
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

	float attenutaion 	= light.intensity * Lighting_computeAttenuation(light, lerp(lightSqLen, 0.0, isDirectionalLight));

	attenutaion = !isSpotLight ? attenutaion : attenutaion * Lighting_computeSpotIntensity(light, lightDir, -L);

	o = Pbr_computeDirectLighting(surface, viewDir, L, light.color.xyz, attenutaion);
	//o = Phong_computeDirectLighting(surface, viewDir, L, light.color.xyz, attenutaion);

	//o.diffuse.rgb = float3(lightSqLen, lightSqLen, lightSqLen);
	//o.diffuse.rgb = lightPosDir;
	//o.diffuse.rgb = float3(intensity, intensity, intensity);
	//o.diffuse.rgb = float3(attenutaion, attenutaion, attenutaion);
	//o.diffuse.rgb = light.color.xyz;
	//o.diffuse.rgb = L;
	//o.diffuse.rgb = float3(0.1, 0.1, 0.1);

	//o.specular.rgb = 0.0;

	return o;
}


LightingResult Lighting_computeLighting_Ws(Light light, Surface surface, float3 cameraPos, out float3 viewDir)
{
	LightingResult o = (LightingResult)0;
	
	viewDir = Lighting_computeViewDir_Ws(surface.pos, cameraPos);
	o = Lighting_computeLighting_Impl(light, light.positionWs.xyz, light.directionWs.xyz, surface, viewDir);
	return o;
}

LightingResult Lighting_computeLighting_Vs(Light light, Surface surface, float3 cameraPos, out float3 viewDir)
{
	LightingResult o = (LightingResult)0;

	viewDir = Lighting_computeViewDir_Vs(surface.pos, cameraPos);
	o = Lighting_computeLighting_Impl(light, light.positionVs.xyz, light.directionVs.xyz, surface, viewDir);
	return o;
}

LightingResult Lighting_computeForwardLighting_Ws(Surface surface, float3 cameraPos, out float3 viewDir)
{
	LightingResult 	o = (LightingResult)0;

	uint 			lightCount 		= rds_Lights_getLightCount();
	for (uint iLight = 0; iLight < lightCount; ++iLight)
	{
		Light light = rds_Lights_get(iLight);

		LightingResult result = Lighting_computeLighting_Ws(light, surface, cameraPos, viewDir);
		o.diffuse 	+= result.diffuse;
		o.specular 	+= result.specular;
	}
	
	return o;
}

LightingResult Lighting_computeForwardLighting_Vs(Surface surface, float3 cameraPos, out float3 viewDir)
{
	LightingResult 	o = (LightingResult)0;

	uint 			lightCount 		= rds_Lights_getLightCount();
	for (uint iLight = 0; iLight < lightCount; ++iLight)
	{
		Light light = rds_Lights_get(iLight);

		LightingResult result = Lighting_computeLighting_Vs(light, surface, cameraPos, viewDir);
		o.diffuse 	+= result.diffuse;
		o.specular 	+= result.specular;
	}

	return o;
}


#endif


#endif