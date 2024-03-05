#if 0
Shader {
	Properties {
		
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

#include "../common/rdsCommon.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float3 positionOS	: POSITION;
};

RDS_TEXTURE_CUBE(envCubeMap);
float deltaPhi;
float deltaTheta;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    o.positionHCS   = mul(rds_matrix_mvp, i.positionOS);
    o.positionOS 	= i.positionOS.xyz;

    o.positionOS.x = deltaPhi + i.positionOS.x;
    o.positionOS.x -= deltaPhi;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);
    o.r = rds_matrix_mvp[0][0];
    o.r += deltaPhi; 
    o.r -= rds_matrix_mvp[0][0]; 

    // tangent space
    float3 normal   = normalize(i.positionOS);
	float3 up       = float3(0.0, 1.0, 0.0);
	float3 right    = normalize(cross(up, normal));
	up = cross(normal, right);

	const float pi_two  = rds_pi * 2.0;
	const float pi_half = rds_pi * 0.5;

	float3  irradiance   = float3(0.0, 0.0, 0.0);
	uint    sampleCount  = 0u;
	for (float phi = 0.0; phi < pi_two; phi += deltaPhi) 
    {
		for (float theta = 0.0; theta < pi_half; theta += deltaTheta) 
        {
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // spherical to cartesian (in tangent space)
            float3 cartesianCoord = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
            // tangent space to world
            float3 sampleDir    = cartesianCoord.x * right + cartesianCoord.y * up + cartesianCoord.z * normal; 

            irradiance += RDS_SAMPLE_TEXTURE_CUBE(envCubeMap, sampleDir).rgb * cosTheta * sinTheta;
			sampleCount++;
		}
	}
    irradiance = irradiance * (rds_pi / sampleCount);
    
    o = irradiance;
    
    return float4(o, 1.0);
}