


#if 0
#pragma mark --- rds_global_variable-Impl ---
#endif
#if 1

cbuffer rds_matrix
{
    float4x4	rds_matrix_model;
    float4x4	rds_matrix_view;
    float4x4	rds_matrix_proj;
    float4x4	rds_matrix_mvp;

    float3      rds_camera_pos;
};

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

#endif

#if 0
#pragma mark --- ColorUtil-Impl ---
#endif
#if 1

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

#endif