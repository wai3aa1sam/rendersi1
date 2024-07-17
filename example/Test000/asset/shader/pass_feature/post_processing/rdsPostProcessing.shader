#if 0
Shader {
	Properties {
		
		Vec3f 	exposure 			= { 1.0, 1.0, 1.0 }
		Vec3f 	contrast 			= { 1.0, 1.0, 1.0 }
		Vec3f 	contrast_offset 	= { 0.5, 0.5, 0.5 }
		
		Vec3f 	brightness 			= { 0.0, 0.0, 0.0 }
		Vec3f 	saturation			= { 1.0, 1.0, 1.0 }

		Color4f color_filter		= { 1.0, 1.0, 1.0, 1.0 }

		Float 	gamma 				= 2.2
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		CsFunc		cs_main
	}
}
#endif

#include "built-in/shader/rds_shader.hlsl"
#include "built-in/shader/post_processing/rdsPostProcessing.hlsl"
#include "built-in/shader/post_processing/rdsToneMapping.hlsl"


struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

RDS_TEXTURE_2D(tex_color);
RDS_IMAGE_2D(float4, out_image);

float3 exposure;
float3 contrast;
float3 contrast_offset;

float3 brightness;
float3 saturation;

float4 color_filter;

float gamma;

[numThreads(CS_BLOCK_SIZE, CS_BLOCK_SIZE, 1)]
void cs_main(ComputeIn input)
{
	DrawParam 	drawParam 			= rds_DrawParam_get();
	uint2 		resolution 			= drawParam.resolution;
	uint2  		dispatchThreadId 	= input.dispatchThreadId.xy;

	bool isInBoundary = Image_isInBoundary(dispatchThreadId, resolution);
	if (!isInBoundary)
		return;

	float4 o = float4(0.0, 0.0, 0.0, 1.0);

	float2 uv 		= Image_computeUv(dispatchThreadId, drawParam);
	float4 sampled 	= RDS_TEXTURE_2D_SAMPLE_LOD(tex_color, uv, 0);

	float3 color = sampled.rgb;

	#if 0
	RDS_IMAGE_2D_GET(float4, out_image)[dispatchThreadId] = float4(color, sampled.a);
	return;
	#endif

	color = PostProc_exposure(color, exposure);
	// TODO: white balancing
	color = PostProc_contrast(color, contrast, contrast_offset);

	color = PostProc_brightness(color, brightness);
	color = PostProc_colorFilter(color, color_filter);

	float3 luminance = PostProc_luminance(color);
	color = PostProc_saturation(color, luminance, saturation);

	color = ToneMapping_reinhard(color);		// ToneMapping_reinhard ToneMapping_uncharted2
	color = PostProc_gammaEncoding(color, gamma);

	o = float4(color, sampled.a);
	RDS_IMAGE_2D_GET(float4, out_image)[dispatchThreadId] = o;
}
