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

		DepthTest	LessEqual
		DepthWrite	false

		Wireframe false

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

/*
	references:
	~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "forward_plus_common.hlsl"
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
#pragma mark --- fwpd_lighting_param ---
#endif // 0
#if 1

RDS_BUFFER(uint, lightIndexList);
RDS_TEXTURE_2D_T(uint2, lightGrid);

#endif

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
	
    ObjectTransform objTransf = rds_ObjectTransform_get();
    DrawParam       drawParam = rds_DrawParam_get();

	o.positionHcs 	= SpaceTransform_objectToClip(	input.positionOs, 	drawParam);
	o.uv 			= input.uv;
    o.positionWs  	= SpaceTransform_objectToWorld( input.positionOs, 	objTransf).xyz;
    o.positionVs  	= SpaceTransform_objectToView(  input.positionOs,   drawParam).xyz;
    o.normal 	  	= SpaceTransform_toWorldNormal( input.normal, 		objTransf);
    o.normalVs 	  	= SpaceTransform_toViewNormal(  input.normal, 		objTransf, drawParam);

    return o;
}

//[earlydepthstencil]
float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam drawParam = rds_DrawParam_get();

	float2 uv = input.uv;
	float3 viewDir;

    uint2 blockIndex 			= uint2(floor(input.positionHcs.xy / BLOCK_SIZE));
	uint2 lightBlock 			= RDS_TEXTURE_2D_T_GET(uint2, lightGrid)[blockIndex];
 	uint  lightIdxStartOffset 	= lightBlock.x;
    uint  lightCount 			= lightBlock.y;

	Surface surface = Material_makeSurface(uv, input.positionVs, normalize(input.normalVs));

	LightingResult oLightingResult = (LightingResult)0;
	for (uint i = 0; i < lightCount; ++i)
	{
		uint  iLight 	= RDS_BUFFER_LOAD_I(uint, lightIndexList, lightIdxStartOffset + i);
		Light light 	= rds_Lights_get(iLight);
		
		LightingResult result = Lighting_computeLighting_Vs(light, surface, drawParam.camera_pos, viewDir);
		oLightingResult.diffuse 	+= result.diffuse;
		oLightingResult.specular 	+= result.specular;
	}
	o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;

	//uint  iLight 	= 0;
	//Light light 	= rds_Lights_get(iLight);
	float heatMapColor = (float)lightCount / rds_nLights;
	//o.rgb = float3(heatMapColor, heatMapColor, heatMapColor);
	//o.rgb = float3(lightCount, lightCount, lightCount);

	//o.rgb = lights[0].positionVs.rgb;
	//o.rgb = input.positionVs;
	
    return o;
}