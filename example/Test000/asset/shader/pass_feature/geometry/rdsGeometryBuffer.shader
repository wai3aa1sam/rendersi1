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
    float3 tangent      : TANGENT;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
	float3 positionWs   : TEXCOORD1;
	float3 positionVs   : TEXCOORD2;
    float3 normal       : TEXCOORD3;
    float3 normalVs     : TEXCOORD4;
    float3 tangentWs    : TEXCOORD5;
    float3 tangentVs    : TEXCOORD6;
};

struct PixelOut
{
    float2 normal				: SV_Target0;
	float4 baseColor			: SV_Target1;
	float4 roughnessMetalness	: SV_Target2;
	float4 emission				: SV_Target3;

	// TODO: remove, just for debug
	float4 position	: SV_Target4;
};

PixelIn vs_main(VertexIn input)
{
    PixelIn o = (PixelIn)0;

	ObjectTransform objTransf = rds_ObjectTransform_get();
	DrawParam		drawParam = rds_DrawParam_get();

    o.positionHcs = SpaceTransform_objectToClip(input.positionOs,   drawParam, objTransf);
	o.uv		  = input.uv;

    o.positionWs  = SpaceTransform_objectToWorld(input.positionOs, 	objTransf).xyz;
    o.positionVs  = SpaceTransform_objectToView(input.positionOs, 	drawParam, objTransf).xyz;

    o.normal 	  = SpaceTransform_toWorldNormal(input.normal, 		objTransf);
    o.normalVs	  = SpaceTransform_toViewNormal( input.normal, 		objTransf, drawParam).xyz;

    o.tangentWs   = SpaceTransform_toWorldNormal(input.tangent, 	objTransf);
	o.tangentVs   = SpaceTransform_toViewNormal( input.tangent,		objTransf, drawParam);

    return o;
}

PixelOut ps_main(PixelIn input)
{
	PixelOut o = (PixelOut)0;

	float2 uv = input.uv;

	float3 pos;
	float3 normal;
	float3 tangent;

	//float3 pos 		= input.positionVs.xyz;
	normal 	= normalize(input.normalVs);

	//pos 	= input.positionWs.xyz;
	normal 	= normalize(input.normal.xyz);
	tangent = normalize(input.tangentWs.xyz);
	
    Surface surface = Material_makeSurface(uv, pos, normal, tangent);
	o.normal   				= surface.normal.xy;
	o.baseColor   			= surface.baseColor;
	o.roughnessMetalness 	= float4(surface.roughness, surface.metalness, 1.0, 1.0);
	o.emission 				= surface.emission;

	o.position = float4(input.positionWs.xyz, 1.0);

    return o;
}