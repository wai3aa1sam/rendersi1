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

//		DepthTest	Always
//		DepthWrite	false

//		Wireframe true

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		GeomFunc    geom_main
		PsFunc		ps_main

		RenderPrimitiveType Point	// TODO: remove
	}
}
#endif

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
*/

#include "rdsVct_Common.hlsl"

struct VertexIn
{
    uint vertexId       : SV_VertexID;
};

struct GeometryIn
{
    float4 positionHcs  : SV_POSITION;
	float4 color		: COLOR;

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

RDS_TEXTURE_3D_T(float4, voxel_tex3D);
	
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

float4 toWorldVoxel(float3 v)
{
	float voxelScale = voxel_size * voxel_size_scale;
	float4 o = float4(v, 1.0);
	o.xyz = (o.xyz * voxelScale) + clipmap_center;
	o = SpaceTransform_objectToWorld(o);
	return o;
}


GeometryIn vs_main(VertexIn input)
{
	GeometryIn o = (GeometryIn)0;
	RDS_SET_PT_SIZE(o.ptSize, 1.0);

	uint3 dimensions;
	RDS_TEXTURE_3D_T_GET_DIMENSIONS(float4, voxel_tex3D, dimensions);

	float3 position = (float3)0;
	
	// just like for (x, y, z) to generate [0, width], [0, height], [0, depth] points
	#if 0
	position.x = input.vertexId % dimensions.x;
	position.y = input.vertexId / (dimensions.x * dimensions.x);
	position.z = (input.vertexId / dimensions.x) % dimensions.x;
	o.positionHcs 	= float4(position.xyz - ((dimensions.x) / 2) + 0.5, 1.0);		// [0, dim] -> [-dim / 2, dim / 2]
	#endif
	
	position 		= unflatten3D(input.vertexId, dimensions);
	o.positionHcs 	= float4(position.xyz, 1.0);;
	o.color 		= RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex3D, position.xyz, 0);

    return o;
}

[maxvertexcount(36)]
void geom_main(point GeometryIn input[1], inout TriangleStream<PixelIn> outStream)
{
	PixelIn o = (PixelIn)0;

	DrawParam 	drawParam 			= rds_DrawParam_get();
	float 		voxelResolutionInv 	= 1.0 / voxel_resolution;

	float3 pos 	 = input[0].positionHcs.xyz;
	float4 color = input[0].color;

	float3 center		= (pos + 0.5);
	float3 uvw 			= (center) * voxelResolutionInv;
	float3 posVoxel 	= Vct_clipmapUvwToVoxel(uvw, voxel_resolution);

	float3 imageCoords = pos;
	color = RDS_TEXTURE_3D_T_LOAD(float4, voxel_tex3D, imageCoords, 0);

	if (!any(color))
		return;

	/*
		4 5	
		6 7
			0 1
			2 3
	*/
	float halfExtent = 1.0;
	float4 vtx0 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent,  halfExtent)), drawParam);
	float4 vtx1 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent,  halfExtent)), drawParam);
	float4 vtx2 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent,  halfExtent)), drawParam);
	float4 vtx3 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent,  halfExtent)), drawParam);
	float4 vtx4 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent, -halfExtent)), drawParam);
	float4 vtx5 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent, -halfExtent)), drawParam);
	float4 vtx6 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent, -halfExtent)), drawParam);
	float4 vtx7 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent, -halfExtent)), drawParam);
	
	// uv is wrong now
	createQuad(outStream, vtx0, vtx2, vtx4, vtx6, color);		// left 	face
	createQuad(outStream, vtx7, vtx3, vtx5, vtx1, color);		// right 	face
	createQuad(outStream, vtx2, vtx3, vtx6, vtx7, color);		// bottom 	face
	createQuad(outStream, vtx5, vtx1, vtx4, vtx0, color);		// top 		face
	createQuad(outStream, vtx7, vtx5, vtx6, vtx4, color);		// back 	face
	createQuad(outStream, vtx0, vtx1, vtx2, vtx3, color);		// front 	face
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