#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

		DepthTest	Less

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

#include "built-in/shader/common/rdsCommon.hlsl"

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

struct PixelOut
{
	float4 albedo	: SV_Target0;
    float4 normal	: SV_Target1;
    float4 position	: SV_Target2;
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    o.positionHCS = mul(RDS_MATRIX_MVP, i.positionOS);
    o.uv          = i.uv;
	o.normal      = i.normal;
    
    return o;
}

PixelOut ps_main(PixelIn i)
{
	PixelOut o;
    //float2 pos2f = i.positionOS;

	o.albedo   = float4(1.0, 1.0, 1.0, 1.0);
	o.normal   = float4(i.normal, 1.0);
	o.position = i.positionHCS;

	o.normal = (o.normal + float4(1.0, 1.0, 1.0, 1.0)) / 2.0;

    return o;
}