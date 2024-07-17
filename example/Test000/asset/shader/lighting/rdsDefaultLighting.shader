#if 0
Shader {
	Properties {
		Color4f  	RDS_MATERIAL_PROPERTY_baseColor	= {1.0, 1.0, 1.0, 1.0}
		Color4f  	RDS_MATERIAL_PROPERTY_emission	= {1.0, 1.0, 1.0, 1.0}
		Float   	RDS_MATERIAL_PROPERTY_metalness	= 0.0
		Float   	RDS_MATERIAL_PROPERTY_roughness	= 0.5

		Color4f 	ambient				= {1.0, 1.0, 1.0, 1.0}
		Color4f 	diffuse				= {1.0, 1.0, 1.0, 1.0}
		Color4f 	specular			= {1.0, 1.0, 1.0, 1.0}
		Float		ambientOcclusion	= 0.0

		Texture2D 	RDS_MATERIAL_TEXTURE_baseColor
		Texture2D 	RDS_MATERIAL_TEXTURE_normal
		Texture2D 	RDS_MATERIAL_TEXTURE_roughnessMetalness
		Texture2D 	RDS_MATERIAL_TEXTURE_emission

		Float 		csm_depth_baias = 0.005
		Float		csm_pcf_scale   = 0.75;
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

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
    float3 tangent      : TANGENT;
    //float3 binormal     : BINORMAL;		// adding binormal instead of cross(normal, tangent) solve texture seam problem in assimp
};

struct PixelIn 
{
	float4 		positionHcs  	: SV_POSITION;
    float2 		uv           	: TEXCOORD0;
	float3 		normal   		: NORMAL;
	float3 		positionWs   	: TEXCOORD1;
    float3 		tangent      	: TEXCOORD2;
	float3 		positionVs   	: TEXCOORD3;

	//float3 		normalVs   		: TEXCOORD1;
    //float3 		tangentVs      	: TEXCOORD5;
};

#if 0
#pragma mark --- default_lighting_param ---
#endif // 0
#if 1

static const float s_kMaxLod = 9.0;

RDS_TEXTURE_2D(tex_brdfLut);
RDS_TEXTURE_CUBE(cube_irradianceEnv);
RDS_TEXTURE_CUBE(cube_prefilteredEnv);

float4  ambient;
float4  diffuse;
float4  specular;
float   ambientOcclusion;

#endif

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
  	o.positionVs  	= SpaceTransform_worldToView(  o.positionWs,   		drawParam);

    //o.normalVs 	  	= SpaceTransform_toViewNormal(  input.normal, 		objTransf, drawParam);
	//o.tangentVs   	= SpaceTransform_toViewNormal( input.tangent,		objTransf, drawParam);

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
	float3 tangent;
	float3 viewDir;

	pos 	= input.positionWs;
	normal 	= normalize(input.normal);
	tangent = normalize(input.tangent);

	Surface surface = Material_makeSurface(uv, pos, normal, tangent);
	//surface = Material_makeSurface(uv, pos, normal);
	surface.ambient				= ambient;
	surface.diffuse				= diffuse;
	surface.specular			= specular;
	surface.ambientOcclusion    = ambientOcclusion;
	//surface.metalness = 1.0;
	//surface.roughness = 0.0;
	
	if (surface.baseColor.a < rds_alphaCutOff)
		discard;

	LightingResult oLightingResult = (LightingResult)0;
	{
		oLightingResult = Lighting_computeForwardLighting_Ws(surface, drawParam.camera_pos, viewDir);

		//o.rgb = oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;
		//o.rgb = oLightingResult.specular.rgb;

		//o.rgb = rds_Lights_get(0).directionVs.rgb;
		//o.rgb = remapNeg11To01(surface.normalVs.rgb);
		//o.rgb = remapNeg11To01(normal);
		//o.rgb = oLightingResult.diffuse.rgb;
		//o.rgb = surface.baseColor.rgb;
	}
	
	if (RDS_TEXTURE_2D_SAMPLE(tex_brdfLut, uv).a != 0.0)
    {
		float3 dirRefl 			= reflect(viewDir, normal);
		float  dotNV            = max(dot(surface.normal, viewDir), 0.0);
		float3 irradiance       = RDS_TEXTURE_CUBE_SAMPLE(cube_irradianceEnv, surface.normal).rgb;
		float3 prefilteredRefl  = RDS_TEXTURE_CUBE_SAMPLE_LOD(cube_prefilteredEnv, dirRefl, surface.roughness * s_kMaxLod).rgb;
		float2 brdf             = RDS_TEXTURE_2D_SAMPLE(tex_brdfLut, float2(dotNV, surface.roughness)).rg;

		LightingResult indirectLight = Pbr_computeIndirectLighting(surface, irradiance, prefilteredRefl, brdf, dotNV);
		o.rgb += indirectLight.diffuse.rgb + indirectLight.specular.rgb;
	}

	//float3 lightDir = rds_Lights_get(0).directionWs.xyz;
	//input.positionWs.y = -input.positionWs.y;
	//input.positionVs.y = -input.positionVs.y;
	float shadow = csm_computeShadow(input.positionWs, input.positionVs.z);
	//shadow = 0.0;
	o.rgb += (1.0 - shadow) * oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;

	//o.rgb *= csm_debugCascadeLevel(input.positionVs.z);
	
	//o.rgb = remapNeg11To01(normal);
	//o.rgb = remapNeg11To01(surface.normal);

    return o;
}

