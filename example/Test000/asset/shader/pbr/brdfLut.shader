#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

//		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

/*
reference:
~ https://learnopengl.com/PBR/IBL/Diffuse-irradiance
*/

#include "rdsPbrCommon.hlsl"

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

static const uint s_kSampleCount = 1024u;

PixelIn vs_main(VertexIn input)
{
    PixelIn o;

    uint 	vertexId 	= input.vertexId;
    float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionHcs = ScreenQuad_makePositionHcs(uv);
    o.uv          = uv;
    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);

    float2 uv = input.uv;
    o.xy = Pbr_integrateBrdf(uv.x, uv.y, s_kSampleCount);
    
    return float4(o, 1.0);
}