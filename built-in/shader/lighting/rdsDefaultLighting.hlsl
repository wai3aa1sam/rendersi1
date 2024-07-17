#ifndef __rdsDefaultLighting_HLSL__
#define __rdsDefaultLighting_HLSL__

#include "built-in/shader/common/rdsShader_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop_Material.hlsl"
#include "built-in/shader/shadow/cascaded_shadow_maps/rdsCascadedShadowMaps.hlsl"
#include "built-in/shader/post_processing/rdsPostProcessing.hlsl"
#include "built-in/shader/post_processing/rdsToneMapping.hlsl"

#if 0
#pragma mark --- default_lighting-Param ---
#endif // 0
#if 1

float4  RDS_MATERIAL_PROPERTY_baseColor;
float   RDS_MATERIAL_PROPERTY_metalness;
float   RDS_MATERIAL_PROPERTY_roughness;
float4  RDS_MATERIAL_PROPERTY_emission;

uint    RDS_MATERIAL_PROPERTY_useTexBaseColor;
uint    RDS_MATERIAL_PROPERTY_useTexNormal;
uint    RDS_MATERIAL_PROPERTY_useTexRoughnessMetalness;
uint    RDS_MATERIAL_PROPERTY_useTexEmisson;

RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_baseColor);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_normal);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_roughnessMetalness);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_emission);

#endif

#if 0
#pragma mark --- Material-Impl ---
#endif // 0
#if 1

float4      Material_sampleTexture_BaseColor(           float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_baseColor,             uv); }
float4      Material_sampleTexture_Normal(              float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_normal,             uv); }
float4      Material_sampleTexture_RoughnessMetalness(  float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_roughnessMetalness, uv); }
float4      Material_sampleTexture_Emission(            float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_emission,           uv); }

Texture2D   Material_getTexture_BaseColor()                             { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_baseColor); }
Texture2D   Material_getTexture_Normal()                                { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_normal); }
Texture2D   Material_getTexture_RoughnessMetalness()                    { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_roughnessMetalness); }
Texture2D   Material_getTexture_Emission()                              { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_emission); }

float       Material_unpackRoughness(float4 roughnessMetalness)         { return roughnessMetalness.r; }
float       Material_unpackMetalness(float4 roughnessMetalness)         { return roughnessMetalness.g; }

float4      Material_getProperty_BaseColor()                            { return RDS_MATERIAL_PROPERTY_baseColor; }
float4      Material_getProperty_Emission()                             { return RDS_MATERIAL_PROPERTY_emission; }
float       Material_getProperty_Metalness()                            { return RDS_MATERIAL_PROPERTY_metalness; }
float       Material_getProperty_Roughness()                            { return RDS_MATERIAL_PROPERTY_roughness; }

bool        Material_getProperty_useTexBaseColor()                      { return RDS_MATERIAL_PROPERTY_useTexBaseColor; }
bool        Material_getProperty_useTexNormal()                         { return RDS_MATERIAL_PROPERTY_useTexNormal; }
bool        Material_getProperty_useTexRoughnessMetalness()             { return RDS_MATERIAL_PROPERTY_useTexRoughnessMetalness; }
bool        Material_getProperty_useTexEmissive()                       { return RDS_MATERIAL_PROPERTY_useTexEmisson; }

float4      Material_packRoughnessMetalness(float roughness, float metalness)         
{ 
    float4 o;
    o.x = roughness;
    o.y = metalness;
    return o; 
}

// adding binormal instead of cross(normal, tangent) solve texture seam problem in assimp
float3x3 computeTbn(float3 tangent, float3 normal)
{
    tangent                 = normalize(tangent - dot(tangent, normal) * normal);
    float3      binormal    = cross(normal, tangent);
	float3x3    tbn         = float3x3(tangent, binormal, normal);
    tbn = transpose(tbn);
    return tbn;
}

float4 sampleNormalMap(float3x3 tbn, float4 sampleNormal)
{
    float3 normal       = remap01ToNeg11(sampleNormal.xyz);
    //normal.y = -normal.y;

    // tangent space -> view / world space
    normal = mul(tbn, normal);
    return float4(normal, sampleNormal.a);
    return float4(normalize(normal), sampleNormal.a);
}

