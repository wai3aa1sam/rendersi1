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

//#include "built-in/shader/rds_shader.hlsl"
#include "rdsPbrCommon.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float4 positionWs   : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

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

RDS_TEXTURE_2D(texture0);
RDS_TEXTURE_2D(texture1);

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHCS = mul(RDS_MATRIX_MVP,     input.positionOs);
    o.positionWs  = mul(RDS_MATRIX_MODEL,   input.positionOs);
    o.normal      = normalize(mul((float3x3)RDS_MATRIX_MODEL, input.normal));
    o.uv          = input.uv;
    
    o.uv.x = roughness;     // temp solution for match same descriptor set layout
    o.uv.x = input.uv.x;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    float3 o = float3(1.0, 1.0, 1.0);
	DrawParam drawParam = rds_DrawParam_get();

    Surface surface;
    surface.pos         = input.positionWs.xyz;
    surface.normal      = normalize(input.normal);
    surface.color.rgb   = albedo;
    surface.roughness   = roughness;
    surface.metallic    = metallic;

    float3 normal = surface.normal;

    float3 posView = drawParam.camera_pos;
    float3 viewDir  = normalize(posView - surface.pos);

    o = Pbr_computeDirectLighting(surface, viewDir, posLight, colorLight);

    float3 kAmbient = float3(0.03, 0.03, 0.03);
    float3 ambient  = kAmbient * albedo * ao;

    o = ambient + o;

    #if 0
    float3 viewDir  = normalize(posView  - input.positionWs);
    float3 dirLight = normalize(posLight - input.positionWs);
    float3 dirHalf  = normalize(viewDir  + dirLight);

    float3 diffuse  = max(dot(normal, dirLight), 0.0) * colorLight;
    float3 spec     = pow(max(dot(normal, dirHalf), 0.0), roughness) * colorSpec;
    o = ambient + diffuse + spec;
    //o = spec;
    //o = float3(roughness);
    #endif

    o = ToneMapping_reinhard(o);
    o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}