#if 0
Shader {
	Properties {
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	Always
//		DepthWrite	false

		BlendRGB 	Add SrcAlpha 	OneMinusSrcAlpha
		BlendAlpha	Add One 		OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

struct VertexIn {
	float2 pos : POSITION;
	float4 col : COLOR0;
	float2 uv  : TEXCOORD0;
};

struct PixelIn {
	float4 pos 	: SV_POSITION;
	float4 col 	: COLOR0;
	float2 uv	: TEXCOORD0;
};

float4x4 projectionMatrix; 

Texture2D       texture0 				: register(t1, space1);
SamplerState    _rds_texture0_sampler 	: register(s1, space1);

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

PixelIn vs_main(VertexIn i) 
{
	PixelIn o;
	o.pos = mul(projectionMatrix, float4(i.pos.xy, 0.f, 1.f));
	o.col = i.col;
	o.uv  = i.uv;

	//o.uv.y = -o.uv.y;

	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET 
{
	float4 o = i.col * texture0.Sample(_rds_texture0_sampler, i.uv);	// for ImGui::GetTexDataAsRGBA32
	//o = i.col * texture0.Sample(_rds_texture0_sampler, i.uv).r;		// for ImGui::GetTexDataAsAlpha8

	return o;
	//return float4(Color_Linear_to_sRGB(o), o.a); // use this if VkImageView is SRGB
}
