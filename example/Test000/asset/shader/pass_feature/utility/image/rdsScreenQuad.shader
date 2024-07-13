#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

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

/*
references:
~ Screen quad vertex shader with no vertex buffer - [https://github.com/Microsoft/DirectX-Graphics-Samples/issues/211]
*/

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

RDS_TEXTURE_2D(tex_color);

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;

	uint 	vertexId 	= input.vertexId;
	float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionHcs = ScreenQuad_makePositionHcs(uv);
    o.uv          = uv;
    
    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = RDS_TEXTURE_2D_SAMPLE(tex_color, input.uv);
    return o;
}