#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

		DepthTest	Less

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
};

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHcs = mul(RDS_MATRIX_MVP, input.positionOs);
    return o;
}

float ps_main(PixelIn input) : SV_TARGET
{
    //float4 color;
    //color = float4(input.positionHcs.z, input.positionHcs.z, input.positionHcs.z, 1.0);
    return input.positionHcs.z;
}