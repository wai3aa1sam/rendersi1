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

		//Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

//#include "common/rdsCommon.hlsl"
#include "common/rdsPbrCommon.hlsl"

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
	float4 positionWS   : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

RDS_TEXTURE_2D_1(texture0);
RDS_TEXTURE_2D_2(texture1);

cbuffer PbrParam
{
    float   roughness;
    float3  posLight;
    float3  colorLight;
    float   ao;
    float   albedo;
    float   metallic;
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    o.positionHCS = mul(rds_matrix_mvp,     i.positionOS);
    o.positionWS  = mul(rds_matrix_model,   i.positionOS);
    o.normal      = i.normal;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o;
    o = float3(1.0, 1.0, 1.0);

    rds_Surface surface;
    surface.posWS       = i.positionWS;
    surface.normal      = i.normal;
    surface.roughness   = roughness;
    surface.metallic    = metallic;

    float3 posView = rds_camera_pos;    // rds_camera_pos

    //float3 posLight     = float3(0.0, 10.0, 0.0);
    //float3 colorLight   = float3(23.47, 21.31, 20.79);
    o = Pbr_basic_lighting(surface, posView, posLight, colorLight);

    float3 kAmbient = float3(0.03);
    //float3 albedo   = float3(1.0);
    //float  ao       = 0.2;
    float3 ambient  = kAmbient * albedo * ao;

    o = ToneMapping_reinhard(o);
    o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}