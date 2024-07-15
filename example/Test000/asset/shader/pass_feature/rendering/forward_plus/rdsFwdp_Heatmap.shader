#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

		DepthTest	LessEqual
		DepthWrite	false

		Wireframe false

		BlendRGB 	Add SrcAlpha 	OneMinusSrcAlpha
		BlendAlpha	Add One 		Zero
		
		VsFunc		vs_main
		PsFunc		ps_main
	}

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

/*
	references:
	~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "rdsFwdp_Common.hlsl"
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

#if 0
#pragma mark --- fwpd_lighting_param ---
#endif // 0
#if 1

RDS_BUFFER(uint, lightIndexList);
RDS_TEXTURE_2D_T(uint2, lightGrid);
RDS_TEXTURE_2D(tex_color);

#endif

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;

	uint 	vertexId 	= input.vertexId;
	float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionHcs = ScreenQuad_makePositionHcs(uv);
    o.uv          = uv;
    
    return o;
}

//[earlydepthstencil]
float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam 	drawParam 	= rds_DrawParam_get();
	float2 		uv 			= input.uv;

	//o.rgba = RDS_TEXTURE_2D_SAMPLE(tex_color, input.uv);
    uint2 blockIndex 			= uint2(floor(input.positionHcs.xy / BLOCK_SIZE));
	uint2 lightBlock 			= RDS_TEXTURE_2D_T_GET(uint2, lightGrid)[blockIndex];
 	uint  lightIdxStartOffset 	= lightBlock.x;
    uint  lightCount 			= lightBlock.y;

	float heatMapColor = (float)lightCount / LIGHT_LIST_LOCAL_SIZE;
	o.rgb = float3(0.0, heatMapColor, 1.0);
	
    return o;
}