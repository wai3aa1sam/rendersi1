#ifndef __rdsShaderNonBindless_HLSL__
#define __rdsShaderNonBindless_HLSL__

#include "rdsMarco_Common.hlsl"
#include "rdsConstant_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop.hlsl"

/* 
--- define space for bindless type
*/

//#define RDS_CONSTANT_BUFFER_SPACE spacex		// define when compile
#define RDS_TEXURE_SPACE 	space1
#define RDS_SAMPLER_SPACE 	RDS_TEXURE_SPACE

//#define RDS_K_SAMPLER_COUNT 1		// set in compiler
SamplerState rds_samplers[RDS_K_SAMPLER_COUNT] : register(s33, RDS_CONSTANT_BUFFER_SPACE);

/* 
--- define Texture
*/

#define RDS_SAMPLER_NAME(NAME)      RDS_CONCAT(NAME, _sampler)
#define RDS_SAMPLER_GET(NAME)       rds_samplers[RDS_SAMPLER_NAME(NAME)]

#define RDS_TEXTURE_ST_SUFFIX _ST_
#define RDS_TEXTURE_UV2(NAME, UV)          float2(UV.xy * RDS_TEXTURE_ST(NAME).xy + RDS_TEXTURE_ST(NAME).zw)

#define RDS_TEXTURE_NAME(           NAME)  NAME
#define RDS_TEXTURE_ST(             NAME)  RDS_CONCAT(NAME, RDS_TEXTURE_ST_SUFFIX)
#define RDS_TEXTURE_1D(             NAME)  Texture1D        RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_2D(             NAME)  Texture2D        RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME); float4 RDS_TEXTURE_ST(NAME)
#define RDS_TEXTURE_3D(             NAME)  Texture3D        RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_CUBE(           NAME)  TextureCube      RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_2D_ARRAY(       NAME)  Texture2DArray   RDS_TEXTURE_NAME(NAME); uint RDS_SAMPLER_NAME(NAME)

#define RDS_TEXTURE_1D_GET(             NAME) RDS_TEXTURE_NAME(NAME)
#define RDS_TEXTURE_2D_GET(             NAME) RDS_TEXTURE_NAME(NAME)
#define RDS_TEXTURE_3D_GET(             NAME) RDS_TEXTURE_NAME(NAME)
#define RDS_TEXTURE_CUBE_GET(           NAME) RDS_TEXTURE_NAME(NAME)
#define RDS_TEXTURE_2D_ARRAY_GET(       NAME) RDS_TEXTURE_NAME(NAME)

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
#define RDS_TEXTURE_1D_T(           T, NAME) Texture1D      <T> RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_2D_T(           T, NAME) Texture2D      <T> RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME); float4 RDS_TEXTURE_ST(NAME)
#define RDS_TEXTURE_3D_T(           T, NAME) Texture3D      <T> RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_CUBE_T(         T, NAME) TextureCube    <T> RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)
#define RDS_TEXTURE_2D_ARRAY_T(     T, NAME) Texture2DArray <T> RDS_TEXTURE_T_NAME(T, NAME); uint RDS_SAMPLER_NAME(NAME)

#define RDS_TEXTURE_1D_T_GET(           T, NAME) RDS_TEXTURE_T_NAME(T, NAME)
#define RDS_TEXTURE_2D_T_GET(           T, NAME) RDS_TEXTURE_T_NAME(T, NAME)
#define RDS_TEXTURE_3D_T_GET(           T, NAME) RDS_TEXTURE_T_NAME(T, NAME)
#define RDS_TEXTURE_CUBE_T_GET(         T, NAME) RDS_TEXTURE_T_NAME(T, NAME)
#define RDS_TEXTURE_2D_ARRAY_T_GET(     T, NAME) RDS_TEXTURE_T_NAME(T, NAME)

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
#define RDS_BUFFER_NAME(NAME)                                   NAME
#define RDS_BUFFER(TYPE, NAME)                                  ByteAddressBuffer RDS_BUFFER_NAME(NAME)
#define RDS_BUFFER_GET(NAME)                                    RDS_BUFFER_NAME(NAME)
#define RDS_BUFFER_LOAD_I(TYPE, NAME, IDX)                      RDS_BUFFER_GET(NAME).Load<TYPE>(sizeof(TYPE) * (IDX))
#define RDS_BUFFER_LOAD(TYPE, NAME)                             RDS_BUFFER_LOAD_I(TYPE, NAME, 0)

