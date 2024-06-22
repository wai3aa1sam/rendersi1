#if 0
Shader {
	Properties {
		Float delta_phi     = 0.025
		Float delta_theta   = 0.025
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

//		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

/*
reference:
~ https://learnopengl.com/PBR/IBL/Diffuse-irradiance
*/

#include "rdsPbr_Common.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float3 positionOs	: POSITION;
};

RDS_TEXTURE_CUBE(cube_env);

float       delta_phi;
float       delta_theta;
float4x4    matrix_vp;

PixelIn vs_main(VertexIn input)
{
    PixelIn o;

    o.positionHCS   = mul(matrix_vp, input.positionOs);
    o.positionOs 	= input.positionOs.xyz;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);

    // tangent space
    float3 normal   = normalize(input.positionOs);
	float3 up       = float3(0.0, 1.0, 0.0);
	float3 right    = normalize(cross(up, normal));
	up = cross(normal, right);

	const float pi_two  = rds_pi * 2.0;
	const float pi_half = rds_pi * 0.5;

	float3  irradiance   = float3(0.0, 0.0, 0.0);
	uint    sampleCount  = 0u;
	for (float phi = 0.0; phi < pi_two; phi += delta_phi) 
    {
		for (float theta = 0.0; theta < pi_half; theta += delta_theta) 
        {
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // spherical to cartesian (in tangent space)
            float3 cartesianCoord = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
            // tangent space to world
            float3 sampleDir    = cartesianCoord.x * right + cartesianCoord.y * up + cartesianCoord.z * normal; 

            irradiance += RDS_TEXTURE_CUBE_SAMPLE(cube_env, sampleDir).rgb * cosTheta * sinTheta;
			sampleCount++;
		}
	}
    irradiance = irradiance * (rds_pi / sampleCount);
    
    o = irradiance;
    
    return float4(o, 1.0);
}