#if 0
Shader {
	Properties {
		Color4f border_color = {0.5, 0.5, 0.5, 1.0}
		Float 	border_width = 0.05

		Float 	voxel_size_scale = 1.0
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

		DepthTest	LessEqual
//		DepthWrite	false

//		Wireframe true

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		GeomFunc    geom_main
		PsFunc		ps_main
	}
}
#endif

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/

#include "rdsVct_Common.hlsl"

struct VertexIn
{
    uint vertexId     : SV_VertexID;
};

struct GeometryIn
{
	uint vertexId     : TEXCOORD0;
	
	RDS_DECLARE_PT_SIZE(ptSize);
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
	float2 uv			: TEXCOORD0;
	float4 color		: COLOR;
};

float 	border_width;
float4 	border_color;

float 	voxel_size_scale;

uint visualize_level;

RDS_TEXTURE_3D_T(float4, voxel_tex_radiance);
	
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_x);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_y);
RDS_TEXTURE_3D_T(float4, voxel_tex_pos_z);
RDS_TEXTURE_3D_T(float4, voxel_tex_neg_z);
	
void createQuad(inout TriangleStream<PixelIn> outStream, float4 vtx0, float4 vtx1, float4 vtx2, float4 vtx3, float4 color)
{
	PixelIn o = (PixelIn)0;
	o.color	= color;

	o.positionHcs 	= vtx0;
	o.uv 			= float2(0.0, 1.0);
	outStream.Append(o);

	o.positionHcs 	= vtx2;
	o.uv 			= float2(0.0, 0.0);
	outStream.Append(o);

	o.positionHcs 	= vtx1;
	o.uv 			= float2(1.0, 1.0);
	outStream.Append(o);
    outStream.RestartStrip();

	o.positionHcs 	= vtx1;
	o.uv 			= float2(1.0, 1.0);
	outStream.Append(o);

	o.positionHcs 	= vtx2;
	o.uv 			= float2(0.0, 0.0);
	outStream.Append(o);
	
	o.positionHcs 	= vtx3;
	o.uv 			= float2(1.0, 0.0);
	outStream.Append(o);
    outStream.RestartStrip();
}

float4 toWorldVoxel(float3 v, VoxelClipmap clipmap)
{
	float voxelScale = clipmap.voxelSize * voxel_size_scale;
	float4 o = float4(v, 1.0);
	o.xyz = (o.xyz * voxelScale) + clipmap.center;
	//o = SpaceTransform_objectToWorld(o);
	o.xyz += 0.01;		// prevent z flight
	return o;
}

GeometryIn vs_main(VertexIn input)
{
	GeometryIn o = (GeometryIn)0;
	RDS_SET_PT_SIZE(o.ptSize, 1.0);

	o.vertexId		= input.vertexId;
	//o.color 		= RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, position.xyz, 0);

    return o;
}

