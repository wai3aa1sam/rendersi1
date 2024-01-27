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

RDS_TEXTURE_2D_1(texture0);
RDS_TEXTURE_2D_2(texture1);

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
    float4 o;
    o = float4(1.0, 1.0, 1.0, 1.0);
    if (i.uv.x > 0.5)
    {
	    o = RDS_SAMPLE_TEXTURE_2D(texture0, i.uv);
    }
    else
    {
	    o = RDS_SAMPLE_TEXTURE_2D(texture1, i.uv);
    }

    return o;
}