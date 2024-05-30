#ifndef __rdsShaderInterop_Material_HLSL__
#define __rdsShaderInterop_Material_HLSL__

#include "rdsShaderInterop_Common.hlsl"

#define RDS_MATERIAL_TEXTURE_Albedo                 tex2D_albedo
#define RDS_MATERIAL_TEXTURE_Normal                 tex2D_normal
#define RDS_MATERIAL_TEXTURE_RoughnessMetalness     tex2D_roughnessMetalness
#define RDS_MATERIAL_TEXTURE_Emissive               tex2D_emissive

#define RDS_MATERIAL_PROPERTY_albedo                albedo
#define RDS_MATERIAL_PROPERTY_emission              emission
#define RDS_MATERIAL_PROPERTY_metalness             metalness
#define RDS_MATERIAL_PROPERTY_roughness             roughness

#endif

