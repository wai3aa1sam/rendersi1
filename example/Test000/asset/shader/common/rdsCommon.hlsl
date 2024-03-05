#ifndef __rdsCommon_HLSL__
#define __rdsCommon_HLSL__

/*
references:
~ https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
~ http://filmicgames.com/archives/75
*/

#if 0
#pragma mark --- rds_global_variable-Impl ---
#endif
#if 1

cbuffer rds_CommonParam
{
    float4x4	rds_matrix_model;
    float4x4	rds_matrix_view;
    float4x4	rds_matrix_proj;
    float4x4	rds_matrix_mvp;

    float3      rds_camera_pos;
};

struct rds_Surface 
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

static const float rds_pi = 3.14159265359;

#endif

#if 0
#pragma mark --- Texture-Impl ---
#endif
#if 1

#define RDS_GET_SAMPLER(NAME)       _rds_ ## NAME ## _sampler
#define RDS_GET_TEXTURE_2D(NAME)    NAME
#define RDS_GET_TEXTURE_CUBE(NAME)  NAME

#define RDS_TEXTURE_2D(NAME) \
Texture2D       RDS_GET_TEXTURE_2D(NAME) 	: register(t1, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(s1, space1) \
// ---

#define RDS_TEXTURE_CUBE(NAME) \
TextureCube     RDS_GET_TEXTURE_CUBE(NAME) 	: register(t1, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(s1, space1) \
// ---

#define RDS_TEXTURE_2D_N(NAME, T, S) \
Texture2D       RDS_GET_TEXTURE_2D(NAME) 	: register(T, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(S, space1) \
// ---

#define RDS_TEXTURE_CUBE_N(NAME, T, S) \
TextureCube     RDS_GET_TEXTURE_CUBE(NAME) 	: register(T, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(S, space1) \
// ---

#define RDS_TEXTURE_2D_1(NAME)      RDS_TEXTURE_2D_N(NAME, t1, s1)
#define RDS_TEXTURE_2D_2(NAME)      RDS_TEXTURE_2D_N(NAME, t2, s2)
#define RDS_TEXTURE_2D_3(NAME)      RDS_TEXTURE_2D_N(NAME, t3, s3)
#define RDS_TEXTURE_2D_4(NAME)      RDS_TEXTURE_2D_N(NAME, t4, s4)
#define RDS_TEXTURE_2D_5(NAME)      RDS_TEXTURE_2D_N(NAME, t5, s5)
#define RDS_TEXTURE_2D_6(NAME)      RDS_TEXTURE_2D_N(NAME, t6, s6)

#define RDS_TEXTURE_CUBE_1(NAME)    RDS_TEXTURE_CUBE_N(NAME, t1, s1)
#define RDS_TEXTURE_CUBE_2(NAME)    RDS_TEXTURE_CUBE_N(NAME, t2, s2)
#define RDS_TEXTURE_CUBE_3(NAME)    RDS_TEXTURE_CUBE_N(NAME, t3, s3)
#define RDS_TEXTURE_CUBE_4(NAME)    RDS_TEXTURE_CUBE_N(NAME, t4, s4)
#define RDS_TEXTURE_CUBE_5(NAME)    RDS_TEXTURE_CUBE_N(NAME, t5, s5)
#define RDS_TEXTURE_CUBE_6(NAME)    RDS_TEXTURE_CUBE_N(NAME, t6, s6)


#define RDS_SAMPLE_TEXTURE_2D(TEX, UV)      RDS_GET_TEXTURE_2D(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)
#define RDS_SAMPLE_TEXTURE_CUBE(TEX, UV)    RDS_GET_TEXTURE_CUBE(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)

#define RDS_TEXTURE_2D_SAMPLE(TEX, UV)                  RDS_GET_TEXTURE_2D(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)
#define RDS_TEXTURE_2D_SAMPLE_LOD(TEX, UV, LOD)         RDS_GET_TEXTURE_2D(TEX).SampleLevel(RDS_GET_SAMPLER(TEX), UV, LOD)
#define RDS_TEXTURE_2D_GET_DIMENSIONS(TEX, OUT_WH)      RDS_GET_TEXTURE_2D(TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

#define RDS_TEXTURE_CUBE_SAMPLE(TEX, UV)                RDS_GET_TEXTURE_CUBE(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)
#define RDS_TEXTURE_CUBE_SAMPLE_LOD(TEX, UV, LOD)       RDS_GET_TEXTURE_CUBE(TEX).SampleLevel(RDS_GET_SAMPLER(TEX), UV, LOD)
#define RDS_TEXTURE_CUBE_GET_DIMENSIONS(TEX, OUT_WH)    RDS_GET_TEXTURE_CUBE(TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

#endif

#if 0
#pragma mark --- ColorUtil-Impl ---
#endif
#if 1

#if 0

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

#else

float3 Color_Linear_to_sRGB(float3 x) { return select(x < 0.0031308, 12.92 * x, 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719); }
float3 Color_sRGB_to_Linear(float3 x) { return select(x < 0.04045, x / 12.92, -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864); }

#endif


#endif


#if 0
#pragma mark --- LightingUtil-Impl ---
#endif
#if 1

float Lighting_attenuation(float3 posWs, float3 posLight)
{
    return 1.0;
}

#endif

#if 0
#pragma mark --- PostProcessingUtil-Impl ---
#endif
#if 1

float3 PostProc_gammaEncoding(float3 v) { return pow(v, 1.0 / 2.2); }
float3 PostProc_gammaDecoding(float3 v) { return pow(v, 2.2); }

#if 0
#pragma mark --- ToneMappingUtil-Impl ---
#endif
#if 1

float3 ToneMapping_reinhard(float3 v) { return v / (v + float3(1.0, 1.0, 1.0)); }

// From http://filmicgames.com/archives/75
float3 ToneMapping_uncharted2(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}


#endif

#endif

#if 0
#pragma mark --- SampleUtil-Impl ---
#endif
#if 1

float SampleUtil_sequence_vdc(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 SampleUtil_sequence_hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), SampleUtil_sequence_vdc(i));
}  

#endif

float4x4 inverse(float4x4 m) 
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}


























#endif

