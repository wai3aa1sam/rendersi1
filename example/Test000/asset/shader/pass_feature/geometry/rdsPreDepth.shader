#if 0
Shader {
	Properties {
		Color4f  	RDS_MATERIAL_PROPERTY_baseColor	= {1.0, 1.0, 1.0, 1.0}
		Texture2D 	RDS_MATERIAL_TEXTURE_baseColor
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

		//DepthTest	LessEqual

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
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
	float2 uv			: TEXCOORD0;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
	float2 uv			: TEXCOORD0;
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
    DrawParam drawParam = rds_DrawParam_get();
	o.positionHcs 		= SpaceTransform_objectToClip(input.positionOs, 	drawParam);
	o.uv				= input.uv;
    return o;
}

float ps_main(PixelIn input) : SV_TARGET
{
    //float4 color;
    //color = float4(inputpositionHcs.z, inputpositionHcs.z, inputpositionHcs.z, 1.0);
	//float depth = linearizeDepth(input.positionHcs.z, rds_DrawParam_get());

	float4 baseColor = Material_sampleBaseColor(input.uv);
	if (baseColor.a < rds_alphaCutOff)
		discard;
	
	float depth = input.positionHcs.z;
    return depth;
}