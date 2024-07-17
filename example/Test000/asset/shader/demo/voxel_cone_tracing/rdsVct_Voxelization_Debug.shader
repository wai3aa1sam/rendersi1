#if 0
Shader {
	Properties {
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	Always
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
    uint vertexId       : SV_VertexID;
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;

	float3 positionWs   : POSITION;
};

float4x4 	matrix_world;

//RDS_IMAGE_2D(float4, tex2D_baseColor);
RDS_TEXTURE_2D(tex2D);
RDS_TEXTURE_3D_T(float4, voxel_tex_radiance);

RDS_TEXTURE_3D_T(float4, voxel_tex_pos_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_z);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_z);

PixelIn vs_main(VertexIn input)
{
	PixelIn o = (PixelIn)0;

	uint 	vertexId 	= input.vertexId;
	float2 	uv 			= ScreenQuad_makeUv(vertexId);

    o.positionWs  = mul(matrix_world, ScreenQuad_makePositionHcs(uv)).xyz;
	o.positionHcs = SpaceTransform_worldToClip(float4(o.positionWs, 1.0));
    o.uv          = uv;
	o.positionHcs = ScreenQuad_makePositionHcs(uv);

    return o;
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam 	drawParam 	= rds_DrawParam_get();
	float2 		uv 			= input.uv;

	if (uv.x > 1.0 || uv.y > 1.0)
		discard;

	int maxIter = voxel_resolution;
	#if VCT_USE_6_FACES_CLIPMAP

	for (int i = 0; i < maxIter; ++i)
	{
		float4 color = RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance
			, float3(uv.x * voxel_resolution * Vct_kClipmapFaceCount, uv.y * voxel_resolution * clipmap_maxLevel, (float)i), 0);
		if (color.a != 1.0)
			continue;
		o.rgb += color.rgb;
	}

	#else

	for (int i = 0; i < maxIter; ++i)
	{
		float4 color = RDS_TEXTURE_3D_T_SAMPLE_LOD(float4, voxel_tex_radiance, float3(uv, (float)i / maxIter - 1), 0);
		// linear sample will blend color, it may not be 1.0
		// if (color.a != 1.0)
		// 	continue;
		o.rgb += color.rgb;
	}

	#endif
	
	return o;
}