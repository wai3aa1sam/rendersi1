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

#include "built-in/shader/common/rdsCommon.hlsl"

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

RDS_TEXTURE_2D(texture0);
float4 color;
bool test_bool; 

PixelIn vs_main(VertexIn i)
{
	DrawParam 		drawParam 	= rds_DrawParam_get();
	ObjectTransform objTrans 	= rds_ObjectTransform_get();
	float4x4 mv 	= mul(drawParam.matrix_view, objTrans.matrix_model);
    float4x4 mvp  	= mul(drawParam.matrix_proj, mv);

    PixelIn o;
	o.positionHCS = mul(mvp, i.positionOS);
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	//float4 o = RDS_TEXTURE_2D_SAMPLE(texture0, i.uv) * color;
	float4 o = RDS_TEXTURE_2D_SAMPLE(texture0, i.uv);

	#if RDS_ENABLE_FEATURE_1
	o.r = 0;
	//o = color;
	#endif

	#if RDS_ENABLE_FEATURE_2
	o.g = 0;
	#endif
	if (test_bool)
	{
		//o.g = 0;
	}
	else
	{
		o = color;
	}

	float4 white = {1.0, 1.0, 1.0, 1.0};
    return o;
}