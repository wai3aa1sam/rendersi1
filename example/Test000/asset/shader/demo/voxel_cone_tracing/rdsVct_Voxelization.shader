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

		BlendRGB 	Disable One OneMinusSrcAlpha
		BlendAlpha	Disable One OneMinusSrcAlpha
		
		VsFunc		vs_main
		GeomFunc    geom_main
		PsFunc		ps_main
	}
}
#endif

#include "rdsVct_Common.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;
};

struct GeometryIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;

    float3 positionWs   : POSITION;
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;

	float3 positionWs   : POSITION;
	float4 positionCs   : TEXCOORD1;
	float3 posVoxel   	: TEXCOORD2;
};

float4x4 voxel_matrix_vp_x;
float4x4 voxel_matrix_vp_y;
float4x4 voxel_matrix_vp_z;
float4x4 rds_matrix_vp;
float4x4 voxel_matrix_proj;

RDS_IMAGE_3D(uint, voxel_tex_albedo);


GeometryIn vs_main(VertexIn input)
{
	GeometryIn o = (GeometryIn)0;
	ObjectTransform objTransf = rds_ObjectTransform_get();
    o.positionWs  	= SpaceTransform_objectToWorld(input.positionOs, 	objTransf).xyz;
    o.normal 	  	= SpaceTransform_toWorldNormal(input.normal, 		objTransf);
    o.uv		  	= input.uv;

	//o.positionHcs  	= mul(voxel_matrix_proj, float4(o.positionWs.xyz, 1.0));
	o.positionHcs	= float4(o.positionWs.xyz, 1.0);

	//o.positionHcs = SpaceTransform_worldToClip(float4(o.positionWs, 1.0));
	//o.positionHcs = input.positionOs;

    return o;
}

[maxvertexcount(3)]
void geom_main(triangle GeometryIn input[3], inout TriangleStream<PixelIn> outStream)
{
	//int axisIdx = getDominantAxisIdx(input[0].positionWs.xyz, input[1].positionWs.xyz, input[2].positionWs.xyz);
	DrawParam drawParam = rds_DrawParam_get();
	float voxelResolutionInv = 1.0 / voxel_resolution;
	
	float3 vtx0 = input[0].positionHcs.xyz;
	float3 vtx1 = input[1].positionHcs.xyz;
	float3 vtx2 = input[2].positionHcs.xyz;
	float3 n 	= abs(normalize(cross(vtx1 - vtx0, vtx2 - vtx0)));
	float  axis = max(n.x, max(n.y, n.z));

	PixelIn output[3];
	for(int i = 0; i < 3 ; i++)
	{
		PixelIn o = (PixelIn)0;
		o.positionCs  = input[i].positionHcs;
		o.normal	  = input[i].normal;
		o.uv 		  = input[i].uv;
		o.positionWs  = input[i].positionWs.xyz;
		o.posVoxel	  = (input[i].positionWs - clipmap_center) * voxelResolutionInv / voxel_size; 	// to voxel grid space

		bool isDominantAxisX = n.x > n.y && n.x > n.z;
		bool isDominantAxisY = n.y > n.z;

		isDominantAxisX = axis == n.x;
		isDominantAxisY = axis == n.y;

		if (isDominantAxisX)
		{
			o.positionHcs = float4(o.posVoxel.zy, 0.0, 1.0);
		}
		else if (isDominantAxisY)
		{
			o.positionHcs = float4(o.posVoxel.xz, 0.0, 1.0);
		}
		else
		{
			o.positionHcs = float4(o.posVoxel.xy, 0.0, 1.0);
		}
		//o.positionHcs.z = 1.0;
		//o.positionHcs = SpaceTransform_worldToClip(float4(input[i].positionWs.xyz, 1.0));
		
		output[i] = o;
	}

	#if	0
	// Conservative Rasterization setup:
	float2 side0N = normalize(output[1].positionHcs.xy - output[0].positionHcs.xy);
	float2 side1N = normalize(output[2].positionHcs.xy - output[1].positionHcs.xy);
	float2 side2N = normalize(output[0].positionHcs.xy - output[2].positionHcs.xy);
	output[0].positionHcs.xy += normalize(side2N - side0N) * voxelResolutionInv;
	output[1].positionHcs.xy += normalize(side0N - side1N) * voxelResolutionInv;
	output[2].positionHcs.xy += normalize(side1N - side2N) * voxelResolutionInv;
	#endif

	outStream.Append(output[0]);
	outStream.Append(output[1]);
	outStream.Append(output[2]);
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	DrawParam 	drawParam 	= rds_DrawParam_get();
	float3 		posVoxel 	= input.posVoxel;
	float3 		posWs 		= input.positionWs;

	float3 imageCoords = float3(0, 0, 0);
	// if (Vct_isOutsideVoxelRegion(posWs, voxel_region_minPosWs, voxel_region_maxPosWs))
	// 	discard;
	//imageCoords = voxel_resolution * remapNeg11To01(float3(posVoxel.x, posVoxel.y, posVoxel.z));
	//float 	regionExtentWs 	= Vct_getVoxelRegionExtent(voxel_region_minPosWs, voxel_region_maxPosWs).x;
	//imageCoords = Vct_toImageCoords(posWs, regionExtentWs, voxel_resolution, clipmap_level);

	float voxelResolutionInv = 1.0 / voxel_resolution;
	float3 uvw  = Vct_worldToClipmapUvw(posWs - clipmap_center, voxelResolutionInv, voxel_size);
	imageCoords = uvw * voxel_resolution;

	if (!isInBoundary01(uvw))
		discard;

	o.rgb = remapNeg11To01(input.normal);

	AtomicOp_avgRGBA8(RDS_IMAGE_3D_GET(uint, voxel_tex_albedo), uint3(imageCoords), float4(o.rgb, 1.0));

	return o;
}