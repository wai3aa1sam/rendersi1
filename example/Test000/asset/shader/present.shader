#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

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

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

#define RDS_GET_SAMPLER(NAME)   _rds_ ## NAME ## _sampler
#define RDS_GET_TEXTURE2D(NAME) NAME

#define RDS_TEXTURE2D(NAME) \
Texture2D       RDS_GET_TEXTURE2D(NAME) 	: register(t1, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(s1, space1) \
// ---

#define RDS_SAMPLE_TEXTURE2D(TEX, UV) RDS_GET_TEXTURE2D(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

RDS_TEXTURE2D(texture0);

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    o.positionHCS = i.positionOS;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	float4 o = RDS_SAMPLE_TEXTURE2D(texture0, i.uv);
    return o;
}