float4 Material_sampleBaseColor(float2 uv)
{
    float4 sampleBaseColor          = Material_sampleTexture_BaseColor(uv);
    float  useTexBaseColor          = Material_getProperty_useTexBaseColor();
    sampleBaseColor                 = lerp(Material_getProperty_BaseColor(), sampleBaseColor, useTexBaseColor);
    
    float4 o = sampleBaseColor;
    //o = Material_getProperty_BaseColor();
    return o;
}

float3 Material_sampleNormal(float2 uv, float3 normal, float3 tangent)
{
    float3x3    tbn                 = computeTbn(tangent, normal);
    float4      sampleNormal        = sampleNormalMap(tbn, Material_sampleTexture_Normal(uv));
    float       useTexNormal        = Material_getProperty_useTexNormal();
    sampleNormal.xyz                = lerp(normal, sampleNormal.rgb, useTexNormal);
    sampleNormal.xyz                = sampleNormal.xyz;
    //sampleNormal.xyz                = normalize(sampleNormal.xyz);

    float3 o = sampleNormal.xyz;
    return o;
}

float4 Material_sampleRoughnessMetalness(float2 uv)
{
    float4 roughnessMetalness       = Material_sampleTexture_RoughnessMetalness(uv);
    float  useTexRoughnessMetalness = Material_getProperty_useTexRoughnessMetalness();
    float  roughness                = lerp(Material_getProperty_Roughness(), Material_unpackRoughness(roughnessMetalness), useTexRoughnessMetalness);
    float  metalness                = lerp(Material_getProperty_Metalness(), Material_unpackMetalness(roughnessMetalness), useTexRoughnessMetalness);
    
    float4 o = Material_packRoughnessMetalness(roughness, metalness);
    return o;
}

float4 Material_sampleEmission(float2 uv)
{
    float4 sampleEmission           = Material_sampleTexture_Emission(uv);
    float  useTexEmission           = Material_getProperty_useTexBaseColor();
    sampleEmission                  = lerp(Material_getProperty_Emission(), sampleEmission, useTexEmission);

    float4 o = sampleEmission;
    return o;
}

Surface Material_makeSurface(float2 uv, float3 pos, float3 normal_, float3 tangent)
{
    Surface o = (Surface)0;

    float4 baseColor            = Material_sampleBaseColor(uv);
    float3 normal               = Material_sampleNormal(uv, normal_, tangent);
    float4 roughnessMetalness   = Material_sampleRoughnessMetalness(uv);
    float4 emission             = Material_sampleEmission(uv);

    o.pos                 = pos;
    o.normal              = normal;
    o.baseColor           = baseColor;
    o.roughness           = Material_unpackRoughness(roughnessMetalness);
    o.metalness           = Material_unpackMetalness(roughnessMetalness);
    o.emission            = emission;
    
    o.ambientOcclusion    = 1.0;
	o.ambient             = float4(1.0, 1.0, 1.0, 1.0);
	o.diffuse             = float4(1.0, 1.0, 1.0, 1.0);
	o.specular            = float4(1.0, 1.0, 1.0, 1.0);

    return o;
}

Surface Material_makeSurface(float2 uv, float3 pos, float3 normal)
{
    Surface o = (Surface)0;
    o           = Material_makeSurface(uv, pos, normal, float3(0.0, 0.0, 0.0));
    o.normal    = normal;
    return o;
}

Surface Material_makeSurface(float3 pos, float3 normal, float4 roughnessMetalness, float4 baseColor, float4 emission)
{
    Surface o = (Surface)0;

    o.pos                 = pos;
    o.normal              = normal;
    o.baseColor           = baseColor;
    o.roughness           = Material_unpackRoughness(roughnessMetalness);
    o.metalness           = Material_unpackMetalness(roughnessMetalness);
    o.emission            = emission;
    
    o.ambientOcclusion    = 1.0;
	o.ambient             = float4(1.0, 1.0, 1.0, 1.0);
	o.diffuse             = float4(1.0, 1.0, 1.0, 1.0);
	o.specular            = float4(1.0, 1.0, 1.0, 1.0);

    return o;
}

#endif

#endif