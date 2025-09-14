#if 0
Shader {
	Properties {
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
		
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOS  			: SV_POSITION;
	float4 color 				: COLOR;
    float2 uv           		: TEXCOORD0;
    float3 centerAndRadius      : NORMAL0;		// useless, delete later
    //float2 fadeAndThickness     : TEXCOORD2;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float4 color 		: COLOR;
    float2 uv           : TEXCOORD0;
	float  radius		: TEXCOORD1;
	float2 posOs		: TEXCOORD2;

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
	o.posOs	  	  = i.positionOS.xy;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	float2 centreOffset = (i.uv.xy - 0.5);
	float  sqrDist 		= dot(centreOffset, centreOffset);
	
	if (sqrDist > square(0.5))
		discard;

	float dist			= sqrt(sqrDist);
	float delta 		= fwidth(dist);
	float alpha 		= 1 - smoothstep(1 - delta, 1 + delta, sqrDist);

	float3 color = i.color.rgb;
	return float4(color, alpha);
}