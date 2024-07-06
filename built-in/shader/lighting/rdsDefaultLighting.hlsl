#ifndef __rdsDefaultLighting_HLSL__
#define __rdsDefaultLighting_HLSL__

#include "built-in/shader/common/rdsShader_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop_Material.hlsl"



#if 0
#pragma mark --- default_lighting-Param ---
#endif // 0
#if 1

float4  RDS_MATERIAL_PROPERTY_albedo;
float4  RDS_MATERIAL_PROPERTY_emission;
float   RDS_MATERIAL_PROPERTY_metalness;
float   RDS_MATERIAL_PROPERTY_roughness;

RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Albedo);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Normal);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_RoughnessMetalness);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Emissive);

#endif

#if 0
#pragma mark --- Material-Impl ---
#endif // 0
#if 1

float4      Material_sampleTexture_Albedo(              float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_Albedo,             uv); }
float4      Material_sampleTexture_Normal(              float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_Normal,             uv); }
float4      Material_sampleTexture_RoughnessMetalness(  float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_RoughnessMetalness, uv); }
float4      Material_sampleTexture_Emissive(            float2 uv)      { return RDS_TEXTURE_2D_SAMPLE(RDS_MATERIAL_TEXTURE_Emissive,           uv); }

Texture2D   Material_getTexture_Albedo()                                { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_Albedo); }
Texture2D   Material_getTexture_Normal()                                { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_Normal); }
Texture2D   Material_getTexture_RoughnessMetalness()                    { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_RoughnessMetalness); }
Texture2D   Material_getTexture_Emissive()                              { return RDS_TEXTURE_2D_GET(RDS_MATERIAL_TEXTURE_Emissive); }

float4      Material_getProperty_Albedo()                               { return RDS_MATERIAL_PROPERTY_albedo; }
float4      Material_getProperty_Emission()                             { return RDS_MATERIAL_PROPERTY_emission; }
float       Material_getProperty_Metalness()                            { return RDS_MATERIAL_PROPERTY_metalness; }
float       Material_getProperty_Roughness()                            { return RDS_MATERIAL_PROPERTY_roughness; }

// adding binormal instead of cross(normal, tangent) solve texture seam problem
float3x3 computeTbn(float3 tangent, float3 normal)
{
    tangent                 = normalize(tangent - dot(tangent, normal) * normal);
    float3      binormal    = cross(normal, tangent);
	float3x3    tbn         = float3x3(tangent, binormal, normal);
    tbn = transpose(tbn);
    return tbn;
}

float4 sampleNormalMap(float3x3 tbn, float2 uv)
{
    float4 sampleNormal = Material_sampleTexture_Normal(uv);
    float3 normal       = remap01ToNeg11(sampleNormal.xyz);
    //normal.y = -normal.y;

    // tangent space -> view / world space
    normal = mul(tbn, normal);
    return float4(normal, sampleNormal.a);
    return float4(normalize(normal), sampleNormal.a);
}

Surface Material_makeSurface(float2 uv, float3 pos, float3 normal, float3 tangent)
{
    Surface o = (Surface)0;

    float4 sampleAlbedo         = Material_sampleTexture_Albedo(uv);
    sampleAlbedo.rgb            = lerp(Material_getProperty_Albedo().rgb, sampleAlbedo.rgb, sampleAlbedo.a);

    float2      normalUv        = float2(uv.x, uv.y);
    float3x3    tbn             = computeTbn(tangent, normal);
    float4      sampleNormal    = sampleNormalMap(tbn, normalUv);
    sampleNormal.xyz            = lerp(normal, sampleNormal.rgb, sampleNormal.a);

    float4 roughnessMetalness = Material_sampleTexture_RoughnessMetalness(uv);
    float roughness = lerp(Material_getProperty_Roughness(), roughnessMetalness.r, roughnessMetalness.a);
    float metalness = lerp(Material_getProperty_Metalness(), roughnessMetalness.g, roughnessMetalness.a);

    o.pos                 = pos;
    o.normal              = float3(sampleNormal.xyz);
    o.color               = float4(sampleAlbedo.rgb, 1.0);
    o.roughness           = roughness;
    o.metalness           = metalness;
    
    //o.ambientOcclusion    = 0.2;
	o.ambient             = float4(1.0, 1.0, 1.0, 1.0);
	o.diffuse             = float4(1.0, 1.0, 1.0, 1.0);
	o.specular            = float4(1.0, 1.0, 1.0, 1.0);

    return o;
}

Surface Material_makeSurface(float2 uv, float3 pos, float3 normal)
{
    Surface o = (Surface)0;

    float4 sampleAlbedo = Material_sampleTexture_Albedo(uv);
    sampleAlbedo.rgb    = lerp(Material_getProperty_Albedo().rgb, sampleAlbedo.rgb, sampleAlbedo.a);

    float4 sampleNormal = float4(normal.xyz, 0.0);

    float4 roughnessMetalness = Material_sampleTexture_RoughnessMetalness(uv);
    float roughness = lerp(Material_getProperty_Roughness(), roughnessMetalness.r, roughnessMetalness.a);
    float metalness = lerp(Material_getProperty_Metalness(), roughnessMetalness.g, roughnessMetalness.a);

    o.pos                 = pos;
    o.normal              = float3(sampleNormal.xyz);
    o.color               = float4(sampleAlbedo.rgb, 1.0);
    o.roughness           = roughness;
    o.metalness           = metalness;
    
    //o.ambientOcclusion    = 0.2;
	o.ambient             = float4(1.0, 1.0, 1.0, 1.0);
	o.diffuse             = float4(1.0, 1.0, 1.0, 1.0);
	o.specular            = float4(1.0, 1.0, 1.0, 1.0);

    return o;
}

#endif


#endif