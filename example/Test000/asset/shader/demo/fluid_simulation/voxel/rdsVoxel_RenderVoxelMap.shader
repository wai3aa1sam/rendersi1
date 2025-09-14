#if 0
Shader {
	Properties {
		Color4f border_color = {0.5, 0.5, 0.5, 1.0}
		Float 	border_width = 0.05

		Float 	u_voxelScale = 1.0
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

#include "built-in/shader/rds_shader.hlsl"

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
//float 	voxel_size_scale;

RDS_TEXTURE_3D_T(half, u_voxelMap);
uint3 	u_voxelMapSize;
float 	u_voxelScale;
float3 	u_boundingPos;

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

float4 toWorldVoxel(float3 posOs)
{
	float voxelScale = u_voxelScale;
	float4 o = float4(posOs, 1.0);
	o.xyz = (o.xyz * voxelScale) + u_boundingPos;
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

float3 uvwToVoxel(float3 uvw, float voxelResolution)
{
	float3 posVoxel = remap01ToNeg11(uvw) * voxelResolution;
	return posVoxel;
}

[maxvertexcount(36)]
void geom_main(point GeometryIn input[1], inout TriangleStream<PixelIn> outStream)
{
	PixelIn o = (PixelIn)0;

	uint vertexId		= input[0].vertexId;
	uint voxelIndex		= vertexId;

	float3 posMap 		= unflatten3D(voxelIndex, u_voxelMapSize);
	float3 center		= (posMap + 0.5);
	float3 uvw 			= (center) / u_voxelMapSize;
	float3 posVoxel 	= uvwToVoxel(uvw, u_voxelMapSize.x);

	Texture3D<half> tex = RDS_TEXTURE_3D_T_GET(half, u_voxelMap);
	half particleCountPercentage = tex[posMap];
	if (particleCountPercentage == 0.0)
		return;

	/*
		4 5	
		6 7
			0 1
			2 3
	*/
	DrawParam drawParam = rds_DrawParam_get();
	float halfExtent = 1.0;
	float4 vtx0 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent,  halfExtent)), drawParam);
	float4 vtx1 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent,  halfExtent)), drawParam);
	float4 vtx2 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent,  halfExtent)), drawParam);
	float4 vtx3 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent,  halfExtent)), drawParam);
	float4 vtx4 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent, -halfExtent)), drawParam);
	float4 vtx5 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent, -halfExtent)), drawParam);
	float4 vtx6 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent, -halfExtent)), drawParam);
	float4 vtx7 = SpaceTransform_worldToClip(toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent, -halfExtent)), drawParam);
	
	float4 color = 1;
	// uv is wrong now
	{ createQuad(outStream, vtx7, vtx3, vtx5, vtx1, color); }		// right 	face
	{ createQuad(outStream, vtx0, vtx2, vtx4, vtx6, color); }		// left 	face
	{ createQuad(outStream, vtx5, vtx1, vtx4, vtx0, color); }		// top 		face
	{ createQuad(outStream, vtx2, vtx3, vtx6, vtx7, color); }		// bottom 	face
	{ createQuad(outStream, vtx0, vtx1, vtx2, vtx3, color); }		// front 	face
	{ createQuad(outStream, vtx7, vtx5, vtx6, vtx4, color); }		// back 	face
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
	//o.rgb = float3(1.0, 0.0, 0.0);

	return o;
}