#ifndef __rdsPhongLighting_HLSL__
#define __rdsPhongLighting_HLSL__

#include "rdsLighting_Common.hlsl"
#include "rdsLight.hlsl"

#if 0
#pragma mark --- Phong-Impl ---
#endif
#if 1

LightingResult Phong_computeDirectLighting(Surface surface, float3 viewDir, float3 L, float3 colorLight, float intensity)
{
    LightingResult o = (LightingResult)0;

    float3 normal      = surface.normal;
    float3 baseColor   = surface.baseColor.rgb;

    float 	diff 	= max(dot(L, normal), 0.0);
    float3 	diffuse = diff * baseColor.xyz * colorLight;

    float3  halfwayDir 	= normalize(L + viewDir);
    float3  spec 		= pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    float3 	specular 	= surface.specular.xyz * spec;

    o.diffuse.rgb 	+= diffuse  * intensity;
    o.specular.rgb 	+= specular * intensity;

    return o;
}



#endif



#endif