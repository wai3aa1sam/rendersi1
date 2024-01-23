#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	LessEqual

//		DepthTest	Always
		DepthWrite	false

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
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float3 uv           : TEXCOORD0;
};

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

TextureCube     skybox 				    : register(t1, space1);
SamplerState    _rds_skybox_sampler 	: register(s1, space1);

#define RDS_GET_SAMPLER(NAME)   _rds_ ## NAME ## _sampler
#define RDS_GET_TEXTURE_CUBE(NAME) NAME

#define RDS_TEXTURE_CUBE(NAME) \
TextureCube     RDS_GET_TEXTURE_CUBE(NAME) 	: register(t1, space1); \
SamplerState    RDS_GET_SAMPLER(NAME) 	    : register(s1, space1) \
// ---

#define RDS_SAMPLE_TEXTURE_CUBE(TEX, UV) RDS_GET_TEXTURE_CUBE(TEX).Sample(RDS_GET_SAMPLER(TEX), UV)

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    float4x4 matView = rds_matrix_view;
	matView[0][3] = 0.0;
	matView[1][3] = 0.0;
	matView[2][3] = 0.0;
    float4x4 matVp  = mul(rds_matrix_proj, matView);

    o.positionHCS   = mul(matVp, i.positionOS);              // view mat is camera translation only
    o.positionHCS   = o.positionHCS.xyww;
    o.uv            = i.positionOS.xyz;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float4 o;
	float3 uv = i.uv;
	uv.y = -uv.y;
    o = RDS_SAMPLE_TEXTURE_CUBE(skybox, uv);
    o.a = 1.0;
    return o;
}