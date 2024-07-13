#ifndef __rdsCascadedShadowMapping_HLSL__
#define __rdsCascadedShadowMapping_HLSL__

#include "built-in/shader/rds_shader.hlsl"

#define CSM_MAX_LEVEL 4

static const float4x4 csm_bias_matrix = 
float4x4(
	0.5, 0.0, 0.0, 0.5,
	0.0, 0.5, 0.0, 0.5,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
);
 
float4x4    csm_matrices[CSM_MAX_LEVEL];
float       csm_plane_dists[CSM_MAX_LEVEL];
uint        csm_level_count;
float       csm_depth_baias;
float       csm_pcf_scale;
RDS_TEXTURE_2D_ARRAY(csm_shadowMap);

float csm_sampleShadowMap(float4 shadowCoord, float2 offset, uint cascadeLevel)
{
    float shadow = 0.0;
    float bias = 0.005;
    bias = csm_depth_baias;

    SamplerState    shadowMapSampler    = RDS_SAMPLER_GET(csm_shadowMap);
    Texture2DArray  shadowMap           = RDS_TEXTURE_2D_ARRAY_GET(csm_shadowMap);
    
    if (shadowCoord.w > 0 && shadowCoord.z > 0.0 && shadowCoord.z < 1.0) 
    {
        float2 uv   = shadowCoord.xy + offset;
        float dist  = shadowMap.Sample(shadowMapSampler, float3(uv, cascadeLevel)).r;
        if (dist < shadowCoord.z - bias) 
        {
            shadow = 1.0;
        }
    }
    return shadow;
}

float csm_sampleShadowMap_Pcf(float4 shadowCoord, float2 offset, uint cascadeLevel)
{
    float shadow = 0.0;

    SamplerState    shadowMapSampler    = RDS_SAMPLER_GET(csm_shadowMap);
    Texture2DArray  shadowMap           = RDS_TEXTURE_2D_ARRAY_GET(csm_shadowMap);

    uint3 dim;
    shadowMap.GetDimensions(dim.x, dim.y, dim.z);
    float scale = csm_pcf_scale;
    float dx    = scale * 1.0 / (float)dim.x;
    float dy    = scale * 1.0 / (float)dim.y;

    int count = 0;
    int range = 1;
    for (int y = -range; y <= range; ++y)
    {
        for (int x = -range; x <= range; ++x)
        {
            shadow += csm_sampleShadowMap(shadowCoord, float2(dx * x, dy * y), cascadeLevel);
            count++;
        }
    }
    return shadow / count;
}

uint csm_computeCascadeLevel(float linearDepthVs)
{
    uint cascadeLevel = 0;
    #if 0
    for(uint i = 0; i < csm_level_count - 1; ++i) 
    {
	    if(linearDepthVs < csm_plane_dists[i]) 
        {
		    cascadeLevel = i + 1;
	    }
    }
    #else
    float3 planDists    = float3(csm_plane_dists[0], csm_plane_dists[1], csm_plane_dists[2]);
    float3 result       = step(float3(linearDepthVs, linearDepthVs, linearDepthVs), planDists);
    cascadeLevel = result.x + result.y + result.z;
    // TODO: lerp cascade index
    #endif

    return cascadeLevel;
}

float csm_computeShadow(float3 posWs, float linearDepthVs)
{
    uint cascadeLevel = 0;
    cascadeLevel = csm_computeCascadeLevel(linearDepthVs);
    
    float4 shadowCoord = mul(csm_bias_matrix, mul(csm_matrices[cascadeLevel], float4(posWs, 1.0)));
    shadowCoord /= shadowCoord.w;

    float shadow = 0.0;
    shadow = csm_sampleShadowMap_Pcf(shadowCoord, float2(0.0, 0.0), cascadeLevel);

    return shadow;
}

float3 csm_debugCascadeLevel(float linearDepthVs)
{
    float3 o = float3(0.0, 0.0, 0.0);
    #if 1
	{
        static const float3 colors[] = {
            float3(1.0, 0.0, 0.0),
            float3(0.0, 1.0, 0.0),
            float3(1.0, 1.0, 0.0),
            float3(0.0, 0.0, 1.0),
        };

		uint cascadeLevel = 0;
        cascadeLevel = csm_computeCascadeLevel(linearDepthVs);
		o.rgb = colors[cascadeLevel];
	}
	#endif

    return o;
}

#endif

