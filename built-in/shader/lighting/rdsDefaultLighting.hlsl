#ifndef __rdsDefaultLighting_HLSL__
#define __rdsDefaultLighting_HLSL__

#include "built-in/shader/common/rdsShader_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop_Material.hlsl"



#if 0
#pragma mark --- default_lighting-Param ---
#endif // 0
#if 1

RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Albedo);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Normal);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_RoughnessMetalness);
RDS_TEXTURE_2D(RDS_MATERIAL_TEXTURE_Emissive);

float4  RDS_MATERIAL_PROPERTY_albedo;
float4  RDS_MATERIAL_PROPERTY_emission;
float   RDS_MATERIAL_PROPERTY_metalness;
float   RDS_MATERIAL_PROPERTY_roughness;

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

#endif


#endif