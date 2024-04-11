#ifndef __rdsShaderInterop_HLSL__
#define __rdsShaderInterop_HLSL__

#define RDS_PADDING float RDS_CONCAT(__padding, __COUNTER__)

struct DrawParam
{
    float3   camera_pos;
    RDS_PADDING;    
    float4x4 matrix_view;
    float4x4 matrix_proj;
    float4x4 matrix_view_inv;
    float4x4 matrix_proj_inv;

    float2 resolution;
    float2 delta_time;
};

struct PerObjectParam
{
    uint id;
};

struct ObjectTransform
{
    float4x4 matrix_model;
    float4x4 matrix_mvp;
};

struct Surface 
{
	float3 posWS;
	float3 normal;	
	float3 color;
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float  shininess;

    float  roughness;
    float  metallic;

    float  ambientOcclusion;
};

#endif

