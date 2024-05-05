#if 0
Shader {
	Properties {
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D 	texture0
		Bool		test_bool = true
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
#pragma mark --- fwpd_lighting_param ---
#endif // 0
#if 1

RDS_BUFFER(uint, lightIndexList);
RDS_TEXTURE_2D_T(uint2, lightGrid);

#endif

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
	o.positionHcs 	= mul(RDS_MATRIX_MVP, 	input.positionOs);
	//o.positionHcs 	= input.positionOs;

	o.positionWs 	= mul(RDS_MATRIX_MODEL, input.positionOs).xyz;
	o.positionVs 	= mul(RDS_MATRIX_VIEW, 	float4(o.positionWs, 1.0)).xyz;
    o.normalVs      = mul(RDS_MATRIX_VIEW,  float4(input.normal, 0.0)).xyz;
    o.uv          	= input.uv;
    
    return o;
}

//[earlydepthstencil]
float4 ps_main(PixelIn input) : SV_TARGET
{
	float2 uv = input.uv;
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

    uint2 blockIndex 			= uint2(floor(input.positionHcs.xy / BLOCK_SIZE));
	uint2 lightBlock 			= RDS_TEXTURE_2D_T_GET(uint2, lightGrid)[blockIndex];
 	uint lightIdxStartOffset 	= lightBlock.x;
    uint lightCount 			= lightBlock.y;
	//lightCount = rds_nLights;

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

	const uint nLights = 2;
	//lightCount = nLights;
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

	LightingResult oLightingResult = (LightingResult)0;
	for (uint i = 0; i < lightCount; ++i)
	{
		uint  iLight 	= RDS_BUFFER_LOAD_I(uint, lightIndexList, lightIdxStartOffset + i);
		//iLight = i;
		Light light 	= rds_Lights_get(iLight);
		light = lights[iLight];

		LightingResult result = Lighting_computeLighting(light, surface, dirView);
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

		//oLightingResult.diffuse 	+= light.positionVs;
	}
	o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;
	//o.rgb = surface.color.rgb;

	//uint  iLight 	= 0;
	//Light light 	= rds_Lights_get(iLight);
	float heatMapColor = (float)lightCount / rds_nLights;
	//o.rgb = float3(heatMapColor, heatMapColor, heatMapColor);
	//o.rgb = float3(lightCount, lightCount, lightCount);

	//o.rgb = light.color.rgb;
	
    return o;
}