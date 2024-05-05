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

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

RDS_TEXTURE_2D(texColor);

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHcs = input.positionOs;
    o.positionHcs = input.positionOs;
    o.uv          = input.uv;
    
    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = RDS_TEXTURE_2D_SAMPLE(texColor, input.uv);
    return o;
}