#ifndef __rdsShader_Common_HLSL__
#define __rdsShader_Common_HLSL__

/*
references:
~ https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
~ http://filmicgames.com/archives/75
~ https://notargs.hateblo.jp/entry/invert_matrix
*/

#include "rdsMarco_Common.hlsl"
#include "rdsConstant_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop.hlsl"

#if 0
#pragma mark --- rdsCommonDefine-Impl ---
#endif
#if 1

#if RDS_SHADER_USE_BINDLESS

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

    // ByteAddressBuffer
    ByteAddressBuffer 	    rds_bufferTable[] 						    : register(t0, RDS_BUFFER_SPACE);
    RWByteAddressBuffer 	rds_rwBufferTable[] 						: register(u0, RDS_BUFFER_SPACE);

    SamplerState    	    rds_samplerTable[RDS_K_SAMPLER_COUNT] 	    : register(s0, 					RDS_SAMPLER_SPACE);
    Texture1D 			    rds_texture1DTable[]  					    : register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    Texture2D 			    rds_texture2DTable[]  					    : register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    Texture3D 			    rds_texture3DTable[]  					    : register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    TextureCube 		    rds_textureCubeTable[]  					: register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    Texture2DArray 		    rds_texture2DArrayTable[]  					: register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE);
    
    // Texture<T>
    #define RDS_TEXTURE_TABLE_T_NAME(ND, T) RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(rds_texture, ND), Table), RDS_CONCAT(_, T))
    #define RDS_TEXTURE_TABLE_T_DECL(ND, T) RDS_CONCAT(Texture, ND)<T> RDS_TEXTURE_TABLE_T_NAME(ND, T)[] : register(RDS_TEXTURE_BINDING, RDS_TEXURE_SPACE)
    
    #define RDS_TEXTURE_TABLE_1D_T_DECL(            T, ...)     RDS_TEXTURE_TABLE_T_DECL(1D,            T);
    #define RDS_TEXTURE_TABLE_2D_T_DECL(            T, ...)     RDS_TEXTURE_TABLE_T_DECL(2D,            T);
    #define RDS_TEXTURE_TABLE_3D_T_DECL(            T, ...)     RDS_TEXTURE_TABLE_T_DECL(3D,            T);
    #define RDS_TEXTURE_TABLE_CUBE_T_DECL(          T, ...)     RDS_TEXTURE_TABLE_T_DECL(Cube,          T);
    #define RDS_TEXTURE_TABLE_2D_ARRAY_T_DECL(      T, ...)     RDS_TEXTURE_TABLE_T_DECL(2DArray,       T);

    #define RDS_TEXTURE_DECL() \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_TABLE_1D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_TABLE_2D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_TABLE_3D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_TABLE_CUBE_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_TEXTURE_TABLE_2D_ARRAY_T_DECL) \
    // ---
    RDS_TEXTURE_DECL();

    // RWTexture<T>
    #define RDS_IMAGE_TABLE_T_NAME(ND, T)          RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(rds_image, ND), Table), RDS_CONCAT(_, T))
    #define RDS_IMAGE_TABLE_T_DECL(ND, T)          RDS_CONCAT(RWTexture, ND)<T> RDS_IMAGE_TABLE_T_NAME(ND, T)[] : register(u0, RDS_IMAGE_SPACE)
    
    #define RDS_IMAGE_TABLE_1D_T_DECL(          T, ...)      RDS_IMAGE_TABLE_T_DECL(1D, T);
    #define RDS_IMAGE_TABLE_2D_T_DECL(          T, ...)      RDS_IMAGE_TABLE_T_DECL(2D, T);
    #define RDS_IMAGE_TABLE_3D_T_DECL(          T, ...)      RDS_IMAGE_TABLE_T_DECL(3D, T);
    #define RDS_IMAGE_TABLE_2D_ARRAY_T_DECL(    T, ...)      RDS_IMAGE_TABLE_T_DECL(2DArray, T);

    #define RDS_IMAGE_DECL() \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_TABLE_1D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_TABLE_2D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_TABLE_3D_T_DECL) \
        RDS_TYPE_ITER_LIST(RDS_IMAGE_TABLE_2D_ARRAY_T_DECL) \
    // ---
    RDS_IMAGE_DECL();
    
    /* 
    --- define Texture
    */

    #define RDS_SAMPLER_NAME(NAME)      RDS_CONCAT(NAME, _sampler)
    #if 0
    #define RDS_SAMPLER_GET(NAME)       rds_samplerTable[NonUniformResourceIndex(RDS_SAMPLER_NAME(NAME))]
    #else
    // immutable sampler behave differently, so use this temp solution
    /*
    * since immutable sampler does not work as expected
    * , it only support with numeric constant eg. rds_samplerTable[NonUniformResourceIndex(1))]
    * maybe it is my fault, btw we change the strategy.
    * currently using SamplerState array to replace the immutable sampler,
    * the frontend part should be compatible with immutable sampler,
    * However, this method is not portable for gpu that only has minimum bind slot count (16)
    */
    SamplerState rds_samplers[RDS_K_SAMPLER_COUNT] : register(s17, RDS_CONSTANT_BUFFER_SPACE);
    #define RDS_SAMPLER_GET(NAME)       rds_samplers[RDS_SAMPLER_NAME(NAME)]
    #endif

    #define RDS_TEXTURE_ST_SUFFIX _ST_
    #define RDS_TEXTURE_UV2(NAME, UV)          float2(UV.xy * RDS_TEXTURE_ST(NAME).xy + RDS_TEXTURE_ST(NAME).zw)

    #define RDS_TEXTURE_NAME(           NAME)  NAME
    #define RDS_TEXTURE_ST(             NAME)  RDS_CONCAT(NAME, RDS_TEXTURE_ST_SUFFIX)
    #define RDS_TEXTURE_1D(             NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_2D(             NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME); float4 RDS_TEXTURE_ST(NAME)
    #define RDS_TEXTURE_3D(             NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_CUBE(           NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_2D_ARRAY(       NAME)  uint RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)

    #define RDS_TEXTURE_1D_GET(             NAME) rds_texture1DTable[           NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_2D_GET(             NAME) rds_texture2DTable[           NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_3D_GET(             NAME) rds_texture3DTable[           NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_CUBE_GET(           NAME) rds_textureCubeTable[         NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]
    #define RDS_TEXTURE_2D_ARRAY_GET(       NAME) rds_texture2DArrayTable[      NonUniformResourceIndex(RDS_TEXTURE_NAME(NAME))]

    #define RDS_TEXTURE_1D_SAMPLE(                      TEX, UV)        RDS_TEXTURE_1D_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), (UV))
    #define RDS_TEXTURE_1D_SAMPLE_LOD(                  TEX, UV, LOD)   RDS_TEXTURE_1D_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_1D_GET_DIMENSIONS(              TEX, OUT_WH)    RDS_TEXTURE_1D_GET(TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

    #define RDS_TEXTURE_2D_SAMPLE(                      TEX, UV)        RDS_TEXTURE_2D_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, (UV)))
    #define RDS_TEXTURE_2D_SAMPLE_LOD(                  TEX, UV, LOD)   RDS_TEXTURE_2D_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, (UV)), (LOD))
    #define RDS_TEXTURE_2D_GET_DIMENSIONS(              TEX, OUT_WH)    RDS_TEXTURE_2D_GET(TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

    #define RDS_TEXTURE_3D_SAMPLE(                      TEX, UV)        RDS_TEXTURE_3D_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), (UV))
    #define RDS_TEXTURE_3D_SAMPLE_LOD(                  TEX, UV, LOD)   RDS_TEXTURE_3D_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_3D_GET_DIMENSIONS(              TEX, OUT_WHD)   RDS_TEXTURE_3D_GET(TEX).GetDimensions(OUT_WHD.x, OUT_WHD.y, OUT_WHD.z)

    #define RDS_TEXTURE_CUBE_SAMPLE(                    TEX, UV)        RDS_TEXTURE_CUBE_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), (UV))
    #define RDS_TEXTURE_CUBE_SAMPLE_LOD(                TEX, UV, LOD)   RDS_TEXTURE_CUBE_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_CUBE_GET_DIMENSIONS(            TEX, OUT_WH)    RDS_TEXTURE_CUBE_GET(TEX).GetDimensions(OUT_WH.x,   OUT_WH.y)

    #define RDS_TEXTURE_2D_ARRAY_SAMPLE(                TEX, UV)        RDS_TEXTURE_2D_ARRAY_GET(TEX).Sample(     RDS_SAMPLER_GET(TEX), (UV))
    #define RDS_TEXTURE_2D_ARRAY_SAMPLE_LOD(            TEX, UV, LOD)   RDS_TEXTURE_2D_ARRAY_GET(TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_2D_ARRAY_GET_DIMENSIONS(        TEX, OUT_WH)    RDS_TEXTURE_2D_ARRAY_GET(TEX).GetDimensions(OUT_WH.x,   OUT_WH.y)

    #define RDS_TEXTURE_T_NAME(         T, NAME) (NAME)
    #define RDS_TEXTURE_1D_T(           T, NAME) uint RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_2D_T(           T, NAME) uint RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME); float4 RDS_TEXTURE_ST(NAME)
    #define RDS_TEXTURE_3D_T(           T, NAME) uint RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_CUBE_T(         T, NAME) uint RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
    #define RDS_TEXTURE_2D_ARRAY_T(     T, NAME) uint RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)

    #define RDS_TEXTURE_1D_T_GET(           T, NAME) RDS_TEXTURE_TABLE_T_NAME(1D,           T)[NonUniformResourceIndex(RDS_TEXTURE_T_NAME(T, NAME))]
    #define RDS_TEXTURE_2D_T_GET(           T, NAME) RDS_TEXTURE_TABLE_T_NAME(2D,           T)[NonUniformResourceIndex(RDS_TEXTURE_T_NAME(T, NAME))]
    #define RDS_TEXTURE_3D_T_GET(           T, NAME) RDS_TEXTURE_TABLE_T_NAME(3D,           T)[NonUniformResourceIndex(RDS_TEXTURE_T_NAME(T, NAME))]
    #define RDS_TEXTURE_CUBE_T_GET(         T, NAME) RDS_TEXTURE_TABLE_T_NAME(Cube,         T)[NonUniformResourceIndex(RDS_TEXTURE_T_NAME(T, NAME))]
    #define RDS_TEXTURE_2D_ARRAY_T_GET(     T, NAME) RDS_TEXTURE_TABLE_T_NAME(2DArray,      T)[NonUniformResourceIndex(RDS_TEXTURE_T_NAME(T, NAME))]

    #define RDS_TEXTURE_2D_T_LOAD(                  T, TEX, UV, LOD)    RDS_TEXTURE_2D_T_GET(T, TEX).Load(uint3((UV).xy, (LOD)))
    #define RDS_TEXTURE_2D_T_SAMPLE(                T, TEX, UV)         RDS_TEXTURE_2D_T_GET(T, TEX).Sample(RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, (UV)))
    #define RDS_TEXTURE_2D_T_SAMPLE_LOD(            T, TEX, UV, LOD)    RDS_TEXTURE_2D_T_GET(T, TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_2D_T_GET_DIMENSIONS(        T, TEX, OUT_WH)     RDS_TEXTURE_2D_T_GET(T, TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

    #define RDS_TEXTURE_3D_T_LOAD(                  T, TEX, UV, LOD)    RDS_TEXTURE_3D_T_GET(T, TEX).Load(uint4((UV).xyz, (LOD)))
    #define RDS_TEXTURE_3D_T_SAMPLE(                T, TEX, UV)         RDS_TEXTURE_3D_T_GET(T, TEX).Sample(RDS_SAMPLER_GET(TEX), (UV))
    #define RDS_TEXTURE_3D_T_SAMPLE_LOD(            T, TEX, UV, LOD)    RDS_TEXTURE_3D_T_GET(T, TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_3D_T_GET_DIMENSIONS(        T, TEX, OUT_WHD)    RDS_TEXTURE_3D_T_GET(T, TEX).GetDimensions(OUT_WHD.x, OUT_WHD.y, OUT_WHD.z)

    #define RDS_TEXTURE_2D_ARRAY_T_LOAD(            T, TEX, UV, LOD)    RDS_TEXTURE_2D_ARRAY_T_GET(T, TEX).Load(uint3((UV).xy, (LOD)))
    #define RDS_TEXTURE_2D_ARRAY_T_SAMPLE(          T, TEX, UV)         RDS_TEXTURE_2D_ARRAY_T_GET(T, TEX).Sample(RDS_SAMPLER_GET(TEX), RDS_TEXTURE_UV2(TEX, (UV)))
    #define RDS_TEXTURE_2D_ARRAY_T_SAMPLE_LOD(      T, TEX, UV, LOD)    RDS_TEXTURE_2D_ARRAY_T_GET(T, TEX).SampleLevel(RDS_SAMPLER_GET(TEX), (UV), (LOD))
    #define RDS_TEXTURE_2D_ARRAY_T_GET_DIMENSIONS(  T, TEX, OUT_WH)     RDS_TEXTURE_2D_ARRAY_T_GET(T, TEX).GetDimensions(OUT_WH.x, OUT_WH.y)

    /* 
    --- define ConstantBuffer Util
    */
    #define RDS_CONSTANT_BUFFER_BINDING(N) RDS_CONCAT(b, N)
    #define RDS_CONSTANT_BUFFER(TYPE, NAME, BINDING) ConstantBuffer<TYPE> NAME : register(RDS_CONSTANT_BUFFER_BINDING(BINDING), RDS_CONSTANT_BUFFER_SPACE)
    
    /* 
    --- define Buffer
    */
    #define RDS_BUFFER(TYPE, NAME)                                  uint NAME
    #define RDS_BUFFER_GET(NAME)                                    rds_bufferTable[NonUniformResourceIndex(NAME)]
    #define RDS_BUFFER_LOAD_I(TYPE, NAME, IDX)                      RDS_BUFFER_GET(NAME).Load<TYPE>(sizeof(TYPE) * (IDX))
    #define RDS_BUFFER_LOAD(TYPE, NAME)                             RDS_BUFFER_LOAD_I(TYPE, NAME, 0)

    #define RDS_RW_BUFFER(TYPE, NAME)                               uint NAME
    #define RDS_RW_BUFFER_GET(NAME)                                 rds_rwBufferTable[NonUniformResourceIndex(NAME)]
    #define RDS_RW_BUFFER_LOAD_I(TYPE, NAME, IDX)                   RDS_RW_BUFFER_GET(NAME).Load<TYPE>( sizeof(TYPE) * (IDX))
    #define RDS_RW_BUFFER_STORE_I(TYPE, NAME, IDX, VALUE)           RDS_RW_BUFFER_GET(NAME).Store<TYPE>(sizeof(TYPE) * (IDX), VALUE)
    #define RDS_RW_BUFFER_LOAD(TYPE, NAME)                          RDS_RW_BUFFER_LOAD_I(TYPE, NAME, 0)
    #define RDS_RW_BUFFER_STORE(TYPE, NAME, VALUE)                  RDS_RW_BUFFER_STORE_I(TYPE, NAME, 0, VALUE)

    #define RDS_RW_BUFFER_ATM_ADD_I(TYPE, NAME, IDX, VAL, OUT_V)    RDS_RW_BUFFER_GET(NAME).InterlockedAdd(sizeof(TYPE) * (IDX), VAL, OUT_V)

    /* 
    --- define Image
    */
    #define RDS_IMAGE_T_NAME(TYPE, NAME) NAME

    #define RDS_IMAGE_2D(        TYPE, NAME)                    uint RDS_IMAGE_T_NAME(TYPE, NAME)
    #define RDS_IMAGE_2D_GET(    TYPE, NAME)                    RDS_IMAGE_TABLE_T_NAME(2D, TYPE)[NonUniformResourceIndex(RDS_IMAGE_T_NAME(TYPE, NAME))]
    #define RDS_IMAGE_2D_LOAD(   TYPE, NAME, UV)                RDS_IMAGE_2D_GET(TYPE, NAME).Load((UV).xy)

    // #define RDS_IMAGE_2D_LOAD_I( TYPE, NAME, IDX)               RDS_IMAGE_2D_GET(TYPE, NAME)[IDX]
    // #define RDS_IMAGE_2D_STORE_I(TYPE, NAME, IDX, VALUE)        RDS_IMAGE_2D_GET(TYPE, NAME)[IDX] = VALUE
    // #define RDS_IMAGE_2D_LOAD(   TYPE, NAME)                    RDS_IMAGE_2D_LOAD_I(TYPE, NAME, 0)
    // #define RDS_IMAGE_2D_STORE(  TYPE, NAME, VALUE)             RDS_IMAGE_2D_STORE_I(TYPE, NAME, 0, VALUE)

    #define RDS_IMAGE_3D(        TYPE, NAME)                    uint RDS_IMAGE_T_NAME(TYPE, NAME)
    #define RDS_IMAGE_3D_GET(    TYPE, NAME)                    RDS_IMAGE_TABLE_T_NAME(3D, TYPE)[NonUniformResourceIndex(RDS_IMAGE_T_NAME(TYPE, NAME))]
    #define RDS_IMAGE_3D_LOAD(   TYPE, NAME, UV)                RDS_IMAGE_3D_GET(TYPE, NAME).Load((UV).xyz)

    #define RDS_IMAGE_2D_ARRAY(        TYPE, NAME)              uint RDS_IMAGE_T_NAME(TYPE, NAME)
    #define RDS_IMAGE_2D_ARRAY_GET(    TYPE, NAME)              RDS_IMAGE_TABLE_T_NAME(2DArray, TYPE)[NonUniformResourceIndex(RDS_IMAGE_T_NAME(TYPE, NAME))]
    #define RDS_IMAGE_2D_ARRAY_LOAD(   TYPE, NAME, UV)          RDS_IMAGE_2D_ARRAY_GET(TYPE, NAME).Load((UV).xyz)

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
RDS_BUFFER(DrawParam,           rds_lastFrame_drawParams);

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

