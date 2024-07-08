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

#include "built-in/shader/rds_shader.hlsl"
#include "built-in/shader/lighting/rdsPbrLighting.hlsl"

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
	float3 positionWs   : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};

RDS_TEXTURE_2D(tex_brdfLut);
RDS_TEXTURE_CUBE(cube_irradianceEnv);
RDS_TEXTURE_CUBE(cube_prefilteredEnv);

static const float s_kMaxLod = 9.0;

float   roughness;
float3  posLight;
float3  colorLight;
float3  color;
float   ao;
float   albedo;
float   metalness;
float3  colorSpec;

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHcs = mul(RDS_MATRIX_MVP,     input.positionOs);
    o.positionWs  = mul(RDS_MATRIX_MODEL,   input.positionOs).xyz;
    o.normal      = mul((float3x3)RDS_MATRIX_MODEL, input.normal);
    //o.normal      = mul((float3x3)transpose(inverse(rds_matrix_model)), input.normal);

    o.uv          = input.uv;
    o.uv.y        = 1 - input.uv.y;
    
    o.uv.x = roughness;     // temp solution for match same descriptor set layout
    o.uv.x += input.uv.x;
    o.uv.x -= roughness;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    DrawParam drawParam = rds_DrawParam_get();

    float3 o = float3(0.0, 0.0, 0.0);
    float2 uv = input.uv;

    Surface surface;
    surface.pos                 = input.positionWs;
    surface.normal              = normalize(input.normal);
    surface.baseColor.rgb           = color;
    surface.roughness           = roughness;
    surface.metalness            = metalness;
    surface.ambientOcclusion    = ao;

    float3 normal = surface.normal;

    float3 posView = drawParam.camera_pos;
    float3 viewDir = normalize(posView - surface.pos);
    float3 dirRefl = reflect(-viewDir, normal);

    o = Pbr_computeDirectLighting(surface, viewDir, posLight, colorLight);

    float  dotNV            = max(dot(normal, viewDir), 0.0);
    float3 irradiance       = RDS_TEXTURE_CUBE_SAMPLE(cube_irradianceEnv, normal).rgb;
    float3 prefilteredRefl  = RDS_TEXTURE_CUBE_SAMPLE_LOD(cube_prefilteredEnv, dirRefl, surface.roughness * s_kMaxLod).rgb;
    float2 brdf             = RDS_TEXTURE_2D_SAMPLE(tex_brdfLut, float2(dotNV, surface.roughness)).rg;

    LightingResult indirect = Pbr_computeIndirectLighting(surface, irradiance, prefilteredRefl, brdf, dotNV);
    //ambient = float3(0.0);
    o = indirect.diffuse.rgb + indirect.specular.rgb + o;
    //o = ambient;

    o = ToneMapping_reinhard(o);
    o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}
