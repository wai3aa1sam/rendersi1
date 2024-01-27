#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "common/rdsCommon.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};


RDS_TEXTURE_2D(texture0);

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    o.positionHCS = i.positionOS;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	float4 o = RDS_SAMPLE_TEXTURE_2D(texture0, i.uv);
    return o;
}