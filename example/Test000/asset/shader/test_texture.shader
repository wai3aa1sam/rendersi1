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
		//Cull		None

		DepthTest	LessEqual

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
    //float4 color        : COLOR;
    float2 uv           : TEXCOORD0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;
};

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

#if	0
Texture2D       texture0 ;//: register(t1);       // texture is a reserved word
SamplerState    _rds_texture0_sampler ;//: register(s1);
#else
Texture2D       texture0 				: register(t1, space1);       // texture is a reserved word
SamplerState    _rds_texture0_sampler 	: register(s1, space1);
#endif

Texture2D       texture2 			;//: register(t2, space0);       // texture is a reserved word
SamplerState    texture2_Sampler 	;//: register(s2, space1);

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = mul(rds_matrix_mvp, i.positionOS);
    //o.positionHCS = i.positionOS;
   // o.color       = i.color;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    //float2 pos2f = i.positionOS;
	float4 o = texture0.Sample(_rds_texture0_sampler, i.uv);
	//o += i.color * texture2.Sample(texture2_Sampler, i.uv);

    o.a = 1;
    return o;
	//return float4(Color_Linear_to_sRGB(o), o.a); // use this if VkImageView is SRGB
}