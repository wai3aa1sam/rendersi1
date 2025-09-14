#if 0
Shader {
	Properties {
		[DisplayName="Color"]
		Color4f	u_color = {1,1,1,1}
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

//		DepthTest	Always
//		DepthWrite	false

//		DepthWrite	false		// pre_depth
		Wireframe true

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

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


float4 u_color;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
	o.positionHCS = mul(RDS_MATRIX_MVP, i.positionOS);
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	//float4 o = RDS_TEXTURE_2D_SAMPLE(texture0, i.uv) * color;
	float4 o = 0;
	o = u_color;
    return o;
}