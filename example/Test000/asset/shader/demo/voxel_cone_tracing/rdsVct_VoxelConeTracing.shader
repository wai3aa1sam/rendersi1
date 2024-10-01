#if 0
Shader {
	Properties {
		Float step_size				= 0.25
		Float step_max_dist			= 10.0
		Float trace_start_offset	= 1.0
	}
	
	Pass {	
		CsFunc		Cs_voxelConeTrace

		VsFunc		vs_main
		PsFunc		ps_main
	}

	Permutation
	{
		//RDS_VCT_DIFFUSE_CONE_COUNT = {6, 16, 32}
	}
}
#endif

#include "rdsVct_Common.hlsl"
#include "rdsVct_ConeTracing_Common.hlsl"
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"
#include "built-in/shader/pass_feature/geometry/rdsGeometryBuffer.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

float step_size;
float step_max_dist;
float trace_start_offset;

#if VCT_USE_6_FACES_CLIPMAP
RDS_TEXTURE_3D_T(float4, voxel_tex_radiance);
#else
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_z);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_z);
#endif

RDS_IMAGE_2D(float4, out_indirect_diffuse);
RDS_IMAGE_2D(float4, out_indirect_specular);

float4 Vct_sampleVoxelClipmap(Texture3D<float4> voxelTex, SamplerState voxelTexSampler, float3 uvw, float3 faceOffsets
							, VoxelClipmap clipmap, float level
							, float3 dirWeights, float stepDist, float clipmapMaxLevel, float voxelResolutionInv)
{
	float4 o = (float4)0;

	#if 1
	// half voxel correction
	float halfVoxel = 0.5 * voxelResolutionInv;
	uvw = clamp(uvw, halfVoxel, 1 - halfVoxel);
	#endif

	float4 anisotropicSample = (float4)0;

	#if VCT_USE_6_FACES_CLIPMAP

	uvw.x = uvw.x / (Vct_kClipmapFaceCount);
	uvw.y = (uvw.y + level) / clipmapMaxLevel;

	anisotropicSample += voxelTex.SampleLevel(voxelTexSampler, uvw + float3(faceOffsets.x, 0.0, 0.0), 0) * dirWeights.x;
	anisotropicSample += voxelTex.SampleLevel(voxelTexSampler, uvw + float3(faceOffsets.y, 0.0, 0.0), 0) * dirWeights.y;
	anisotropicSample += voxelTex.SampleLevel(voxelTexSampler, uvw + float3(faceOffsets.z, 0.0, 0.0), 0) * dirWeights.z;

	#else
    int anisotropicLevel = max(level - 1.0f, 0.0f);

	//anisotropicSample += RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, faceOffsets.x * 6 <= 0.5 ? voxel_tex_pos_x : voxel_tex_neg_x, uvw, anisotropicLevel) * dirWeights.x;
	//anisotropicSample += RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, faceOffsets.y * 6 <= 2.5 ? voxel_tex_pos_y : voxel_tex_neg_y, uvw, anisotropicLevel) * dirWeights.y;
	//anisotropicSample += RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, faceOffsets.z * 6 <= 4.5 ? voxel_tex_pos_z : voxel_tex_neg_z, uvw, anisotropicLevel) * dirWeights.z;

	anisotropicSample += (faceOffsets.x * 6 <= 0.5 ? RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_pos_x, uvw, anisotropicLevel) : RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_neg_x, uvw, anisotropicLevel)) * dirWeights.x;
	anisotropicSample += (faceOffsets.y * 6 <= 2.5 ? RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_pos_y, uvw, anisotropicLevel) : RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_neg_y, uvw, anisotropicLevel)) * dirWeights.y;
	anisotropicSample += (faceOffsets.z * 6 <= 4.5 ? RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_pos_z, uvw, anisotropicLevel) : RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_neg_z, uvw, anisotropicLevel)) * dirWeights.z;

	if (level < 1.0)
	{
		anisotropicSample = lerp(voxelTex.SampleLevel(voxelTexSampler, uvw, 0), anisotropicSample, clamp(level, 0.0, 1.0));
	}
	#endif

	//anisotropicSample = voxelTex.SampleLevel(voxelTexSampler, uvw, 0);

	o = anisotropicSample;

	//o = saturate(anisotropicSample);
	o *= stepDist / clipmap.voxelSize;
	return o;
}

