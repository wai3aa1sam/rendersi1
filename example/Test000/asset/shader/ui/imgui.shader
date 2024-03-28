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

#include "../common/rdsCommon.hlsl"

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

RDS_TEXTURE_2D(fontTex);

PixelIn vs_main(VertexIn i) 
{
	PixelIn o;
	o.pos = mul(rds_matrix_proj, float4(i.pos.xy, 0.f, 1.f));
	o.col = i.col;
	o.uv  = i.uv;
	//o.uv.y = -o.uv.y;		// flip viewport in application side

	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET 
{
	float2 uv = i.uv;
	//uv.y = -uv.y;
	//float4 o = i.col * RDS_TEXTURE_2D_SAMPLE(fontTex, uv);	// for ImGui::GetTexDataAsRGBA32
	float4 o = i.col * (rds_texture2DTable[NonUniformResourceIndex(rds_perObjectParam.id)].Sample(RDS_SAMPLER_GET(fontTex), uv));	// for ImGui::GetTexDataAsRGBA32

	//o = i.col * texture0.Sample(_rds_texture0_sampler, i.uv).r;		// for ImGui::GetTexDataAsAlpha8

	return o;
	//return float4(Color_Linear_to_sRGB(o), o.a); // use this if VkImageView is SRGB
}
