#ifndef __rdsShaderInterop_Common_HLSL__
#define __rdsShaderInterop_Common_HLSL__

#if RDS_LANG_CPP
    #define RDS_OUT(T) T&
#else
    #define RDS_OUT(T) T
#endif

#define RDS_PADDING      float  RDS_CONCAT(__padding, __COUNTER__)
#define RDS_PADDING_32x2 float2 RDS_CONCAT(__padding, __COUNTER__)
#define RDS_PADDING_32x3 float3 RDS_CONCAT(__padding, __COUNTER__)

#define RDS_LIGHT_TYPE_NONE          0
#define RDS_LIGHT_TYPE_POINT         1
#define RDS_LIGHT_TYPE_SPOT          2
#define RDS_LIGHT_TYPE_DIRECTIONAL   3
#define RDS_LIGHT_TYPE_AREA          4

struct DrawParam
{
    float3   camera_pos;
    RDS_PADDING;    
    
    float camera_near;
    float camera_far;
    RDS_PADDING;    
    RDS_PADDING;    
    
    float4x4 matrix_view;
    float4x4 matrix_proj;
    float4x4 matrix_vp;
    float4x4 matrix_view_inv;
    float4x4 matrix_proj_inv;
    float4x4 matrix_vp_inv;

    float2 resolution;
    float2 delta_time;
};

struct PerObjectParam
{
    uint id;
    //uint drawParamIdx;        // keep it to 32bits is better
};

struct ObjectTransform
{
    float4x4 matrix_model;
    float4x4 matrix_mvp;
    float4x4 matrix_mv;
    float4x4 matrix_m_inv_t;
    float4x4 matrix_mv_inv_t;
};

struct Surface 
{
    float3 pos;
	float3 normal;

	float4 baseColor;
	float4 ambient;
	float4 diffuse;
	float4 specular;

    float   roughness;
    float   metalness;
    float4  emission;
    float   ambientOcclusion;
};

struct Light
{
    float4  positionWs;
    float4  directionWs;
    
	float4	positionVs;
	float4	directionVs;
    
    float4  color;

	uint 	type;
	float   range;
    float   intensity;
    RDS_PADDING;

    float4  param;
};

struct LightingResult
{
    float4 diffuse;
    float4 specular;
};

#endif

