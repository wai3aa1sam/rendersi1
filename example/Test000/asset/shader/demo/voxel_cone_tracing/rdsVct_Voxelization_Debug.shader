#if 0
Shader {
	Properties {
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

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

#include "rdsVct_Common.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;

	float3 positionWs   : POSITION;
};

float4x4 	matrix_world;

//RDS_IMAGE_2D(float4, tex2D_albedo);
RDS_TEXTURE_2D(tex2D);
RDS_TEXTURE_3D_T(float4, voxel_tex_albedo);

RDS_TEXTURE_3D_T(float4, voxel_tex_pos_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_z);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_z);

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;
	ObjectTransform objTransf = rds_ObjectTransform_get();
    o.positionWs  = mul(matrix_world, input.positionOs).xyz;
	o.positionHcs = SpaceTransform_worldToClip(float4(o.positionWs, 1.0));
    o.uv		  = input.uv;

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam 	drawParam 	= rds_DrawParam_get();
	float2 		uv 			= input.uv;

	int maxIter = 10;
	for (int i = 0; i < maxIter; ++i)
	{
		o.rgb += RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_albedo, float3(uv, (float)i / maxIter), 0).rgb;
	}

	return o;
}