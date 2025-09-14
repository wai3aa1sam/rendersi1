#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		Front

//		DepthTest	Always
//		DepthWrite	false

//		DepthWrite	false		// pre_depth
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

#include "built-in/shader/rds_shader.hlsl"
#include "rdsFluidSim3D_Common.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL;
	uint   instanceId 	: SV_InstanceID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float2 uv           : TEXCOORD0;
	float4 color 		: TEXCOORD1;
    float3 posOs   		: TEXCOORD2;
    float3 normal   	: NORMAL;
};

RDS_TEXTURE_2D(u_colorMap);
RDS_BUFFER(float3, u_positions);
RDS_BUFFER(float3, u_velocities);

float 	u_scale;
float 	u_velocityMax;
float4 	u_colour;
float   u_depth;	// useless, only use in 2d

float4x4 u_objToWorld;
//float4x4 u_worldToObj;

PixelIn vs_main(VertexIn i)
{
	uint instanceId = i.instanceId;

	float speed = length(RDS_BUFFER_LOAD_I(float3, u_velocities, instanceId));
	float speedT = saturate(speed / u_velocityMax);
	float colT = speedT;
	
	float3 centerWs 		= float3(RDS_BUFFER_LOAD_I(float3, u_positions, instanceId));

	// incorrect
	//float3 posWs 			= centerWs + mul(u_objToWorld, float4(i.positionOS.xyz * u_scale, 1.0)).xyz;
	float3 posWs 			= centerWs + mul(u_objToWorld, i.positionOS * u_scale).xyz;

	//float4 posOs 			= mul(u_worldToObj, float4(posWs.xyz, 1));

	float4 color			= RDS_TEXTURE_2D_SAMPLE_LOD(u_colorMap, float2(colT, 0.5), 0);

    PixelIn o;
	o.positionHCS = mul(RDS_MATRIX_VP, float4(posWs, 1.0));
    o.uv          = i.uv;
	o.color		  = color;
	o.posOs		  = i.positionOS.xyz;
	o.normal	  = i.normal;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	// for debug purpose, end of the position, later should use indirect draw to fix it
	if (all(i.posOs.xyz == s_kInvalid_position))	
		discard;

	float4 o_color = float4(i.color.rgb , 1);

	float3 normal = normalize(i.normal);
	float shading = saturate(dot(rds_DrawParam_get().camera_pos, normal));
	shading = (shading + 0.6) / 1.4;

	o_color.rgb *= shading;
	
	return o_color;
}