DrawParam rds_DrawParam_getLastFrame(uint idx = 0)
{
    return RDS_BUFFER_LOAD_I(DrawParam, rds_lastFrame_drawParams, idx);
}

Light rds_Lights_get(uint idx)
{
    return RDS_BUFFER_LOAD_I(Light, rds_lights, idx);
}

uint rds_Lights_getLightCount()
{
    return rds_nLights;
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
#pragma mark --- Math-Impl ---
#endif
#if 1

/*
~ reference:
- https://notargs.hateblo.jp/entry/invert_matrix
*/
float3x3 inverse(float3x3 m)
{
    return 1.0 / determinant(m) *
        float3x3(
            m._22 * m._33 - m._23 * m._32,       -(m._12 * m._33 - m._13 * m._32),       m._12 * m._23 - m._13 * m._22,
            -(m._21 * m._33 - m._23 * m._31),    m._11 * m._33 - m._13 * m._31,          -(m._11 * m._23 - m._13 * m._21),
            m._21 * m._32 - m._22 * m._31,       -(m._11 * m._32 - m._12 * m._31),       m._11 * m._22 - m._12 * m._21
        );
}

/*
~ reference:
- https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
*/
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

#if 0
#pragma mark --- SpaceTransform-Impl ---
#endif
#if 1

float4 SpaceTransform_clipToWorld(float4 clip, DrawParam drawParam)
{
    float4 o = mul(drawParam.matrix_vp_inv, clip);
    o = o / (o.w + rds_epsilon);
    return o;
}

float4 SpaceTransform_clipToWorld(float4 clip)
{
    DrawParam drawParam = rds_DrawParam_get();
    return SpaceTransform_clipToWorld(clip, drawParam);
}

float4 SpaceTransform_clipToView(float4 clip, DrawParam drawParam)
{
    float4 o = mul(drawParam.matrix_proj_inv, clip);
    o = o / (o.w + rds_epsilon);
    return o;
}

float4 SpaceTransform_clipToView(float4 clip)
{
    DrawParam drawParam = rds_DrawParam_get();
    return SpaceTransform_clipToView(clip, drawParam);
}

float4 SpaceTransform_screenUvToClip(float2 screenUv, float depth, float w)
{
	float2 uv = screenUv;
    float4 clip = float4(float2(uv.x, 1.0 - uv.y) * 2.0 - 1.0, depth, w);
    //float4 clip = float4(float2(uv.x, uv.y) * 2.0 - 1.0, depth, w);       // vulkan origin ndc (y is down)
	return clip;
}

float4 SpaceTransform_screenToClip(float4 screen, DrawParam drawParam)
{
	float2 screenUv = screen.xy / drawParam.resolution;
    float4 clip = SpaceTransform_screenUvToClip(screenUv, screen.z, screen.w);
	return clip;
}

float4 SpaceTransform_screenToView(float4 screen, DrawParam drawParam)
{
    float4 clip = SpaceTransform_screenToClip(screen, drawParam);
    return SpaceTransform_clipToView(clip, drawParam);
}

float4 SpaceTransform_screenToView(float4 screen)
{
    DrawParam drawParam = rds_DrawParam_get();
    return SpaceTransform_screenToView(screen, drawParam);
}

float3 SpaceTransform_computePositionWs(float2 screen, float ndcZ, DrawParam drawParam)
{
	float4 clip = SpaceTransform_screenUvToClip(screen, ndcZ, 1.0);
	float4 o = SpaceTransform_clipToWorld(clip, drawParam);
	return o.xyz;
}

float SpaceTransform_linearDepthWsToVs(float3 posWs, float4x4 viewMatrix)
{
    float3 o = viewMatrix[0].xyz * posWs.x + viewMatrix[1].xyz * posWs.y + viewMatrix[2].xyz * posWs.z;
    return o.z;
}

float3 SpaceTransform_computePositionWs(float2 screen, float ndcZ, DrawParam drawParam, out float linearDepthVs)
{
	float3 posWs = SpaceTransform_computePositionWs(screen, ndcZ, drawParam).xyz;
    linearDepthVs = SpaceTransform_linearDepthWsToVs(posWs, drawParam.matrix_view);
	return posWs;
}

float3 SpaceTransform_computeNormal(float2 normalXy)
{
	float3 o = float3(normalXy.xy, 0.0);
	o.z = sqrt(1.0 - (o.x * o.x + o.y * o.y));
	return o;
}

float3 SpaceTransform_toWorldNormal(float3 normal, ObjectTransform objectTransform)
{
    //float3 o = (mul((float3x3)transpose(inverse(objectTransform.matrix_model)), normal));
    float3   o = mul((float3x3)objectTransform.matrix_m_inv_t, normal);
	return o;
}

float3 SpaceTransform_toWorldNormal(float3 normal)
{
	return SpaceTransform_toWorldNormal(normal, rds_ObjectTransform_get());
}

float3 SpaceTransform_toViewNormal(float3 normal, ObjectTransform objectTransform, DrawParam drawParam)
{
    //float3   o = mul((float3x3)drawParam.matrix_view, mul((float3x3)transpose(inverse(objectTransform.matrix_model)), normal));
    float3   o = mul((float3x3)objectTransform.matrix_mv_inv_t, normal);
	return (o);
}

float3 SpaceTransform_toViewNormal(float3 normal)
{
	return SpaceTransform_toViewNormal(normal, rds_ObjectTransform_get(), rds_DrawParam_get());
}

float3 SpaceTransform_objectToWorld(float4 v, ObjectTransform objectTransform)
{
	return mul(objectTransform.matrix_model, v).xyz;
}

float3 SpaceTransform_objectToWorld(float4 v)
{
	return SpaceTransform_objectToWorld(v, rds_ObjectTransform_get());
}

float3 SpaceTransform_objectToView(float4 v, DrawParam drawParam, ObjectTransform objectTransform)
{
	//return mul(drawParam.matrix_view, SpaceTransform_objectToWorld(v, objectTransform));
	return mul(objectTransform.matrix_mv, v).xyz;
}

float3 SpaceTransform_objectToView(float4 v, DrawParam drawParam)
{
	return SpaceTransform_objectToView(v, drawParam, rds_ObjectTransform_get());
}

float3 SpaceTransform_objectToView(float4 v)
{
	return SpaceTransform_objectToView(float4(SpaceTransform_objectToWorld(v), 1.0), rds_DrawParam_get());
}

float4 SpaceTransform_objectToClip(float4 v)
{
	return mul(rds_ObjectTransform_get().matrix_mvp, v);
}

float4 SpaceTransform_objectToClip(float4 v, DrawParam drawParam, ObjectTransform objectTransform)
{
	return mul(drawParam.matrix_vp, float4(SpaceTransform_objectToWorld(v, objectTransform), 1.0));
}

float4 SpaceTransform_objectToClip(float4 v, DrawParam drawParam)
{
	return SpaceTransform_objectToClip(v, drawParam, rds_ObjectTransform_get());
}

float3 SpaceTransform_worldToView(float3 v, DrawParam drawParam)
{
	return mul(drawParam.matrix_view, float4(v, 1.0)).xyz;
}

float3 SpaceTransform_worldToView(float3 v)
{
	return SpaceTransform_worldToView(v, rds_DrawParam_get());
}

float4 SpaceTransform_worldToClip(float4 v, DrawParam drawParam)
{
	return mul(drawParam.matrix_vp, v);
}

float4 SpaceTransform_worldToClip(float4 v)
{
	return SpaceTransform_worldToClip(v, rds_DrawParam_get());
}

float4 SpaceTransform_viewToClip(float4 v, DrawParam drawParam)
{
	return mul(drawParam.matrix_proj, v);
}

float4 SpaceTransform_viewToClip(float4 v)
{
	return SpaceTransform_viewToClip(v, rds_DrawParam_get());
}

bool SpaceTransform_isInvalidDepth(float depth)
{
    return depth == 1.0;
}

#endif

#if 0
#pragma mark --- ScreenQuad-Impl ---
#endif
#if 1

float2 ScreenQuad_makeUv(uint vertexId)
{
	float2 o = float2(uint2(vertexId, vertexId << 1) & 2);
	return o;
}

float4 ScreenQuad_makePositionHcs(float2 uv)
{
	float4 o = float4(lerp(float2(-1.0, 1.0), float2(1.0, -1.0), uv), 0.0, 1.0);
	return o;
}

float4 ScreenQuad_makePositionHcs(uint vertexId)
{
	float2 uv = ScreenQuad_makeUv(vertexId);
	float4 o = ScreenQuad_makePositionHcs(uv);
	return o;
}


#endif























#endif

