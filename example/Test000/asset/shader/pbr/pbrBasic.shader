#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	Less

//		DepthTest	Always
//		DepthWrite	false

//		Wireframe true

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

//#include "common/rdsCommon.hlsl"
#include "rdsPbrCommon.hlsl"

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

    float3 colorSpec;
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    o.positionHCS = mul(rds_matrix_mvp,     i.positionOS);
    o.positionWS  = mul(rds_matrix_model,   i.positionOS);
    o.normal      = normalize(mul((float3x3)rds_matrix_model, i.normal));
    o.uv          = i.uv;
    
    o.uv.x = roughness;     // temp solution for match same descriptor set layout
    o.uv.x = i.uv.x;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o = float3(1.0, 1.0, 1.0);

    o.r = rds_camera_pos.x;

    rds_Surface surface;
    surface.posWS       = i.positionWS.xyz;
    surface.normal      = normalize(i.normal);
    surface.color       = albedo;
    surface.roughness   = roughness;
    surface.metallic    = metallic;

    float3 posView = rds_camera_pos;
    float3 dirView  = normalize(posView - surface.posWS);

    o = Pbr_basic_lighting(surface, dirView, posLight, colorLight);

    float3 kAmbient = float3(0.03, 0.03, 0.03);
    float3 ambient  = kAmbient * albedo * ao;

    o = ambient + o;

    #if 0
    float3 dirView  = normalize(posView  - i.positionWS);
    float3 dirLight = normalize(posLight - i.positionWS);
    float3 dirHalf  = normalize(dirView  + dirLight);

    float3 diffuse  = max(dot(i.normal, dirLight), 0.0) * colorLight;
    float3 spec     = pow(max(dot(i.normal, dirHalf), 0.0), roughness) * colorSpec;
    o = ambient + diffuse + spec;
    //o = spec;
    //o = float3(roughness);
    #endif

    o = ToneMapping_reinhard(o);
    o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}