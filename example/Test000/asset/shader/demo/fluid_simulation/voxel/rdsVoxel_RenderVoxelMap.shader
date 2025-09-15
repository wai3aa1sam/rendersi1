#if 0
Shader {
	Properties {
		Color4f border_color = {0.5, 0.5, 0.5, 1.0}
		Float 	border_width = 0.05

		Float 	u_voxelScale = 1.0

		Color4f u_ambient = {0.5, 0.5, 0.5, 1.0}
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
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"

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
	float3 normal		: NORMAL;

	float density		: TEXCOORD1;
    float3 positionWs   : TEXCOORD2;
};

float 	border_width;
float4 	border_color;
//float 	voxel_size_scale;

RDS_TEXTURE_3D_T(half, u_voxelMap);
uint3 	u_voxelMapSize;
float 	u_voxelScale;
float3 	u_boundingPos;

RDS_TEXTURE_2D(u_colorMap);
float 	u_densityMax;

float4 u_ambient;

void createQuad(inout TriangleStream<PixelIn> outStream
, float density
, uint vtx0, uint vtx1, uint vtx2, uint vtx3
, float4 color, float3 faceNormalWs, float3 cameraPos
, float4 verticesWs[8], float4 verticesHcs[8])
{
	PixelIn o = (PixelIn)0;
	o.color			= color;
	o.normal 		= faceNormalWs;	// TODO: transform normal to world
	o.density   	= density;

    float3 toCamera = normalize(cameraPos - verticesWs[vtx0].xyz);
    float dotProduct = dot(faceNormalWs, toCamera);

	bool isFrontFace = dotProduct > 0;
    if (!isFrontFace)
		return;

	o.positionHcs 	= verticesHcs[vtx0];
	o.positionWs 	= verticesWs[vtx0].xyz;
	o.uv 			= float2(0.0, 1.0);
	outStream.Append(o);

	o.positionHcs 	= verticesHcs[vtx2];
	o.positionWs 	= verticesWs[vtx2].xyz;
	o.uv 			= float2(0.0, 0.0);
	outStream.Append(o);

	o.positionHcs 	= verticesHcs[vtx1];
	o.positionWs 	= verticesWs[vtx1].xyz;
	o.uv 			= float2(1.0, 1.0);
	outStream.Append(o);
    outStream.RestartStrip();

	o.positionHcs 	= verticesHcs[vtx1];
	o.positionWs 	= verticesWs[vtx1].xyz;
	o.uv 			= float2(1.0, 1.0);
	outStream.Append(o);

	o.positionHcs 	= verticesHcs[vtx2];
	o.positionWs 	= verticesWs[vtx2].xyz;
	o.uv 			= float2(0.0, 0.0);
	outStream.Append(o);
	
	o.positionHcs 	= verticesHcs[vtx3];
	o.positionWs 	= verticesWs[vtx3].xyz;
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
	half density = tex[posMap];
	if (density == 0.0)
		return;

	float 	densityT 	= density / u_densityMax;
	float4 	color   	= RDS_TEXTURE_2D_SAMPLE_LOD(u_colorMap, float2(densityT, 0.5), 0);

	/*
		4 5	
		6 7
			0 1
			2 3
	*/
	DrawParam drawParam = rds_DrawParam_get();
	float3 camera_pos = drawParam.camera_pos;

	float halfExtent = 1.0;
	float4 verticesWs[8];
	float4 verticesHcs[8];

	verticesWs[0] = toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent,  halfExtent));
	verticesWs[1] = toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent,  halfExtent));
	verticesWs[2] = toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent,  halfExtent));
	verticesWs[3] = toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent,  halfExtent));
	verticesWs[4] = toWorldVoxel(posVoxel + float3(-halfExtent,  halfExtent, -halfExtent));
	verticesWs[5] = toWorldVoxel(posVoxel + float3( halfExtent,  halfExtent, -halfExtent));
	verticesWs[6] = toWorldVoxel(posVoxel + float3(-halfExtent, -halfExtent, -halfExtent));
	verticesWs[7] = toWorldVoxel(posVoxel + float3( halfExtent, -halfExtent, -halfExtent));

	verticesHcs[0] = SpaceTransform_worldToClip(verticesWs[0], drawParam);
	verticesHcs[1] = SpaceTransform_worldToClip(verticesWs[1], drawParam);
	verticesHcs[2] = SpaceTransform_worldToClip(verticesWs[2], drawParam);
	verticesHcs[3] = SpaceTransform_worldToClip(verticesWs[3], drawParam);
	verticesHcs[4] = SpaceTransform_worldToClip(verticesWs[4], drawParam);
	verticesHcs[5] = SpaceTransform_worldToClip(verticesWs[5], drawParam);
	verticesHcs[6] = SpaceTransform_worldToClip(verticesWs[6], drawParam);
	verticesHcs[7] = SpaceTransform_worldToClip(verticesWs[7], drawParam);

	// uv is wrong now
	{ createQuad(outStream, density, 7, 3, 5, 1, color, float3(+1.0, +0.0, +0.0), camera_pos, verticesWs, verticesHcs); }		// right 	face	// 
	{ createQuad(outStream, density, 0, 2, 4, 6, color, float3(-1.0, +0.0, +0.0), camera_pos, verticesWs, verticesHcs); }		// left 	face	// 
	{ createQuad(outStream, density, 5, 1, 4, 0, color, float3(+0.0, +1.0, +0.0), camera_pos, verticesWs, verticesHcs); }		// top 		face	// 
	{ createQuad(outStream, density, 2, 3, 6, 7, color, float3(+0.0, -1.0, +0.0), camera_pos, verticesWs, verticesHcs); }		// bottom 	face	// 
	{ createQuad(outStream, density, 0, 1, 2, 3, color, float3(+0.0, +0.0, +1.0), camera_pos, verticesWs, verticesHcs); }		// front 	face	// 
	{ createQuad(outStream, density, 7, 5, 6, 4, color, float3(+0.0, +0.0, -1.0), camera_pos, verticesWs, verticesHcs); }		// back 	face	// 
}

float4 ps_main(PixelIn input) : SV_TARGET
{
	float4 o = float4(0.0, 0.0, 0.0, 1.0);
	float2 uv 			= input.uv;
	float4 color 		= input.color;
	float3 normal 		= input.normal;
	float3 normalColor 	= remapNeg11To01(normal);

	float 	densityT 	= input.density / u_densityMax;
	color = 1.0;

	float isBorder = 1.0 - min((min(uv.x, (min(uv.y, min(1.0 - uv.x, 1.0 - uv.y))))) / border_width, 1.0);
	o.rgb = lerp(color.rgb, border_color.rgb, isBorder);
	//o.rgb = lerp(normalColor.rgb, o.rgb, isBorder);
	//o.rgb *= u_ambient.rgb;

	float3 			viewDir;
	DrawParam 		drawParam = rds_DrawParam_get();

	Surface surface = Material_makeSurface(uv, input.positionWs, normalize(input.normal));
	surface.baseColor = o;

	LightingResult o_ightingResult = (LightingResult)0;
	LightingResult result = Lighting_computeForwardLighting_Ws(surface, drawParam.camera_pos, viewDir);
	o_ightingResult.diffuse 	+= result.diffuse;
	o_ightingResult.specular 	+= result.specular;

	o.rgb = o_ightingResult.diffuse.rgb + o_ightingResult.specular.rgb;

	return o;
}
