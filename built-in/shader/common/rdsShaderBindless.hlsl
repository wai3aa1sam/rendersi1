#ifndef __rdsShaderBindless_HLSL__
#define __rdsShaderBindless_HLSL__

#include "rdsMarco_Common.hlsl"
#include "rdsConstant_Common.hlsl"
#include "built-in/shader/interop/rdsShaderInterop.hlsl"

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


#endif