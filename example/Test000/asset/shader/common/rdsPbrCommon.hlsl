/*
references:
~ https://learnopengl.com/PBR/Lighting
*/

#ifndef __rdsPbrCommon_HLSL__
#define __rdsPbrCommon_HLSL__

#include "rdsCommon.hlsl"

float3 Pbr_fresnelSchlick(float cosTheta, float3 baseRefl)
{
    return baseRefl + (1.0 - baseRefl) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float Pbr_distributionGGX(float dotNH, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float dotNH2 = dotNH * dotNH;
	
    float num   = a2;
    float denom = (dotNH * (a2 - 1.0) + 1.0);
    denom       = rds_pi * denom * denom;
	
    return num / denom;
}

float Pbr_geometrySchlickGGX(float cosTheta, float roughness)
{
    float r         = (roughness + 1.0);
    float k         = (r * r) / 8.0;

    float num   = cosTheta;
    float denom = cosTheta * (1.0 - k) + k;
	
    return num / denom;
}

float Pbr_geometrySmith(float dotNV, float dotNL, float roughness)
{
    float ggx2  = Pbr_geometrySchlickGGX(dotNV, roughness);
    float ggx1  = Pbr_geometrySchlickGGX(dotNL, roughness);
	
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

    float dotHV = max(dot(dirHalf,  dirView), 0.0);
    float dotNH = max(dot(normal,   dirHalf), 0.0);
    float dotNV = max(dot(normal,   dirView), 0.0);
    float dotNL = max(dot(normal,   dirLight), 0.0);

    float3 baseRefl = float3(0.04);   // non-metallic surfaces
    baseRefl        = lerp(baseRefl, albedo, surface.metallic);

    float3  fresnel     = Pbr_fresnelSchlick(dotNV, baseRefl);                  // dotNV or dotHV
    float   normalDist  = Pbr_distributionGGX(dotNH, surface.roughness);        // speculat highlight, distriGGX give us % of mircofacet align H 
    float   geo         = Pbr_geometrySmith(dotNV, dotNL, surface.roughness);

    float3  spec_numer  = normalDist * geo * fresnel;
    float   spec_denom  = 4.0 * max(dot(normal, dirView), 0.0) * max(dot(normal, dirLight), 0.0) + 0.0001;
    float3  specular    = spec_numer / spec_denom;

    float3 kSpecular = fresnel;
    float3 kDiffuse  = float3(1.0) - kSpecular;
    kDiffuse        *= 1.0 - surface.metallic;  // metallic surface absorb all diffuse 
    
    float   lambert = colorLight / rds_pi;
    float3  brdf    = kDiffuse * lambert + specular;

    float   attenuation = Lighting_attenuation(posWs, posLight);
    float3  radiance    = colorLight * attenuation;
    float3  o           = brdf * radiance * dotNL;

    //o = fresnel;

    return o;
}

#endif