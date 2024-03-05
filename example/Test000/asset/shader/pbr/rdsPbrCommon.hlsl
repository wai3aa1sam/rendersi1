/*
references:
~ https://learnopengl.com/PBR/Lighting
*/

#ifndef __rdsPbrCommon_HLSL__
#define __rdsPbrCommon_HLSL__

#include "../common/rdsCommon.hlsl"

float3 Pbr_fresnelSchlick(float cosTheta, float3 baseRefl)
{
    return baseRefl + (1.0 - baseRefl) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 Pbr_fresnelSchlick(float cosTheta, float3 baseRefl, float roughness)
{
    //return baseRefl + (max(float3(1.0 - roughness), baseRefl) - baseRefl) * pow(1.0 - cosTheta, 5.0);
    return baseRefl + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), baseRefl) - baseRefl) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float Pbr_distributionGGX(float dotNH, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float dotNH2    = dotNH * dotNH;
	
    float nom   = a2;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    denom       = rds_pi * denom * denom;
	
    return nom / denom;
}

float Pbr_geometrySchlickGGX_impl(float k, float cosTheta)
{
    float nom   = cosTheta;
    float denom = cosTheta * (1.0 - k) + k;
	
    return nom / denom;
}

float Pbr_geometrySchlickGGX(float cosTheta, float roughness)
{
    float r         = (roughness + 1.0);
    float k         = (r * r) / 8.0;
    return Pbr_geometrySchlickGGX_impl(k, cosTheta);
}

float Pbr_geometrySchlickGGX_ibl(float cosTheta, float roughness)
{
    float r         = (roughness);
    float k         = (r * r) / 2.0;
    return Pbr_geometrySchlickGGX_impl(k, cosTheta);
}

float Pbr_geometrySmith(float dotNV, float dotNL, float roughness)
{
    float ggx2  = Pbr_geometrySchlickGGX(dotNV, roughness);
    float ggx1  = Pbr_geometrySchlickGGX(dotNL, roughness);
	
    return ggx1 * ggx2;
}

float Pbr_geometrySmith_ibl(float dotNV, float dotNL, float roughness)
{
    float ggx2  = Pbr_geometrySchlickGGX_ibl(dotNV, roughness);
    float ggx1  = Pbr_geometrySchlickGGX_ibl(dotNL, roughness);
	
    return ggx1 * ggx2;
}

float3 Pbr_basic_lighting(rds_Surface surface, float3 dirView, float3 posLight, float3 colorLight)
{
    float3 posWs    = surface.posWS;
    float3 normal   = surface.normal;
    float3 albedo   = surface.color;

    //float3 dirView  = normalize(posView - posWs);
    float3 dirLight = normalize(posLight - posWs);
    float3 dirHalf  = normalize(dirView + dirLight);

    float dotHV = max(dot(dirHalf,  dirView),  0.0);
    float dotNH = max(dot(normal,   dirHalf),  0.0);
    float dotNV = max(dot(normal,   dirView),  0.0);
    float dotNL = max(dot(normal,   dirLight), 0.0);

    float3 baseRefl = float3(0.04, 0.04, 0.04);   // non-metallic surfaces
    baseRefl        = lerp(baseRefl, albedo, surface.metallic);

    float3  fresnel     = Pbr_fresnelSchlick(dotHV, baseRefl);                  // dotNV or dotHV
    float   normalDist  = Pbr_distributionGGX(dotNH, surface.roughness);        // speculat highlight, distriGGX give us % of mircofacet align H 
    float   geo         = Pbr_geometrySmith(dotNV, dotNL, surface.roughness);

    float3  spec_numer  = normalDist * geo * fresnel;
    float   spec_denom  = 4.0 * dotNV * dotNL + 0.0001;
    float3  specular    = spec_numer / spec_denom;

    float3 kSpecular = fresnel;
    float3 kDiffuse  = float3(1.0, 1.0, 1.0) - kSpecular;
    kDiffuse        *= 1.0 - surface.metallic;  // metallic surface absorb all diffuse 
    
    float3  lambert = albedo / rds_pi;
    float3  brdf    = kDiffuse * lambert + specular;

    float   attenuation = Lighting_attenuation(posWs, posLight);
    float3  radiance    = colorLight * attenuation;
    float3  o           = brdf * radiance * dotNL;

    //o = fresnel;

    return o;
}

