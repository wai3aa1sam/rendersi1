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

    uint2 blockIndex 			= uint2(floor(input.positionHcs.xy / BLOCK_SIZE));
	uint2 lightBlock 			= RDS_TEXTURE_2D_T_GET(uint2, lightGrid)[blockIndex];
 	uint lightIdxStartOffset 	= lightBlock.x;
    uint lightCount 			= lightBlock.y;

	Surface surface;
	surface = Material_makeSurface(uv, input.positionVs, normalize(input.normalVs));

	const uint nLights = 2;
	Light lights[nLights];
	lights[0].type	 	 = rds_LightType_Point;
	lights[0].positionVs = mul(RDS_MATRIX_VIEW, float4(0.0f, 1.0f, 0.0f, 1.0f));
	lights[0].directionVs= float4(0.0f, -0.698f, -0.7161, 0.0f);
	lights[0].range		 = 4;
	lights[0].intensity	 = 1;
	lights[0].color		 = float4(1.0, 1.0, 1.0, 1.0);

	lights[1].type	 	 = rds_LightType_Point;
	lights[1].positionVs = mul(RDS_MATRIX_VIEW, float4(3.0f, 1.0f, 0.0f, 1.0f));
	lights[1].directionVs= float4(0.0f, -0.698f, -0.7161, 0.0f);
	lights[1].range		 = 4;
	lights[1].intensity	 = 1;
	lights[1].color		 = float4(1.0, 1.0, 1.0, 1.0);

	lightCount = nLights;
	//lightCount = rds_Lights_getLightCount();
	lightCount = nLights;
	LightingResult oLightingResult = (LightingResult)0;
	for (uint i = 0; i < lightCount; ++i)
	{
		uint  iLight 	= RDS_BUFFER_LOAD_I(uint, lightIndexList, lightIdxStartOffset + i);
		Light light 	= rds_Lights_get(iLight);

		// iLight = i;
		// light = lights[iLight];

		LightingResult result = Lighting_computeLighting_Vs(light, surface, drawParam.camera_pos);
		oLightingResult.diffuse 	+= result.diffuse;
		oLightingResult.specular 	+= result.specular;

		if (iLight < 0 || iLight > 3)
		{
			//oLightingResult.diffuse 	+= float4(1.0, 0.0, 0.0, 0.0);
		}

		if (iLight == 0)
		{
			//oLightingResult.diffuse 	+= result.diffuse;
			//oLightingResult.diffuse.xyz 	+= light.color.xyz;
			//oLightingResult.diffuse.xyz 	+= light.positionVs.xyz;

			//oLightingResult.diffuse 	+= float4(0.0, 1.0, 0.0, 0.0);
		}
		//oLightingResult.diffuse.xyz 	+= float3(0.1, 0.1, 0.1).xyz;

		oLightingResult.diffuse 	+= light.positionVs;
	}
	o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;


	//o.rgb = surface.color.rgb;

	//uint  iLight 	= 0;
	//Light light 	= rds_Lights_get(iLight);
	float heatMapColor = (float)lightCount / rds_nLights;
	//o.rgb = float3(heatMapColor, heatMapColor, heatMapColor);
	//o.rgb = float3(lightCount, lightCount, lightCount);

	//o.rgb = lights[0].positionVs.rgb;
	//o.rgb = input.positionVs;
	
    return o;
}