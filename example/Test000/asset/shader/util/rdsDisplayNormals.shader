#if 0
Shader {
	Properties {
		Float normalScale = 1.0
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	Always
//		DepthWrite	false

//		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		GeomFunc    geom_displayNormals_triangle
		PsFunc		ps_main
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float3 normal   	: NORMAL;
};

struct GeometryIn
{
    float4 positionOs   : POSITION;
    float3 normal   	: NORMAL;
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
    float4 color        : COLOR;
};

float normalScale;

GeometryIn vs_main(VertexIn input)
{
	GeometryIn o = (GeometryIn)0;
    o.positionOs  = input.positionOs;
    o.normal 	  = input.normal;
    
    return o;
}

[maxvertexcount(6)]
void geom_displayNormals_triangle(triangle GeometryIn input[3], inout LineStream<PixelIn> outStream)
{
	float normalLength = 0.1 * normalScale;
	for(int i = 0; i < 3 ; i++)
	{
		float3 posOs 	= input[i].positionOs.xyz;
		float3 normal 	= input[i].normal.xyz;

		PixelIn o = (PixelIn)0;
		o.positionHcs = mul(RDS_MATRIX_MVP, float4(posOs, 1.0));
		//o.positionHcs 	= mul(RDS_MATRIX_PROJ, mul(RDS_MATRIX_MODEL, float4(posOs, 1.0)));
		o.color 		= float4(1.0, 1.0, 0.0, 1.0);
		outStream.Append(o);

		o.positionHcs 	= mul(RDS_MATRIX_MVP, float4(posOs + normal * normalLength, 1.0));
		//o.positionHcs 	= mul(RDS_MATRIX_PROJ, mul(RDS_MATRIX_MODEL, float4(posOs + normal * normalLength, 1.0)));
		o.color 		= float4(1.0, 1.0, 0.0, 1.0);
		outStream.Append(o);

		outStream.RestartStrip();
	}
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(input.color.xyzw);
    return o;
}