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
struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;
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

Texture2D       texture0 				: register(t1, space1);       // texture is a reserved word
SamplerState    _rds_texture0_sampler 	: register(s1, space1);

float3 Color_Linear_to_sRGB(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 Color_sRGB_to_Linear(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }

[numthreads(256, 1, 1)]
void cs_main (uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint index = dispatchThreadId.x;  

	Particle particleIn = in_particle_buffer[index];

	out_particle_buffer[index].position = particleIn.position + particleIn.velocity.xy * rds_dt;
	out_particle_buffer[index].velocity = particleIn.velocity;

	// Flip movement at window border
	if ((out_particle_buffer[index].position.x <= -1.0) || (out_particle_buffer[index].position.x >= 1.0)) 
	{
		out_particle_buffer[index].velocity.x = -out_particle_buffer[index].velocity.x;
	}
	if ((out_particle_buffer[index].position.y <= -1.0) || (out_particle_buffer[index].position.y >= 1.0)) 
	{
		out_particle_buffer[index].velocity.y = -out_particle_buffer[index].velocity.y;
	}
}

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = mul(rds_matrix_mvp, i.positionOS);
    //o.positionHCS = i.positionOS;
    o.color       = i.color;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    //float2 pos2f = i.positionOS;
	float4 o = texture0.Sample(_rds_texture0_sampler, i.uv);
	//o += i.color * texture2.Sample(texture2_Sampler, i.uv);

    o.a = 1;
    return o;
	//return float4(Color_Linear_to_sRGB(o), o.a); // use this if VkImageView is SRGB
}