float4 Vct_coneTrace(Texture3D<float4> voxelTex, SamplerState voxelTexSampler, float3 pos, float3 normal
					, float3 coneDir, float coneAperture
					, float stepSize, float maxDist, uint clipmapMaxLevel, float voxelResolution)
{
	float4 result = (float4)0;

	DrawParam drawParam = rds_DrawParam_get();
	float3 cameraPos = drawParam.camera_pos;

	float 	voxelResolutionInv 	= 1.0 / voxelResolution;

	VoxelClipmap 	clipmap 			= Vct_getVoxelClipmap(0);
	float 			voxelFullSizeL0 	= voxel_sizeL0 * 2.0;		// full extent
	float 			voxelFullSizeL0_inv = 1.0 / voxelFullSizeL0;

	// (Diameter / 2) / F = tan(theta / 2)
	// Diameter = F * (2 * tan(theta / 2))
	float 	diameterFactor 	= 2.0 * tan(coneAperture * 0.5); 
	//diameterFactor 	= 2.0 * coneAperture; 

	float3 	sampleDir 		= -coneDir;
	float3 	faceOffsets 	= Vct_computeVoxelFaceOffsets(sampleDir);
	float3  dirWeights 		= abs(coneDir);

	float 	level 		= 0;
	float 	dist 		= voxelFullSizeL0;
	float 	stepDist 	= dist;
	float3 	startPos 	= pos + normal * voxelFullSizeL0 * trace_start_offset;
	//startPos += coneDir * voxelFullSizeL0 * trace_start_offset * 0.5;
	while (dist < maxDist * 1 && result.a < 1.0 && level < clipmap_maxLevel)
	{
		float3 dstPos = startPos + coneDir * dist;

		#if 0
		{
			dstPos = pos;
			float3 uvw = Vct_worldToClipmapUvw(dstPos, clipmap, voxelResolutionInv);
			if (!isInBoundary01(uvw))
			{
				break;
			}
			float4 sampleVoxel = Vct_sampleVoxelClipmap(voxelTex, voxelTexSampler, uvw, faceOffsets, clipmap, level
													, dirWeights, stepDist, clipmapMaxLevel, voxelResolutionInv);
			result.rgb 	+= sampleVoxel.rgb;
			result.a 	+= sampleVoxel.a;
			break;
		}
		#endif
		
		float diameter 	= max(voxelFullSizeL0, dist * diameterFactor);
		float lod 		= floor(clamp(log2(diameter * voxelFullSizeL0_inv), level, clipmapMaxLevel - 1));
		clipmap 		= Vct_getVoxelClipmap(lod);

		float3 uvw = Vct_worldToClipmapUvw(dstPos, clipmap, voxelResolutionInv);
		if (!isInBoundary01(uvw))
		{
			level++;
			continue;
		}

		float4 sampleVoxel = Vct_sampleVoxelClipmap(voxelTex, voxelTexSampler, uvw, faceOffsets, clipmap, lod
													, dirWeights, stepDist, clipmapMaxLevel, voxelResolutionInv);

		float a = 1 - result.a;
		result.rgb 	+= a * sampleVoxel.rgb;
		result.a	+= a * sampleVoxel.a;

		stepDist = diameter / diameterFactor * stepSize;
		stepDist = diameter * stepSize;

		dist 	+= stepDist;
		//dist 	+= max(diameter, voxelFullSizeL0) * stepSize;
		break;
	}

	return result;
}

float4 Vct_computeIndirectDiffuse(Texture3D<float4> voxelTex, SamplerState voxelTexSampler, float3 pos, float3 normal, uint voxelResolution)
{
	float4 result 		= (float4)0;
	float  totalWeight 	= 0.0;

	#if !VCT_USE_6_FACES_CLIPMAP
	// Gram-Schmidt process, re-orthogonalize such that perpendicular to others
	float3 upDir = float3(0.0, 1.0, 0.0);
	if (abs(dot(normal, upDir)) == 1.0)
		upDir = float3(0.0, 0.0, 1.0);
	float3 right 	= normalize(upDir - dot(normal, upDir) * normal);
	float3 up 		= cross(right, normal);
	#endif

	float3 coneDir;
	for (int iCone = 0; iCone < Vct_kDiffuseConeCount; ++iCone)
	{
		#if VCT_USE_6_FACES_CLIPMAP
		coneDir = Vct_kDiffuseConeDirections[iCone];
		#else
		coneDir = normalize(normal + Vct_kDiffuseConeDirections[iCone].x * right + Vct_kDiffuseConeDirections[iCone].z * up);
		#endif

		float coneWeight = dot(normal, coneDir);
		if (coneWeight <= 0.0)
			continue;

		result += Vct_coneTrace(voxelTex, voxelTexSampler, pos, normal, coneDir, Vct_kDiffuseConeAperture
								, step_size, step_max_dist, clipmap_maxLevel, voxelResolution) * coneWeight;
		totalWeight += coneWeight;
	}
	result /= totalWeight;

	result.rgb = max(0.0, result.rgb);
	result.a   = saturate(result.a);

	return result;
}

float4 Vct_computeIndirectSpecular(Texture3D<float4> voxelTex, SamplerState voxelTexSampler, float3 pos, float3 normal
									, float3 viewDir, float roughness, uint voxelResolution)
{
	float4 result = (float4)0;

	float3 	coneDir 	= reflect(-viewDir, normal);
	float 	aperture 	= max(roughness, Vct_kDiffuseConeAperture);
	result += Vct_coneTrace(voxelTex, voxelTexSampler, pos, normal, coneDir, aperture
								, step_size, step_max_dist, clipmap_maxLevel, voxelResolution);

	result.rgb = max(0.0, result.rgb);
	result.a   = saturate(result.a);

	return result;
}

