#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		Front

		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

//		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHcs  : SV_POSITION;
};

float4x4 light_vp;

PixelIn vs_main(VertexIn input)
{
    PixelIn o;
    o.positionHcs = float4(SpaceTransform_objectToWorld(input.positionOs), 1.0);
    o.positionHcs = mul(light_vp, o.positionHcs);

	// vulkan reversed viewport
	#if RDS_SHADER_VULKAN
	o.positionHcs.y = -o.positionHcs.y;
	#endif
    return o;
}

void ps_main(PixelIn input) : SV_TARGET
{
	//float depth = input.positionHcs.z;
    //return depth;
}

