#ifndef __rdsShader_Common_HLSL__
#define __rdsShader_Common_HLSL__

/*
references:
~ https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
~ http://filmicgames.com/archives/75
*/

#include "rdsMarco_Common.hlsl"
#include "rdsConstant_Common.hlsl"
#include "built-in/shader/rdsShaderInterop.hlsl"

#if 0
#pragma mark --- rdsCommonDefine-Impl ---
#endif
#if 1

// typedef uint64_t u64; // this is ok
#define u64 uint64_t

#define UINT_MAX 0xffffffff

#define RDS_BINDLESS 1
#if RDS_BINDLESS

    /* 
    --- define space for bindless type
    */
    #define RDS_BUFFER_SPACE 	space0

    #define RDS_TEXURE_SPACE 	space1
    #define RDS_SAMPLER_SPACE 	RDS_TEXURE_SPACE

    #define RDS_IMAGE_SPACE 	space2

    //#define RDS_CONSTANT_BUFFER_SPACE spacex		// define when compile

    //#define RDS_K_SAMPLER_COUNT 1		// set in compiler
    #define RDS_TEXTURE_BINDING RDS_CONCAT(t, RDS_K_SAMPLER_COUNT)

    /* 
    --- define bindless type
    */
    SamplerState rds_sampler : register(s13, RDS_CONSTANT_BUFFER_SPACE);   // immutable sampler behave differently, so use this temp solution

    // ByteAddressBuffer
    ByteAddressBuffer 	    rds_bufferTable		    [] 						: register(t0, RDS_BUFFER_SPACE);
    RWByteAddressBuffer 	rds_rwBufferTable	    [] 						: register(u0, RDS_BUFFER_SPACE);

    SamplerState    	    rds_samplerTable	    [RDS_K_SAMPLER_COUNT] 	: register(s0, 					RDS_SAMPLER_SPACE);
    Texture2D 			    rds_texture2DTable	    []  					: register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    TextureCube 		    rds_textureCubeTable    []  					: register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    
    // Texture<T>
    #define RDS_TEXTURE_T_NAME(ND, T) RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(rds_texture, ND), Table), RDS_CONCAT(_, T))
    #define RDS_TEXTURE_T_DECL(ND, T)      RDS_CONCAT(Texture, ND)<T> RDS_TEXTURE_T_NAME(ND, T)[] : register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE)
    
    #define RDS_TEXTURE_1D_T_DECL(T, ...)    RDS_TEXTURE_T_DECL(1D, T);
    #define RDS_TEXTURE_2D_T_DECL(T, ...)    RDS_TEXTURE_T_DECL(2D, T);
    #define RDS_TEXTURE_3D_T_DECL(T, ...)    RDS_TEXTURE_T_DECL(3D, T);
    #define RDS_TEXTURE_CUBE_T_DECL(T, ...)  RDS_TEXTURE_T_DECL(Cube, T);

    #define RDS_TEXTURE_DECL() \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_1D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_2D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_3D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_CUBE_T_DECL) \
    // ---
    RDS_TEXTURE_DECL();

    // RWTexture<T>
    #define RDS_IMAGE_T_NAME(ND, T) RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(rds_image, ND), Table), RDS_CONCAT(_, T))
    #define RDS_IMAGE_T_DECL(ND, T)      RDS_CONCAT(RWTexture, ND)<T> RDS_IMAGE_T_NAME(ND, T)[] : register(u0, RDS_IMAGE_SPACE)
    
    #define RDS_IMAGE_1D_T_DECL(T, ...)      RDS_IMAGE_T_DECL(1D, T);
    #define RDS_IMAGE_2D_T_DECL(T, ...)      RDS_IMAGE_T_DECL(2D, T);
    #define RDS_IMAGE_3D_T_DECL(T, ...)      RDS_IMAGE_T_DECL(3D, T);

    #define RDS_IMAGE_DECL() \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_1D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_2D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_3D_T_DECL) \
    // ---
    RDS_IMAGE_DECL();
    
    /* 
    --- define Texture
    */

    #define RDS_TEXTURE_ST_SUFFIX _ST_

    #define RDS_TEXTURE_NAME(NAME) NAME
    #define RDS_TEXTURE_ST(NAME)   RDS_CONCAT(NAME, RDS_TEXTURE_ST_SUFFIX)
    #define RDS_SAMPLER_NAME(NAME) RDS_CONCAT(RDS_CONCAT(_rds_, NAME), _sampler)
    #define RDS_TEXTURE_2D(NAME)    uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME); float4 RDS_TEXTURE_ST(NAME)
    #define RDS_TEXTURE_CUBE(NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)

    #define RDS_SAMPLER_GET(NAME)       rds_sampler

    #define RDS_TEXTURE_UV2(NAME, UV) float2(UV.xy * RDS_TEXTURE_ST(NAME).xy + RDS_TEXTURE_ST(NAME).zw)

    //#define RDS_SAMPLER_GET(NAME)       rds_samplerTable[NonUniformResourceIndex(RDS_SAMPLER_NAME(NAME))]
    #define RDS_TEXTURE_2D_GET(NAME)    rds_texture2DTable[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_CUBE_GET(NAME)  rds_textureCubeTable[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]

    #define RDS_TEXTURE_2D_SAMPLE(TEX, UV)                  RDS_TEXTURE_2D_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, UV))
    #define RDS_TEXTURE_2D_SAMPLE_LOD(TEX, UV, LOD)         RDS_TEXTURE_2D_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, UV), LOD)
    #define RDS_TEXTURE_2D_GET_DIMENSIONS(TEX, OUT_WH)      RDS_TEXTURE_2D_GET(TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

    #define RDS_TEXTURE_CUBE_SAMPLE(TEX, UV)                RDS_TEXTURE_CUBE_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), UV)
    #define RDS_TEXTURE_CUBE_SAMPLE_LOD(TEX, UV, LOD)       RDS_TEXTURE_CUBE_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), UV, LOD)
    #define RDS_TEXTURE_CUBE_GET_DIMENSIONS(TEX, OUT_WH)    RDS_TEXTURE_CUBE_GET(TEX).GetDimensions(OUT_WH.x,   OUT_WH.y)

    #define RDS_TEXTURE_2D_T(T, NAME) RDS_TEXTURE_2D(NAME)

    #define RDS_TEXTURE_1D_T_GET(  T, NAME) RDS_TEXTURE_T_NAME(1D,   T)[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_2D_T_GET(  T, NAME) RDS_TEXTURE_T_NAME(2D,   T)[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_3D_T_GET(  T, NAME) RDS_TEXTURE_T_NAME(3D,   T)[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_CUBE_T_GET(T, NAME) RDS_TEXTURE_T_NAME(Cube, T)[NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]

    #define RDS_TEXTURE_2D_T_SAMPLE(T, TEX, UV) RDS_TEXTURE_2D_T_GET(T, TEX).Sample(     RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, UV))

    /* 
    --- define ConstantBuffer Util
    */
    #define RDS_CONSTANT_BUFFER_BINDING(N) RDS_CONCAT(b, N)
    #define RDS_CONSTANT_BUFFER(TYPE, NAME, BINDING) ConstantBuffer<TYPE> NAME : register(RDS_CONSTANT_BUFFER_BINDING(BINDING), RDS_CONSTANT_BUFFER_SPACE)
    
    /* 
    --- define Buffer
    */
    #define RDS_BUFFER(TYPE, NAME)                              uint NAME
    #define RDS_BUFFER_GET(NAME)                                rds_bufferTable[NonUniformResourceIndex(NAME)]
    #define RDS_BUFFER_LOAD_I(TYPE, NAME, IDX)                  RDS_BUFFER_GET(NAME).Load<TYPE>(sizeof(TYPE) * (IDX))
    #define RDS_BUFFER_LOAD(TYPE, NAME)                         RDS_BUFFER_LOAD_I(TYPE, NAME, 0)

    #define RDS_RW_BUFFER(TYPE, NAME)                           uint NAME
    #define RDS_RW_BUFFER_GET(NAME)                             rds_rwBufferTable[NonUniformResourceIndex(NAME)]
    #define RDS_RW_BUFFER_LOAD_I(TYPE, NAME, IDX)               RDS_RW_BUFFER_GET(NAME).Load<TYPE>( sizeof(TYPE) * (IDX))
    #define RDS_RW_BUFFER_STORE_I(TYPE, NAME, IDX, VALUE)       RDS_RW_BUFFER_GET(NAME).Store<TYPE>(sizeof(TYPE) * (IDX), VALUE)
    #define RDS_RW_BUFFER_LOAD(TYPE, NAME)                      RDS_RW_BUFFER_LOAD_I(TYPE, NAME, 0)
    #define RDS_RW_BUFFER_STORE(TYPE, NAME, VALUE)              RDS_RW_BUFFER_STORE_I(TYPE, NAME, 0, VALUE)

    #define RDS_RW_BUFFER_ATM_ADD_I(TYPE, NAME, IDX, VAL, OUT) RDS_RW_BUFFER_GET(NAME).InterlockedAdd(sizeof(TYPE) * (IDX), VAL, OUT)

    /* 
    --- define Image
    */
    #define RDS_IMAGE_2D(        TYPE, NAME)                    uint NAME
    #define RDS_IMAGE_2D_GET(    TYPE, NAME)                    RDS_IMAGE_T_NAME(2D, TYPE)[NonUniformResourceIndex(NAME)]
    // #define RDS_IMAGE_2D_LOAD_I( TYPE, NAME, IDX)               RDS_IMAGE_2D_GET(TYPE, NAME)[IDX]
    // #define RDS_IMAGE_2D_STORE_I(TYPE, NAME, IDX, VALUE)        RDS_IMAGE_2D_GET(TYPE, NAME)[IDX] = VALUE
    // #define RDS_IMAGE_2D_LOAD(   TYPE, NAME)                    RDS_IMAGE_2D_LOAD_I(TYPE, NAME, 0)
    // #define RDS_IMAGE_2D_STORE(  TYPE, NAME, VALUE)             RDS_IMAGE_2D_STORE_I(TYPE, NAME, 0, VALUE)

