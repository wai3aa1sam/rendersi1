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
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float3 positionOS	: POSITION;
    float2 uv           : TEXCOORD0;
};

static const uint s_kSampleCount = 1024u;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    o.positionHCS   = mul(RDS_MATRIX_MVP, i.positionOS);
    o.positionOS 	= i.positionOS.xyz;
    o.uv            = i.uv;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);

    float2 uv = i.uv;
    o.xy = Pbr_integrateBrdf(uv.x, uv.y, s_kSampleCount);
    
    return float4(o, 1.0);
}