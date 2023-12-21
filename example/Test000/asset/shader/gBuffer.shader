#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D mainTex
		Texture2D heightTex
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	LessEqual

//		DepthTest	Always
		DepthWrite	false

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
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;
};

struct PixelOut
{
	float4 albedo	: SV_Target0;
    float4 normal	: SV_Target1;
    float4 position	: SV_Target2;
};

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = mul(rds_matrix_mvp, i.positionOS);
    o.color       = i.color;
    o.uv          = i.uv;
    
    return o;
}

PixelOut ps_main(PixelIn i)
{
	PixelOut o;
    //float2 pos2f = i.positionOS;

	o.albedo   = i.color;
	o.normal   = i.positionHCS;
	o.position = i.positionHCS;

    return o;
}