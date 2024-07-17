#if 0
Shader {
	Properties {
		Color4f  	RDS_MATERIAL_PROPERTY_baseColor	= {1.0, 1.0, 1.0, 1.0}
		Color4f  	RDS_MATERIAL_PROPERTY_emission	= {1.0, 1.0, 1.0, 1.0}
		Float   	RDS_MATERIAL_PROPERTY_metalness	= 0.0
		Float   	RDS_MATERIAL_PROPERTY_roughness	= 0.0

		Color4f 	ambient				= {1.0, 1.0, 1.0, 1.0}
		Color4f 	diffuse				= {1.0, 1.0, 1.0, 1.0}
		Color4f 	specular			= {1.0, 1.0, 1.0, 1.0}
		Float		ambientOcclusion	= 0.0

		Texture2D 	RDS_MATERIAL_TEXTURE_baseColor
		Texture2D 	RDS_MATERIAL_TEXTURE_normal
		Texture2D 	RDS_MATERIAL_TEXTURE_roughnessMetalness
		Texture2D 	RDS_MATERIAL_TEXTURE_emission
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
	float3 normal   	: NORMAL;
	float3 positionWs   : TEXCOORD1;
	float3 tangent		: TEXCOORD2;
};

struct PixelOut
{
    float4 normal				: SV_Target0;
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
    DrawParam       drawParam = rds_DrawParam_get();

	o.positionHcs 	= SpaceTransform_objectToClip( input.positionOs, 	drawParam);
	o.uv 			= input.uv;
    o.positionWs  	= SpaceTransform_objectToWorld(input.positionOs, 	objTransf);
    o.normal 	  	= SpaceTransform_toWorldNormal(input.normal, 		objTransf);
    o.tangent   	= SpaceTransform_toWorldNormal(input.tangent, 		objTransf);

    return o;
}

PixelOut ps_main(PixelIn input)
{
	PixelOut o = (PixelOut)0;

	float2 uv = input.uv;

	float3 pos;
	float3 normal;
	float3 tangent;

	pos		= input.positionWs;
	normal 	= normalize(input.normal.xyz);
	tangent = normalize(input.tangent.xyz);
	
    Surface surface = Material_makeSurface(uv, pos, normal, tangent);
	//surface = Material_makeSurface(uv, pos, normal);

	if (surface.baseColor.a < rds_alphaCutOff)
		discard;

	// TODO: pack the normal as RG16s
	o.normal.xyz   			= (remapNeg11To01(surface.normal.xyz));		// do not normalize
	o.baseColor   			= surface.baseColor;
	o.roughnessMetalness 	= Material_packRoughnessMetalness(surface.roughness, surface.metalness);
	o.emission 				= surface.emission;

	o.position = float4(input.positionWs.xyz, 1.0);

    return o;
}