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

float linearizeDepth(float depth, DrawParam drawParam) 
{
	float near 	= drawParam.camera_near;
	float far 	= drawParam.camera_far;

    float z = depth * 2.0 - 1.0; // back to NDC 
	return depth;
    //return (2.0 * near * far) / (far + near - z * (far - near));	
}

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHcs = mul(RDS_MATRIX_MVP, input.positionOs);
    return o;
}

float ps_main(PixelIn input) : SV_TARGET
{
    //float4 color;
    //color = float4(inputpositionHcs.z, inputpositionHcs.z, inputpositionHcs.z, 1.0);
	//float depth = linearizeDepth(input.positionHcs.z, rds_DrawParam_get());
	float depth = input.positionHcs.z;
    return depth;
}