[numThreads(CS_BLOCK_SIZE, CS_BLOCK_SIZE, 1)]
void Cs_voxelConeTrace(ComputeIn input)
{
	DrawParam 	drawParam 	= rds_DrawParam_get();
	uint2 		resolution 	= drawParam.resolution;

	bool isInBoundary = Image_isInBoundary(input.dispatchThreadId, resolution.xy);
	if (!isInBoundary)
		 return;

	uint3 	dispatchThreadId 	= input.dispatchThreadId;
	
	float2 	uv 					= Image_computeUv(dispatchThreadId.xy, drawParam);;
	uint2	imageCoords			= dispatchThreadId.xy;
	float 	voxelResolution		= Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	float   linearDepthVs;

	float depth = GBuffer_sampleDepthBuffer(uv);
	if (SpaceTransform_isInvalidDepth(depth))
		return;

	Surface surface = GBuffer_makeSurface(uv, depth, drawParam, linearDepthVs);
	float3 pos 			= surface.pos;
	float3 normal 		= surface.normal;
	float  roughness 	= surface.roughness;

	float3 viewPos				= true ? drawParam.camera_pos : (float3)0;	// false -> view space
	float3 viewDir				= normalize(viewPos - pos);

	Texture3D<float4> 	voxelTex 		= RDS_TEXTURE_3D_T_GET(float4, voxel_tex_radiance);
	SamplerState 		voxelTexSampler	= RDS_SAMPLER_GET(voxel_tex_radiance);

	float4 indirectDiffuse 	= Vct_computeIndirectDiffuse( voxelTex, voxelTexSampler, pos, normal, voxelResolution);
	float4 indirectSpecular = Vct_computeIndirectSpecular(voxelTex, voxelTexSampler, pos, normal, viewDir, roughness, voxelResolution);

	RDS_IMAGE_2D_GET(float4,  out_indirect_diffuse)[imageCoords] = indirectDiffuse;
	RDS_IMAGE_2D_GET(float4, out_indirect_specular)[imageCoords] = indirectSpecular;

	//RDS_IMAGE_2D_GET(float4,  out_indirect_diffuse)[imageCoords] = float4(remapNeg11To01(normal).rgb, 1.0);
	//RDS_IMAGE_2D_GET(float4,  out_indirect_diffuse)[imageCoords] = float4(surface.baseColor.rgb, 1.0);

	//pos = RDS_TEXTURE_2D_T_SAMPLE_LOD(float3, gBuf_position, 	uv, 0);
	//RDS_IMAGE_2D_GET(float4,  out_indirect_diffuse)[imageCoords] = float4(pos.rgb, 1.0);
}

#if 1

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelOut
{
	float4 diffuse	: SV_Target0;
    float4 specular	: SV_Target1;
};

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;

	uint 	vertexId 	= input.vertexId;
	float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionHcs = ScreenQuad_makePositionHcs(uv);
    o.uv          = uv;
    
    return o;
}

PixelOut ps_main(PixelIn input)
{
	PixelOut o =(PixelOut)0;

	float4 oColor = float4(0.0, 0.0, 0.0, 1.0);
	o.diffuse 	= oColor;
	o.specular 	= oColor;

	DrawParam 	drawParam 		= rds_DrawParam_get();
	float2 		uv				= input.uv;
	float 		voxelResolution	= Vct_computeVoxelResolutionWithBorder(voxel_resolution);

	float  depth 				= RDS_TEXTURE_2D_T_SAMPLE_LOD(float,  tex_depth, 				uv, 0);
	if (SpaceTransform_isInvalidDepth(depth))
		return o;

	float2 sampleNormal 		= RDS_TEXTURE_2D_T_SAMPLE_LOD(float2, gBuf_normal, 				uv, 0);
	float4 baseColor			= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_baseColor, 			uv, 0);
	float4 roughnessMetalness 	= RDS_TEXTURE_2D_T_SAMPLE_LOD(float4, gBuf_roughnessMetalness,	uv, 0);
	float3 pos 					= SpaceTransform_computePositionWs(uv, depth, drawParam).xyz;
	float3 normal 				= SpaceTransform_computeNormal(sampleNormal);
	float  roughness 			= Material_unpackRoughness(roughnessMetalness);

	float3 viewPos				= true ? drawParam.camera_pos : (float3)0;	// false -> view space
	float3 viewDir				= normalize(viewPos - pos);

	Texture3D<float4> 	voxelTex 		= RDS_TEXTURE_3D_T_GET(float4, voxel_tex_radiance);
	SamplerState 		voxelTexSampler	= RDS_SAMPLER_GET(voxel_tex_radiance);

	float4 indirectDiffuse 	= Vct_computeIndirectDiffuse( voxelTex, voxelTexSampler, pos, normal, voxelResolution);
	float4 indirectSpecular = Vct_computeIndirectSpecular(voxelTex, voxelTexSampler, pos, normal, viewDir, roughness, voxelResolution);

	o.diffuse 	= indirectDiffuse;
	o.specular 	= indirectSpecular;

    return o;
}

#endif