[maxvertexcount(36)]
void geom_main(point GeometryIn input[1], inout TriangleStream<PixelIn> outStream)
{
	PixelIn o = (PixelIn)0;

	DrawParam 	drawParam 			= rds_DrawParam_get();

	float 		voxelResolution		= Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	float 		voxelResolutionInv 	= 1.0 / voxelResolution;

	uint3  dimensions 	= uint3(voxel_resolution, voxel_resolution, voxel_resolution);

	uint vertexId		= input[0].vertexId;
	uint visualizeLevel = visualize_level;
	uint clipmapLevel 	= visualize_level;
	uint voxelIndex		= vertexId;

	bool isVisualizeAllLevel = visualizeLevel == VCT_MAX_CLIPMAP_LEVEL;
	if (isVisualizeAllLevel)
	{
		uint voxelCount = dimensions.x * dimensions.y * dimensions.z;
		voxelIndex   = vertexId % voxelCount;
		clipmapLevel = vertexId / voxelCount;
	}

	float3 pos 			= unflatten3D(voxelIndex, dimensions);
	float3 center		= (pos + 0.5) + Vct_kVoxelBorders;
	float3 uvw 			= (center) * voxelResolutionInv;
	float3 posVoxel 	= Vct_clipmapUvwToVoxel(uvw, voxelResolution);

	float3 			imageCoords 	= Vct_computeImageCoords(pos, clipmapLevel, voxelResolution);
	VoxelClipmap 	clipmap 		= Vct_getVoxelClipmap(clipmapLevel);

	// cull if it exist in current level
	// use world space in current level to map in previous level uvw, if it is map in bound, then it must be exist in previous level
	/*
	reference:
	~ https://github.com/turanszkij/WickedEngine
	*/
	if (isVisualizeAllLevel && clipmapLevel > 0)
	{
		VoxelClipmap prevClipmap = Vct_getVoxelClipmap(clipmapLevel - 1);
		float3 p 	= Vct_clipmapUvwToWorld((pos - 0.5 + Vct_kVoxelBorders) / (voxelResolution - 2), clipmap, voxelResolution);
		float3 diff = (p - prevClipmap.center) * voxelResolutionInv / prevClipmap.voxelSize;
		float3 uvw  = remapNeg11To01_Inv_Y(diff);
		if (isInBoundary01(uvw))
			return;
	}

	#if VCT_USE_6_FACES_CLIPMAP
	
	float4 faceColors[6] = {
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(0.0 * voxelResolution, 0.0, 0.0), 0),
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(1.0 * voxelResolution, 0.0, 0.0), 0),
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(2.0 * voxelResolution, 0.0, 0.0), 0),
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(3.0 * voxelResolution, 0.0, 0.0), 0),
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(4.0 * voxelResolution, 0.0, 0.0), 0),
		RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords + float3(5.0 * voxelResolution, 0.0, 0.0), 0),
	};
	if (
		   faceColors[0].a == 0 && faceColors[1].a == 0 && faceColors[2].a == 0 
		&& faceColors[3].a == 0 && faceColors[4].a == 0 && faceColors[5].a == 0
		)
	{
		return;
	}

	//faceColors[0].rgb = faceColors[0].aaa * 100;
	//faceColors[1].rgb = faceColors[1].aaa * 100;
	//faceColors[2].rgb = faceColors[2].aaa * 100;
	//faceColors[3].rgb = faceColors[3].aaa * 100;
	//faceColors[4].rgb = faceColors[4].aaa * 100;
	//faceColors[5].rgb = faceColors[5].aaa * 100;

	#else
	
	color = RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex_radiance, imageCoords, 0);
	float4 faceColors[6] = { color, color, color, color, color, color };
	if (!any(color))
		return;

	#endif
	
	/*
		4 5	
		6 7
			0 1
			2 3
	*/
	float halfExtent = 1.0;
	float4 vtx0 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent,  halfExtent), clipmap), drawParam);
	float4 vtx1 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent,  halfExtent), clipmap), drawParam);
	float4 vtx2 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent,  halfExtent), clipmap), drawParam);
	float4 vtx3 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent,  halfExtent), clipmap), drawParam);
	float4 vtx4 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent, -halfExtent), clipmap), drawParam);
	float4 vtx5 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent, -halfExtent), clipmap), drawParam);
	float4 vtx6 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent, -halfExtent), clipmap), drawParam);
	float4 vtx7 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent, -halfExtent), clipmap), drawParam);
	
	// uv is wrong now
	if (faceColors[0].a != 0.0) { createQuad(outStream, vtx7, vtx3, vtx5, vtx1, faceColors[0]); }		// right 	face
	if (faceColors[1].a != 0.0) { createQuad(outStream, vtx0, vtx2, vtx4, vtx6, faceColors[1]); }		// left 	face
	if (faceColors[2].a != 0.0) { createQuad(outStream, vtx5, vtx1, vtx4, vtx0, faceColors[2]); }		// top 		face
	if (faceColors[3].a != 0.0) { createQuad(outStream, vtx2, vtx3, vtx6, vtx7, faceColors[3]); }		// bottom 	face
	if (faceColors[4].a != 0.0) { createQuad(outStream, vtx0, vtx1, vtx2, vtx3, faceColors[4]); }		// front 	face
	if (faceColors[5].a != 0.0) { createQuad(outStream, vtx7, vtx5, vtx6, vtx4, faceColors[5]); }		// back 	face
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);
	float2 uv 		= input.uv;
	float4 color 	= input.color;

	// if (color.a == 0.0)
	// 	discard;

	o.rgb = color.rgb;

	float isBorder = 1.0 - min((min(uv.x, (min(uv.y, min(1.0 - uv.x, 1.0 - uv.y))))) / border_width, 1.0);
	o.rgb = lerp(color.rgb, border_color.rgb, isBorder);

	//o.rg = uv;
	//o.rgb = float3(1.0, 1.0, 1.0);

	return o;
}