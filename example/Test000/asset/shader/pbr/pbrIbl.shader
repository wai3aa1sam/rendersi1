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
	float3 positionWS   : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

RDS_TEXTURE_2D_1(brdfLut);
RDS_TEXTURE_CUBE_2(irradianceEnvMap);
RDS_TEXTURE_CUBE_3(prefilteredEnvMap);

static const float s_kMaxLod = 9.0;

cbuffer PbrParam
{
    float   roughness;
    float3  posLight;
    float3  colorLight;
    float3  color;
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
    o.normal      = mul((float3x3)rds_matrix_model, i.normal);
    //o.normal      = mul((float3x3)transpose(inverse(rds_matrix_model)), i.normal);

    o.uv          = i.uv;
    o.uv.y        = 1 - i.uv.y;
    
    o.uv.x = roughness;     // temp solution for match same descriptor set layout
    o.uv.x = i.uv.x;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);
    float2 uv = i.uv;

    o.r = rds_camera_pos.x;

    rds_Surface surface;
    surface.posWS               = i.positionWS;
    surface.normal              = normalize(i.normal);
    surface.color               = color;
    surface.roughness           = roughness;
    surface.metallic            = metallic;
    surface.ambientOcclusion    = ao;

    float3 posView = rds_camera_pos;
    float3 dirView = normalize(posView - surface.posWS);
    float3 dirRefl = reflect(-dirView, surface.normal);

    o = Pbr_basic_lighting(surface, dirView, posLight, colorLight);

    float  dotNV            = max(dot(surface.normal, dirView), 0.0);
    float3 irradiance       = RDS_SAMPLE_TEXTURE_CUBE(irradianceEnvMap, surface.normal);
    float3 prefilteredRefl  = RDS_TEXTURE_CUBE_SAMPLE_LOD(prefilteredEnvMap, dirRefl, surface.roughness * s_kMaxLod);
    float2 brdf             = RDS_TEXTURE_2D_SAMPLE(brdfLut, float2(dotNV, surface.roughness));

    float3 ambient = Pbr_indirectLighting(surface, irradiance, prefilteredRefl, brdf, dotNV);
    //ambient = float3(0.0);
    o = ambient + o;
    //o = ambient;

    o = ToneMapping_reinhard(o);
    o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}
