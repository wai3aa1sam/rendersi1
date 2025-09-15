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

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
	uint   instanceId 	: SV_InstanceID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float2 uv           : TEXCOORD0;
	float4 color 		: TEXCOORD1;

    float2 posOs   		: TEXCOORD2;
};

RDS_TEXTURE_2D(u_colorMap);
RDS_BUFFER(float2, u_positions);
RDS_BUFFER(float2, u_velocities);

float 	u_scale;
float 	u_velocityMax;

float4x4 u_objToWorld;
float4x4 u_worldToObj;
float4 	u_colour;
float 	u_depth;

PixelIn vs_main(VertexIn i)
{
	uint instanceId = i.instanceId;

	float speed = length(RDS_BUFFER_LOAD_I(float2, u_velocities, instanceId));
	float speedT = saturate(speed / u_velocityMax);
	float colT = speedT;
	
	float3 centreWorld 		= float3(RDS_BUFFER_LOAD_I(float2, u_positions, instanceId), 0);
	float3 posWs 			= centreWorld + mul(u_objToWorld, float4(i.positionOS.xyz * u_scale, 1.0)).xyz;
	//float4 posOs 			= mul(u_worldToObj, float4(posWs.xyz, 1));
	float4 color			= RDS_TEXTURE_2D_SAMPLE_LOD(u_colorMap, float2(colT, 0.5), 0);

	//posOs.z = u_depth;
	
    PixelIn o;
	o.positionHCS = mul(RDS_MATRIX_VP, float4(posWs, 1.0));
    o.uv          = i.uv;
	o.color		  = color;
	o.posOs		  = i.positionOS.xy;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	// for debug purpose, end of the position, later should use indirect draw to fix it
	if (all(i.posOs.xy == float2(9999, 9999)))	
		discard;

	float2 centreOffset = (i.uv.xy - 0.5);
	float  sqrDist 		= dot(centreOffset, centreOffset);
	
	if (sqrDist > square(0.5))
		discard;

	float dist			= sqrt(sqrDist);
	float delta 		= fwidth(dist);
	float alpha 		= 1 - smoothstep(1 - delta, 1 + delta, sqrDist);

	float3 color = i.color.rgb;
	return float4(color, i.color.a * alpha);
}