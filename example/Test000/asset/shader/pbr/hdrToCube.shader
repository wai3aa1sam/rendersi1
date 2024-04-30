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

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float3 positionOS	: POSITION;
};

RDS_TEXTURE_2D(equirectangularMap);

float2 sampleSphericalMap(float3 v)
{
	const float2 invAtan = float2(0.1591, 0.3183);

    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    o.positionHCS   = mul(RDS_MATRIX_MVP, i.positionOS);
    o.positionOS 	= i.positionOS.xyz;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o;
	
	float2 uv = sampleSphericalMap(normalize(i.positionOS));
    o = RDS_TEXTURE_2D_SAMPLE(equirectangularMap, uv).rgb;

    return float4(o, 1.0);
}