#else
    #error "only support for bindless"
#endif

#endif


#if 0
#pragma mark --- rds_global_variable-Impl ---
#endif
#if 1

[[vk::push_constant]] ConstantBuffer<PerObjectParam> rds_perObjectParam;

RDS_BUFFER(DrawParam,           rds_drawParams);
RDS_BUFFER(ObjectTransform,     rds_objTransforms);
RDS_BUFFER(Light,               rds_lights);
uint                            rds_nLights;

#define RDS_DRAW_PARAM_GET(IDX)     RDS_BUFFER_LOAD_I(DrawParam,           rds_drawParams,      IDX)
#define RDS_OBJ_TRANSFORM_GET()     RDS_BUFFER_LOAD_I(ObjectTransform,     rds_objTransforms,   rds_perObjectParam.id)

#define RDS_MATRIX_VIEW         rds_DrawParam_get().matrix_view
#define RDS_MATRIX_PROJ         rds_DrawParam_get().matrix_proj
#define RDS_MATRIX_VP           rds_DrawParam_get().matrix_vp
#define RDS_MATRIX_VIEW_INV     rds_DrawParam_get().matrix_view_inv
#define RDS_MATRIX_PROJ_INV     rds_DrawParam_get().matrix_proj_inv

#define RDS_MATRIX_MODEL        rds_ObjectTransform_get().matrix_model
#define RDS_MATRIX_MVP          rds_ObjectTransform_get().matrix_mvp

