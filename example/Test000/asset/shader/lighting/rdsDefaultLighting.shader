#if 0
Shader {
	Properties {
		Texture2D 	RDS_MATERIAL_TEXTURE_Albedo
		Texture2D 	RDS_MATERIAL_TEXTURE_Normal
		Texture2D 	RDS_MATERIAL_TEXTURE_RoughnessMetalness
		Texture2D 	RDS_MATERIAL_TEXTURE_Emissive
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

		//DepthTest	LessEqual
		DepthWrite	false

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
	float3 normalVs   	: NORMAL;
	float3 positionWs   : TEXCOORD1;
	float3 positionVs   : TEXCOORD2;
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

	o.positionHcs 	= SpaceTransform_objectToClip(	input.positionOs, 				drawParam);
    o.positionWs  	= SpaceTransform_objectToWorld( input.positionOs, 	            objTransf).xyz;
    o.positionVs  	= SpaceTransform_worldToView(   float4(o.positionWs, 1.0),  	drawParam).xyz;
    //o.normal 	  	= SpaceTransform_toWorldNormal( input.normal, 		            objTransf);
    o.normalVs 	  	= SpaceTransform_toViewNormal(  input.normal, 		            objTransf, drawParam);

    o.uv          	= input.uv;
    
	//o.positionHcs 	= input.positionOs;

    return o;
}

//[earlydepthstencil]
float4 ps_main(PixelIn input) : SV_TARGET
{
	float2 uv = input.uv;
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

    Surface surface;
    surface.posVs               = input.positionVs;
    surface.normalVs            = normalize(input.normalVs);
    surface.color               = float4(1.0, 1.0, 1.0, 1.0);
    surface.roughness           = 0.2/*roughness*/;
    surface.metallic            = 0.0/*metallic*/;
    surface.ambientOcclusion    = 0.2/*ao*/;

	DrawParam drawParam = rds_DrawParam_get();
	float3 posView = drawParam.camera_pos;
	float3 dirView = normalize(posView - input.positionVs);

	LightingResult oLightingResult = (LightingResult)0;

	uint lightCount = rds_nLights;
	for (uint i = 0; i < lightCount; ++i)
	{
		uint  iLight 	= i;
		Light light 	= rds_Lights_get(iLight);

		LightingResult result = Lighting_computeLighting(light, surface, dirView);
		oLightingResult.diffuse 	+= result.diffuse;
		oLightingResult.specular 	+= result.specular;
	}
	o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;

	float4 albedo = Material_sampleTexture_Albedo(uv);
    o.rgb =  lerp(Material_getProperty_Albedo().rgb, albedo.rgb, albedo.a);

	//o.rgb = albedo.rgb;
	//o.rgb = Material_getProperty_Albedo().rgb;
	
    return o;
}