#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D mainTex
		Texture2D heightTex
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

		RenderPrimitiveType Point	// TODO: remove
		//RenderPrimitiveType Triangle	// TODO: remove

		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		CsFunc		cs_main
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#define RDS_DECLARE_PT_SIZE(var) 	[[vk::builtin("PointSize")]] float var : PSIZE
#define RDS_SET_PT_SIZE(var, v) 	var = v

struct VertexIn
{
    float2 positionOS   : SV_POSITION;
    float4 color        : COLOR;
    float2 velocity     : TEXCOORD0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float4 color        : COLOR;
    float2 velocity     : TEXCOORD0;
	RDS_DECLARE_PT_SIZE(ptSize);
};

struct Particle 
{
  float2 position;
  float2 velocity;
  float4 color;
};
StructuredBuffer	<Particle> in_particle_buffer;
RWStructuredBuffer	<Particle> out_particle_buffer;

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

float		rds_dt;

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

[numthreads(256, 1, 1)]
void cs_main (uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint index = dispatchThreadId.x;  
	Particle particleIn = in_particle_buffer[index];

	#if 1

	out_particle_buffer[index].position = particleIn.position + particleIn.velocity.xy * rds_dt;
	out_particle_buffer[index].velocity = particleIn.velocity;
	out_particle_buffer[index].color 	= particleIn.color;

	// Flip movement at window border
	if ((out_particle_buffer[index].position.x <= -1.0) || (out_particle_buffer[index].position.x >= 1.0)) 
	{
		out_particle_buffer[index].velocity.x = -out_particle_buffer[index].velocity.x;
	}
	if ((out_particle_buffer[index].position.y <= -1.0) || (out_particle_buffer[index].position.y >= 1.0)) 
	{
		out_particle_buffer[index].velocity.y = -out_particle_buffer[index].velocity.y;
	}
	#else

	out_particle_buffer[index].position = particleIn.position;
	out_particle_buffer[index].velocity = particleIn.velocity;
	out_particle_buffer[index].color 	= particleIn.color;
	
	#endif
}

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = float4(i.positionOS.xy, 0.5, 1.0);
    //o.positionHCS = i.positionOS;
    o.color       = i.color;
    o.velocity    = i.velocity;

	RDS_SET_PT_SIZE(o.ptSize, 3.0);
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float2 pos2f 	= i.positionHCS.xy;
	float4 o 		= float4(i.color.rgb, 1);
	//o += i.color * texture2.Sample(texture2_Sampler, i.uv);

    return o;
}
