/*
references:
~ https://learnopengl.com/PBR/Lighting
*/

#ifndef __rdsPbrCommon_HLSL__
#define __rdsPbrCommon_HLSL__

#include "rdsCommon.hlsl"

float3 Pbr_fresnelSchlick(float3 dirHalf, float3 dirView, float3 baseRefl)
{
    float cosTheta = max(dot(dirHalf, dirView), 0.0);
    return baseRefl + (1.0 - baseRefl) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float Pbr_distributionGGX(float3 normal, float3 dirHalf, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float cosTheta  = max(dot(normal, dirHalf), 0.0);
    float cosTheta2 = cosTheta * cosTheta;
	
    float num   = a2;
    float denom = (cosTheta * (a2 - 1.0) + 1.0);
    denom       = rds_pi * denom * denom;
	
    return num / denom;
}

float Pbr_geometrySchlickGGX(float3 normal, float3 dir, float roughness)
{
    float cosTheta  = max(dot(normal, dir), 0.0);
    float r         = (roughness + 1.0);
    float k         = (r * r) / 8.0;

    float num   = cosTheta;
    float denom = cosTheta * (1.0 - k) + k;
	
    return num / denom;
}

float Pbr_geometrySmith(float3 normal, float3 dirView, float3 dirLight, float roughness)
{
    float ggx2  = Pbr_geometrySchlickGGX(normal, dirView, roughness);
    float ggx1  = Pbr_geometrySchlickGGX(normal, dirLight, roughness);
	
    return ggx1 * ggx2;
}

float3 Pbr_basic_lighting(rds_Surface surface, float3 posView, float3 posLight, float3 colorLight)
{
    float3 posWs    = surface.posWS;
    float3 normal   = surface.normal;
    float3 albedo   = surface.color;

    float3 dirView  = normalize(posView - posWs);
    float3 dirLight = normalize(posLight - posWs);
    float3 dirHalf  = normalize(dirView + dirLight);

    float3 baseRefl = float3(0.04);   // non-metallic surfaces
    baseRefl        = lerp(baseRefl, albedo, surface.metallic);

    float3  fresnel     = Pbr_fresnelSchlick(dirHalf, dirView, baseRefl);
    float   normalDist  = Pbr_distributionGGX(normal, dirHalf, surface.roughness);       
    float   geo         = Pbr_geometrySmith(normal, dirView, dirLight, surface.roughness);

    float3  spec_numer  = normalDist * geo * fresnel;
    float   spec_denom  = 4.0 * max(dot(normal, dirView), 0.0) * max(dot(normal, dirLight), 0.0) + 0.0001;
    float3  specular    = spec_numer / spec_denom;

    float3 kSpecular = fresnel;
    float3 kDiffuse  = float3(1.0) - kSpecular;
    kDiffuse        *= 1.0 - surface.metallic;  // metallic surface absorb all diffuse 
    
    float   lambert = colorLight / rds_pi;
    float3  brdf    = kDiffuse * lambert + specular;

    float   cosThetaNL  = max(dot(normal, dirLight), 0.0);   
    float   attenuation = Lighting_attenuation(posWs, posLight);
    float3  radiance    = colorLight * attenuation;
    float3  o           = brdf * radiance* cosThetaNL;

    return o;
}

#endif