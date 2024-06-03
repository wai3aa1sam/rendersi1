#if 0
Shader {
	Properties {
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D 	texture0
		Bool		test_bool = true
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

//		DepthTest	Always
//		DepthWrite	false

		DepthWrite	false		// pre_depth
		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
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
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};


RDS_TEXTURE_2D(tex2D);

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
	o.positionHcs = SpaceTransform_objectToClip(i.positionOS);
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	//float4 o = RDS_TEXTURE_2D_SAMPLE(tex2D, i.uv) * color;
	float4 o = RDS_TEXTURE_2D_SAMPLE(tex2D, i.uv);

	float4 white = {1.0, 1.0, 1.0, 1.0};
    return o;
}