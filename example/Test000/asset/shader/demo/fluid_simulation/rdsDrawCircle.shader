#if 0
Shader {
	Properties {
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		Texture2D 	texture0
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

//		DepthTest	Always
//		DepthWrite	false

//		DepthWrite	false		// pre_depth
		Wireframe false

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
    float4 positionOS  			: SV_POSITION;
	float4 color 				: COLOR;
    float2 uv           		: TEXCOORD0;
    float3 centerAndRadius      : NORMAL0;
    //float2 fadeAndThickness     : TEXCOORD2;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float4 color 		: COLOR;
    float2 uv           : TEXCOORD0;
	float  radius		: TEXCOORD1;
	float2 quadPosOs	: TEXCOORD2;

};

//RDS_TEXTURE_2D(texture0);
//float 	raduis;
//float2 	center;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
	o.positionHCS = mul(RDS_MATRIX_VP, i.positionOS);
	o.color		  = i.color;
    o.uv          = i.uv;
	o.radius	  = i.centerAndRadius.z;
	o.quadPosOs	  = (i.positionOS.xy - i.centerAndRadius.xy) / o.radius;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	float4 o = {0.0, 0.0, 0.0, 1.0};

	float2  center			= float2(0.5, 0.5);
	float2  uv				= i.uv;
	float2  resolution 		= rds_DrawParam_get().resolution;
	uv.x 				   *= resolution.x / resolution.y;
    float2 	relativePos 	= (uv) - center;		// uv.x * aspect_ratio 
    //float 	dist 			= 1.0 - length(uv); //length(relativePos);
	// only use uv also ok, no need quadPosOs
	float dist 				= 0.5 - length(i.quadPosOs); //dot() //dist <= center.x;

	float fade = 0.005;
	float thickness = 1.0;

    float circle = smoothstep(0.0, fade, dist);
    circle *= smoothstep(thickness + fade, thickness, dist);

	if (circle == 0.0)
		discard;

	o.rgb = i.color.rgb;
	o.a   = circle;
	return o;

	float4 white = {1.0, 1.0, 1.0, 1.0};
    return o;
}