#define RDS_RW_BUFFER_NAME(NAME)                                NAME
#define RDS_RW_BUFFER(TYPE, NAME)                               RWByteAddressBuffer RDS_RW_BUFFER_NAME(NAME)
#define RDS_RW_BUFFER_GET(NAME)                                 RDS_RW_BUFFER_NAME(NAME)
#define RDS_RW_BUFFER_LOAD_I(TYPE, NAME, IDX)                   RDS_RW_BUFFER_GET(NAME).Load<TYPE>( sizeof(TYPE) * (IDX))
#define RDS_RW_BUFFER_STORE_I(TYPE, NAME, IDX, VALUE)           RDS_RW_BUFFER_GET(NAME).Store<TYPE>(sizeof(TYPE) * (IDX), VALUE)
#define RDS_RW_BUFFER_LOAD(TYPE, NAME)                          RDS_RW_BUFFER_LOAD_I(TYPE, NAME, 0)
#define RDS_RW_BUFFER_STORE(TYPE, NAME, VALUE)                  RDS_RW_BUFFER_STORE_I(TYPE, NAME, 0, VALUE)

#define RDS_RW_BUFFER_ATM_ADD_I(TYPE, NAME, IDX, VAL, OUT_V)    RDS_RW_BUFFER_GET(NAME).InterlockedAdd(sizeof(TYPE) * (IDX), VAL, OUT_V)

/* 
--- define Image
*/

// #define RDS_IMAGE_T(DIMENSION, TYPE, NAME) RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(RDS_CONCAT(RWTexture, DIMENSION), <), TYPE), >) NAME
// #define RDS_IMAGE_T_NAME(DIMENSION, TYPE, NAME) NAME
#define RDS_IMAGE_NAME(NAME)  NAME

#define RDS_IMAGE_2D(        TYPE, NAME)                    RWTexture2D<TYPE> RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_2D_GET(    TYPE, NAME)                    RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_2D_LOAD(   TYPE, NAME, UV)                RDS_IMAGE_2D_GET(TYPE, NAME).Load((UV).xy)

// #define RDS_IMAGE_2D_LOAD_I( TYPE, NAME, IDX)               RDS_IMAGE_2D_GET(TYPE, NAME)[IDX]
// #define RDS_IMAGE_2D_STORE_I(TYPE, NAME, IDX, VALUE)        RDS_IMAGE_2D_GET(TYPE, NAME)[IDX] = VALUE
// #define RDS_IMAGE_2D_LOAD(   TYPE, NAME)                    RDS_IMAGE_2D_LOAD_I(TYPE, NAME, 0)
// #define RDS_IMAGE_2D_STORE(  TYPE, NAME, VALUE)             RDS_IMAGE_2D_STORE_I(TYPE, NAME, 0, VALUE)

#define RDS_IMAGE_3D(        TYPE, NAME)                    RWTexture3D<TYPE> RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_3D_GET(    TYPE, NAME)                    RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_3D_LOAD(   TYPE, NAME, UV)                RDS_IMAGE_3D_GET(TYPE, NAME).Load((UV).xyz)

#define RDS_IMAGE_2D_ARRAY(        TYPE, NAME)              RWTexture2DArray<TYPE> RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_2D_ARRAY_GET(    TYPE, NAME)              RDS_IMAGE_NAME(NAME)
#define RDS_IMAGE_2D_ARRAY_LOAD(   TYPE, NAME, UV)          RDS_IMAGE_2D_ARRAY_GET(TYPE, NAME).Load((UV).xyz)

#endif