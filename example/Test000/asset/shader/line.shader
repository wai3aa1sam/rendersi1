#if 0
Shader {
	Properties {
	}
	
	Pass {
		DepthTest	Always

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn 
{
	float4 positionOs 	: POSITION;
	float4 color 		: COLOR;
};

struct PixelIn 
{
	float4 positionHcs 	: SV_POSITION;
	float4 color 		: COLOR;
};

PixelIn vs_main(VertexIn i) 
{
	PixelIn o;
	o.positionHcs 	= mul(RDS_MATRIX_VP,   i.positionOs);
	o.color	 		= i.color;
	return o;
}

float4 ps_main(PixelIn i) : SV_TARGET 
{
	float4 o = i.color;
	//o = float4(1.0, 1.0, 1.0, 1.0);
	return o;
}