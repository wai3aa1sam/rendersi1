#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	LessEqual

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
    float3 normal       : NORMAL0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = mul(rds_matrix_mvp, i.positionOS);
    o.normal      = i.normal;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    //float2 pos2f = i.positionOS;

    float4 color;
    color = float4(1.0, 1.0, 1.0, 1.0);
	//color.r = i.positionHCS.z;
    //color.a = 1.0;
    return color;
}