float3 Pbr_indirectDiffuse_ibl(rds_Surface surface, float3 dirView, float3 irradianceEnv, float ao)
{
    float3 baselRefl = float3(0.04, 0.04, 0.04); 
    baselRefl = lerp(baselRefl, surface.color, surface.metallic);

    float3 kS = Pbr_fresnelSchlick(max(dot(surface.normal, dirView), 0.0), baselRefl);
    float3 kD = 1.0 - kS;
    kD *= 1.0 - surface.metallic;	  
    float3 irradiance   = irradianceEnv;
    float3 diffuse      = irradiance * surface.color;
    float3 ambient      = (kD * diffuse) * ao;

    return ambient;
}

float3 Pbr_importanceSampleGGX(float2 sampleSeq, float3 normal, float roughness)
{
    float a = roughness * roughness;
	
    float phi      = 2.0 * rds_pi * sampleSeq.x;
    float cosTheta = sqrt((1.0 - sampleSeq.y) / (1.0 + (a * a - 1.0) * sampleSeq.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 halfDir;
    halfDir.x = cos(phi) * sinTheta;
    halfDir.y = sin(phi) * sinTheta;
    halfDir.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up        = abs(normal.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent   = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);
	
    float3 sampleVec = tangent * halfDir.x + bitangent * halfDir.y + normal * halfDir.z;
    return normalize(sampleVec);
}

float2 Pbr_integrateBrdf(float dotNV, float roughness, uint sampleCount)
{
    float2 o = float2(0.0, 0.0);

    float3 viewDir;
    viewDir.x = sqrt(1.0 - dotNV * dotNV);
    viewDir.y = 0.0;
    viewDir.z = dotNV;

    float3 N = float3(0.0, 0.0, 1.0);

    for(uint i = 0u; i < sampleCount; ++i)
    {
        float2 sampleSeq    = SampleUtil_sequence_hammersley(i, sampleCount);
        float3 halfDir      = Pbr_importanceSampleGGX(sampleSeq, N, roughness);
        float3 L            = normalize(2.0 * dot(viewDir, halfDir) * halfDir - viewDir);

        float dotNL = max(L.z, 0.0);
        float dotNH = max(halfDir.z, 0.0);
        float dotVH = max(dot(viewDir, halfDir), 0.0);

        if(dotNL > 0.0)
        {
            float geo       = Pbr_geometrySmith_ibl(dotNV, dotNL, roughness);
            float geoVis    = (geo * dotVH) / (dotNH * dotNV + 0.00001);
            float fresnel   = pow(1.0 - dotVH, 5.0);

            o.x += (1.0 - fresnel) * geoVis;
            o.y += fresnel * geoVis;
        }
    }
    o /= sampleCount;

    return o;
}

float3 Pbr_indirectLighting(rds_Surface surface, float3 irradianceEnv, float3 prefilteredRefl, float2 brdf, float dotNV)
{
    float3 o = float3(0.0, 0.0, 0.0);

    float3 albedo = surface.color;

    float3 baseRefl = float3(0.04, 0.04, 0.04); 
    baseRefl        = lerp(baseRefl, albedo, surface.metallic);

    float3 diffuse = irradianceEnv * albedo;

    float3 fresnel  = Pbr_fresnelSchlick(dotNV, baseRefl, surface.roughness);
    float3 kS       = fresnel;
    float3 kD       = 1.0 - kS;
    kD             *= 1.0 - surface.metallic;

    float3 specular = prefilteredRefl * (fresnel * brdf.x + brdf.y);
    float3 ambient  = (kD * diffuse + specular) * surface.ambientOcclusion;

    o = ambient;
    //o = fresnel * brdf.x + brdf.y;

    return o; 
}

#endif