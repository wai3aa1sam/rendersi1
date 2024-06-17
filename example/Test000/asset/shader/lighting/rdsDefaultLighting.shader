#if 0
Shader {
	Properties {
		Color4f  	RDS_MATERIAL_PROPERTY_albedo	= {1.0, 1.0, 1.0, 1.0}
		Color4f  	RDS_MATERIAL_PROPERTY_emission	= {1.0, 1.0, 1.0, 1.0}
		Float   	RDS_MATERIAL_PROPERTY_metalness	= 0.0
		Float   	RDS_MATERIAL_PROPERTY_roughness	= 0.5

		Texture2D 	RDS_MATERIAL_TEXTURE_Albedo
		Texture2D 	RDS_MATERIAL_TEXTURE_Normal
		Texture2D 	RDS_MATERIAL_TEXTURE_RoughnessMetalness
		Texture2D 	RDS_MATERIAL_TEXTURE_Emissive
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

		//DepthTest	LessEqual
		//DepthWrite	false

		//Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}

	Permutation
	{
		//RDS_ENABLE_FEATURE_1 	= { 0, 1, }
		//RDS_ENABLE_FEATURE_2 	= { 0, 1, }
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
	float3 normal   	: NORMAL;
	float3 normalVs   	: TEXCOORD1;
	float3 positionWs   : TEXCOORD2;
	float3 positionVs   : TEXCOORD3;
};

#if 0
#pragma mark --- default_lighting_param ---
#endif // 0
#if 1


#endif

PixelIn vs_main(VertexIn input)
{
    PixelIn o;

    ObjectTransform objTransf = rds_ObjectTransform_get();
    DrawParam       drawParam = rds_DrawParam_get();

	o.positionHcs 	= SpaceTransform_objectToClip(	input.positionOs, 	drawParam);
    o.positionWs  	= SpaceTransform_objectToWorld( input.positionOs, 	objTransf).xyz;
    o.positionVs  	= SpaceTransform_objectToView(  input.positionOs,   drawParam).xyz;
    o.normal 	  	= SpaceTransform_toWorldNormal( input.normal, 		objTransf);
    o.normalVs 	  	= SpaceTransform_toViewNormal(  input.normal, 		objTransf, drawParam);

    o.uv          	= input.uv;
    
	//o.positionHcs 	= input.positionOs;

    return o;
}

//[earlydepthstencil]
float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

    DrawParam       drawParam = rds_DrawParam_get();

	float2 uv = input.uv;
	float3 pos;
	float3 normal;

	pos 	= input.positionVs;
	normal 	= normalize(input.normalVs);
	
	//pos 	= input.positionWs;
	//normal 	= normalize(input.normal);

	{
		Surface surface;
		surface = Material_makeSurface(uv, pos, normal);

		LightingResult oLightingResult = (LightingResult)0;
		oLightingResult = Lighting_computeForwardLighting_Vs(surface, drawParam.camera_pos);

		o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;
		//o.rgb = oLightingResult.specular.rgb;

		//o.rgb = rds_Lights_get(0).directionVs.rgb;
		//o.rgb = remapNeg11To01(surface.normalVs.rgb);
		//o.rgb = remapNeg11To01(normal);
		//o.rgb = oLightingResult.diffuse.rgb;
		//o.rgb = surface.color.rgb;
	}
	
    return o;
}

