#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	Less

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}

	Permutation
	{
		//IS_EXCLUDE_ALBEDO = { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
	float3 positionWs   : TEXCOORD1;
	float3 positionVs   : TEXCOORD2;
    float3 normalVs     : NORMAL1;
};

struct PixelOut
{
    float2 normal	: SV_Target0;
	float4 albedo	: SV_Target1;

	// TODO: remove, just for debug
	float4 position	: SV_Target2;
};

PixelIn vs_main(VertexIn input)
{
    PixelIn o = (PixelIn)0;

	ObjectTransform objTransf = rds_ObjectTransform_get();
	DrawParam		drawParam = rds_DrawParam_get();

    o.positionHcs = SpaceTransform_objectToClip(input.positionOs,   drawParam, objTransf);
    o.positionWs  = SpaceTransform_objectToWorld(input.positionOs, 	objTransf).xyz;
    o.normal 	  = SpaceTransform_toWorldNormal(input.normal, 		objTransf);
	o.uv		  = input.uv;

    o.positionVs  = SpaceTransform_objectToView(input.positionOs, 	drawParam, objTransf).xyz;
    o.normalVs	  = SpaceTransform_toViewNormal(input.normal, 		objTransf, drawParam).xyz;

    return o;
}

PixelOut ps_main(PixelIn input)
{
	PixelOut o = (PixelOut)0;

	float2 uv = input.uv;

	//float3 pos 		= input.positionVs.xyz;
	float3 normal 	= normalize(input.normalVs);

	//pos 	= input.positionWs.xyz;
	normal 	= normalize(input.normal.xyz);

	o.normal   = float2(normal.xy);

	#if !IS_EXCLUDE_ALBEDO
	float4 albedo = Material_sampleTexture_Albedo(uv);
    albedo.rgb =  lerp(Material_getProperty_Albedo().rgb, albedo.rgb, albedo.a);
	o.albedo   = float4(albedo.rgb, 1.0);
	#endif

	o.position = float4(input.positionWs.xyz, 1.0);

    return o;
}