#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	Less

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
    float3 normal       : NORMAL0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

PixelIn vs_main(VertexIn input)
{
    PixelIn o = (PixelIn)0;
    
    //o.positionOs    = positions[input.vertexId];
    //o.color         = colors[input.vertexId];
    o.positionHcs = mul(RDS_MATRIX_MVP, input.positionOs);
    o.normal      = input.normal;
    o.uv          = input.uv;
    
    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    //float2 pos2f = input.positionOs;

    float4 color;
    color = float4(1.0, 1.0, 1.0, 1.0);
	//color.r = input.positionHcs.z;
    //color.a = input.0;
    return color;
}