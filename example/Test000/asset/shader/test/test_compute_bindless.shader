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

#include "common/rdsCommon.hlsl"

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

RDS_BUFFER(Particle, in_particle_buffer);
RDS_RW_BUFFER(Particle, out_particle_buffer);

//StructuredBuffer	<Particle> in_particle_buffer;
//RWStructuredBuffer	<Particle> out_particle_buffer;

float		rds_dt;

[numthreads(256, 1, 1)]
void cs_main (uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint index = dispatchThreadId.x;  
	Particle particleIn = RDS_BUFFER_LOAD_I(Particle, in_particle_buffer, index);
	
	#if 1

	Particle outParticle;
	outParticle.position 	= particleIn.position + particleIn.velocity.xy * rds_dt;
	outParticle.velocity 	= particleIn.velocity;
	outParticle.color 		= particleIn.color;
	// Flip movement at window border
	if ((outParticle.position.x <= -1.0) || (outParticle.position.x >= 1.0)) 
	{
		outParticle.velocity.x = -outParticle.velocity.x;
	}
	if ((outParticle.position.y <= -1.0) || (outParticle.position.y >= 1.0)) 
	{
		outParticle.velocity.y = -outParticle.velocity.y;
	}

	#else

	Particle outParticle;
	outParticle.position 	= particleIn.position;
	outParticle.velocity 	= particleIn.velocity;
	outParticle.color 		= particleIn.color;
	
	#endif

	RDS_RW_BUFFER_STORE_I(Particle, out_particle_buffer, index, outParticle);
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
