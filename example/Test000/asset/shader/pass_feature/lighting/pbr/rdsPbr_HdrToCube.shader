#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

//		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

/*
reference:
~ https://learnopengl.com/PBR/IBL/Diffuse-irradiance
*/

#include "rdsPbr_Common.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
	float3 positionOs	: POSITION;
};

RDS_TEXTURE_2D(tex_equirectangular);

float4x4 matrix_vp;

float2 sampleSphericalMap(float3 v)
{
	const float2 invAtan = float2(0.1591, 0.3183);

    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

PixelIn vs_main(VertexIn input)
{
    PixelIn o;

    o.positionHcs   = mul(matrix_vp, input.positionOs);
    o.positionOs 	= input.positionOs.xyz;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    float3 o;
	
	float2 uv = sampleSphericalMap(normalize(input.positionOs));
    o = RDS_TEXTURE_2D_SAMPLE(tex_equirectangular, uv).rgb;

    return float4(o, 1.0);
}