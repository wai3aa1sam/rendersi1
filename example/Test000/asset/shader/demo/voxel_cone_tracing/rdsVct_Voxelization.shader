#if 0
Shader {
	Properties {
		Color4f  	RDS_MATERIAL_PROPERTY_baseColor	= {1.0, 1.0, 1.0, 1.0}
		Color4f  	RDS_MATERIAL_PROPERTY_emission	= {1.0, 1.0, 1.0, 1.0}
		Float   	RDS_MATERIAL_PROPERTY_metalness	= 0.0
		Float   	RDS_MATERIAL_PROPERTY_roughness	= 0.5

		Texture2D 	RDS_MATERIAL_TEXTURE_baseColor
		Texture2D 	RDS_MATERIAL_TEXTURE_normal
		Texture2D 	RDS_MATERIAL_TEXTURE_roughnessMetalness
		Texture2D 	RDS_MATERIAL_TEXTURE_emission
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
#include "built-in/shader/lighting/rdsDefaultLighting.hlsl"
#include "built-in/shader/geometry/rdsGeometry_AABBox.hlsl"

struct VertexIn
{
    float4 positionOs   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;
    float3 tangent      : TANGENT;
};

struct GeometryIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;
    float3 positionWs   : POSITION;
    float3 tangent      : TEXCOORD1;
};

struct PixelIn
{
    float4 positionHcs  : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal   	: NORMAL;
	float3 positionWs   : POSITION;
    float3 tangent      : TEXCOORD1;

	float4 positionCs   : TEXCOORD2;
	float3 posVoxel   	: TEXCOORD3;

    nointerpolation float3 aabboxMin   	: TEXCOORD4;
    nointerpolation float3 aabboxMax   	: TEXCOORD5;
};

float4x4 voxel_matrix_vp_x;
float4x4 voxel_matrix_vp_y;
float4x4 voxel_matrix_vp_z;
float4x4 rds_matrix_vp;
float4x4 voxel_matrix_proj;

RDS_IMAGE_3D(uint, voxel_tex_radiance);

GeometryIn vs_main(VertexIn input)
{
	GeometryIn o = (GeometryIn)0;

	DrawParam 		drawParam = rds_DrawParam_get();
	ObjectTransform objTransf = rds_ObjectTransform_get();
	
	o.positionHcs	= float4(SpaceTransform_objectToWorld(input.positionOs, 	objTransf), 1.0);
    o.uv		  	= input.uv;
    o.normal 	  	= SpaceTransform_toWorldNormal(input.normal, 		objTransf);
    o.tangent 		= SpaceTransform_toWorldNormal(input.tangent, 		objTransf);

    o.positionWs  	= o.positionHcs.xyz;

    return o;
}

[maxvertexcount(3)]
void geom_main(triangle GeometryIn input[3], inout TriangleStream<PixelIn> outStream)
{
	//int axisIdx = getDominantAxisIdx(input[0].positionWs.xyz, input[1].positionWs.xyz, input[2].positionWs.xyz);
	DrawParam drawParam = rds_DrawParam_get();
	
	float voxelResolution		= Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	float voxelResolutionInv 	= 1.0 / voxelResolution;

	float3 vtx0 = input[0].positionWs.xyz;
	float3 vtx1 = input[1].positionWs.xyz;
	float3 vtx2 = input[2].positionWs.xyz;
	float3 n 	= abs(normalize(cross(vtx1 - vtx0, vtx2 - vtx0)));
	float  axis = max(n.x, max(n.y, n.z));

	float3 aabboxMin = min(vtx0, min(vtx1, vtx2));
	float3 aabboxMax = max(vtx0, max(vtx1, vtx2));

	//float3 	center 	= (input[0].positionWs.xyz + input[1].positionWs.xyz + input[2].positionWs.xyz) / 3.0;
	//float 	level 	= floor(clamp(log2(length(center - drawParam.camera_pos) / (voxelResolution * voxel_sizeL0)), 0, clipmap_maxLevel - 1));

	float 			clipmapLevel 	= clipmap_level;
	//clipmapLevel 					= level;
	VoxelClipmap 	clipmap 		= Vct_getVoxelClipmap(clipmapLevel);

	PixelIn output[3];
	for(int i = 0; i < 3 ; i++)
	{
		PixelIn o = (PixelIn)0;
		o.positionCs  = input[i].positionHcs;
		o.normal	  = input[i].normal;
		o.uv 		  = input[i].uv;
		o.tangent	  = input[i].tangent.xyz;
		o.positionWs  = input[i].positionWs.xyz;

		o.posVoxel	  = (input[i].positionWs - clipmap.center) / clipmap.voxelSize; 	// to voxel grid space
		//o.level		  = clipmapLevel;

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

	output[0].positionHcs.xy *= voxelResolutionInv;
	output[1].positionHcs.xy *= voxelResolutionInv;
	output[2].positionHcs.xy *= voxelResolutionInv;

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
	float2		uv			= input.uv;
	float3		viewDir;

	float 		voxelResolution		= Vct_computeVoxelResolutionWithBorder(voxel_resolution);
	float 		voxelResolutionInv 	= 1.0 / voxelResolution;

	float3 			imageCoords 	= float3(0, 0, 0);
	float 			clipmapLevel 	= clipmap_level;
	VoxelClipmap 	clipmap 		= Vct_getVoxelClipmap(clipmapLevel);

	float3 uvw  = Vct_worldToClipmapUvw(posWs, clipmap, voxelResolutionInv);
	imageCoords = floor(uvw * voxelResolution);
	imageCoords = Vct_computeImageCoords(imageCoords, clipmapLevel, voxelResolution);

	if (!isInBoundary01(uvw))
		discard;

	float3 pos;
	float3 normal;
	float3 tangent;

	pos		= posWs;
	normal	= normalize(input.normal);
	tangent = normalize(input.tangent);

	#if 1
	Surface surface = Material_makeSurface(uv, pos, normal, tangent);
	//surface = Material_makeSurface(uv, pos, normal);

	LightingResult oLightingResult = (LightingResult)0;
	oLightingResult = Lighting_computeForwardLighting_Ws(surface, drawParam.camera_pos, viewDir);
	float shadow;
	//shadow = csm_computeShadow(input.positionWs, input.positionVs);
	shadow = 0.0;
	o.rgb = (1.0 - shadow) * oLightingResult.diffuse.rgb + oLightingResult.specular.rgb;

	#endif
	
	//o.rgb = float3(1.0, 1.0, 1.0);
	//o.rgb = float3(1.0, 0.0, 0.0);

	//o.rgb = float3(pos.z, pos.z, pos.z);
	//o.rgb = remapNeg11To01(normal);
	//o.rgb = pos;

	float3 faceIndices 	= Vct_computeVoxelFaceIndices(normal);
	float3 weights		= abs(normal);
	
	//weights.x = frameIndex * 2;
	//weights = abs(normal);

	//faceIndices	= (float3)0;
	//weights 	= float3(1.0, 1.0, 1.0);

	RWTexture3D<uint> voxelTexRadiance = RDS_IMAGE_3D_GET(uint, voxel_tex_radiance);

	#if VCT_USE_6_FACES_CLIPMAP
	Vct_VoxelClipmapAtomicStoreAvgRGBA8(voxelTexRadiance, imageCoords, o.rgb, faceIndices, weights, voxelResolution);
	//AtomicOp_avgRGBA8(voxelTexRadiance, uint3(imageCoords), o.rgb);

	#else
	AtomicOp_avgRGBA8(voxelTexRadiance, uint3(imageCoords), o.rgb);
	#endif

	return o;
}
