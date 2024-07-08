#ifndef __rdsShaderInterop_Material_HLSL__
#define __rdsShaderInterop_Material_HLSL__

#include "rdsShaderInterop_Common.hlsl"

#define RDS_MATERIAL_TEXTURE_baseColor                  tex2D_baseColor
#define RDS_MATERIAL_TEXTURE_normal                     tex2D_normal
#define RDS_MATERIAL_TEXTURE_roughnessMetalness         tex2D_roughnessMetalness
#define RDS_MATERIAL_TEXTURE_emission                   tex2D_emission

#define RDS_MATERIAL_PROPERTY_baseColor                 baseColor
#define RDS_MATERIAL_PROPERTY_metalness                 metalness
#define RDS_MATERIAL_PROPERTY_roughness                 roughness
#define RDS_MATERIAL_PROPERTY_emission                  emission

#define RDS_MATERIAL_PROPERTY_useTexBaseColor           use_tex2D_baseColor
#define RDS_MATERIAL_PROPERTY_useTexNormal              use_tex2D_normal
#define RDS_MATERIAL_PROPERTY_useTexRoughnessMetalness  use_tex2D_roughnessMetalness
#define RDS_MATERIAL_PROPERTY_useTexEmisson             use_tex2D_emission

#endif

