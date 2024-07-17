#if 0
Shader {
	Properties {
		
	}
	
	Pass {	

		// Queue	"Transparent"
		//Cull		None

		DepthTest	Always
		DepthWrite	false

//		Wireframe false

		BlendRGB 	Disable One OneMinusSrcAlpha
		BlendAlpha	Disable One OneMinusSrcAlpha

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
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"
#include "built-in/shader/pass_feature/geometry/rdsGeometryBuffer.hlsl"

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

RDS_TEXTURE_2D(indirect_diffuse);
RDS_TEXTURE_2D(indirect_specular);

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;

	uint 	vertexId 	= input.vertexId;
	float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionHcs = ScreenQuad_makePositionHcs(uv);
    o.uv          = uv;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam 	drawParam 		= rds_DrawParam_get();
	float2 		uv				= input.uv;
	float3 		viewDir;
	float		linearDepthVs;

	float depth = GBuffer_sampleDepthBuffer(uv);
	if (SpaceTransform_isInvalidDepth(depth))
		return o;

	Surface surface = GBuffer_makeSurface(uv, depth, drawParam, linearDepthVs);
	float3 posWs = surface.pos;

	LightingResult oLightingResult = (LightingResult)0;
	oLightingResult = Lighting_computeForwardLighting_Ws(surface, drawParam.camera_pos, viewDir);

	float shadow = csm_computeShadow(posWs, linearDepthVs);
	shadow = 0.0;
	o.rgb += (1.0 - shadow) * oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;

	float4 indirectDiffuse 	= RDS_TEXTURE_2D_SAMPLE(indirect_diffuse, 	uv);
	float4 indirectSpecular = RDS_TEXTURE_2D_SAMPLE(indirect_specular, 	uv);

	//o.rgb = indirectDiffuse.a * o.rgb + indirectDiffuse.rgb + indirectSpecular.rgb;

	//o.rgb = indirectDiffuse.rgb;
	o.rgb += (1.0 - shadow) * oLightingResult.diffuse.rgb;
	
	//o.rgb = indirectDiffuse.rgb;

	//o.rgb = remapNeg11To01(normal);
	//o.rgb = baseColor.rgb;

	//o.rgb = posWs.xyz;
	//float3 posVs = SpaceTransform_worldToView(posWs.xyz).xyz;
	//o.rgb = posVs.xyz;

    return o;
}