ObjectTransform rds_ObjectTransform_get()
{
    return RDS_OBJ_TRANSFORM_GET();
}

DrawParam rds_DrawParam_get(uint idx = 0)
{
    return RDS_DRAW_PARAM_GET(idx);
}

Light rds_Lights_get(uint idx)
{
    return RDS_BUFFER_LOAD_I(Light, rds_lights, idx);
}

#endif

#if 0
#pragma mark --- Util-Impl ---
#endif
#if 1

#define RDS_DECLARE_PT_SIZE(var) 	[[vk::builtin("PointSize")]] float var : PSIZE
#define RDS_SET_PT_SIZE(var, v) 	var = v

#endif

#if 0
#pragma mark --- Texture-Impl ---
#endif
#if 1

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

#if 0
#pragma mark --- SpaceTransform-Impl ---
#endif
#if 1

float4 SpaceTransform_clipToView(float4 clip, DrawParam drawParam)
{
    float4 view = mul(drawParam.matrix_proj_inv, clip);
    view = view / view.w;
    return view;
}

float4 SpaceTransform_clipToView(float4 clip)
{
    DrawParam drawParam = rds_DrawParam_get();
    return SpaceTransform_clipToView(clip, drawParam);
}

float4 SpaceTransform_screenToView(float4 screen, DrawParam drawParam)
{
    float2 uv = screen.xy / drawParam.resolution;

    float4 clip = float4(float2(uv.x, 1.0 - uv.y) * 2.0 - 1.0, screen.z, screen.w);
    return SpaceTransform_clipToView(clip, drawParam);
}

float4 SpaceTransform_screenToView(float4 screen)
{
    DrawParam drawParam = rds_DrawParam_get();
    return SpaceTransform_screenToView(screen, drawParam);
}

#endif



























#endif

