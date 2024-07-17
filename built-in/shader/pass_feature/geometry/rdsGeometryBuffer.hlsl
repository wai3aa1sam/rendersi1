
#ifndef __rdsGeometryBuffer_HLSL__
#define __rdsGeometryBuffer_HLSL__

#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

#if 0
#pragma mark --- GeometryBuffer-Read-Impl ---
#endif
#if 1

RDS_TEXTURE_2D_T(float,  tex_depth);
RDS_TEXTURE_2D_T(float4, gBuf_normal);
RDS_TEXTURE_2D_T(float4, gBuf_baseColor);
RDS_TEXTURE_2D_T(float4, gBuf_roughnessMetalness);
RDS_TEXTURE_2D_T(float4, gBuf_emission);

// just for debug purpose
RDS_TEXTURE_2D_T(float4, gBuf_position);

float 
GBuffer_sampleDepthBuffer(float2 uv)
{
	float  depth = RDS_TEXTURE_2D_T_SAMPLE_LOD(float, tex_depth, uv, 0);
	return depth;
}

Surface 
GBuffer_makeSurface(float2 uv, float depth, DrawParam drawParam, out float linearDepthVs)
{
	// TODO: sample normal from RG16s and unpack it
	//float2 sampleNormal 		= RDS_TEXTURE_2D_T_SAMPLE_LOD(float2, gBuf_normal, 				uv, 0).xy;

	float3 sampleNormal 		= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_normal, 				uv, 0).xyz;
	float4 baseColor			= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_baseColor, 			uv, 0);
	float4 roughnessMetalness 	= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_roughnessMetalness,	uv, 0);
	float4 emission				= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_emission, 			uv, 0);
	float3 posWs 				= SpaceTransform_computePositionWs(uv, depth, drawParam, linearDepthVs);

	//float3 normal 				= SpaceTransform_computeNormal(remap01ToNeg11(sampleNormal.xy));
	float3 normal 				= (remap01ToNeg11(sampleNormal));

	Surface surface = Material_makeSurface(posWs, normal, roughnessMetalness, baseColor, emission);
	return surface;
}

#endif













#endif