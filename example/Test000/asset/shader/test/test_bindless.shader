#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D mainTex
		Texture2D heightTex
	}
	
	Pass {
		// Queue	"Transparent"
		//Cull		None

//		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		Wireframe false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

#include "common/rdsCommon.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float4 positionWS   : POSITION;
    float2 uv           : TEXCOORD0;
    float3 normal       : NORMAL0;
};


#define RDS_RegisterUniform(name, structure) \
	cbuffer name structure; \
	uint 	name ## idx; \

struct ResourceHandle
{
	RDS_TEXTURE_2D(texture0);
	RDS_BUFFER(testBufferIndex);
};
RDS_CONSTANT_BUFFER(ResourceHandle, rscHnd, 1);
//ConstantBuffer<ResourceHandle> rscHnd : register(b1, RDS_CONSTANT_BUFFER_SPACE);

struct TestBuffer
{
	float4 color;
};

template<typename T> 
T loadBindings() 
{
    T result = bufferTable[0].Load<T>(0);		// (g_bindingsOffset.bindingsOffset)
	return result;
}

template<typename T> 
T loadTexture(float2 uv) 
{
	T o = texture2DTable[rscHnd.texture0].Sample(samplerTable[rscHnd.RDS_SAMPLER_NAME(texture0)], uv);
	return o;
}

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    o.positionHCS = mul(rds_matrix_mvp,     i.positionOS);
    o.positionWS  = mul(rds_matrix_model,   i.positionOS);
    o.normal      = i.normal;
    o.uv          = i.uv;

	//o.uv.x += (float)texture0;
	//o.uv.x = i.uv.x;

	//Camera cam = Camera(bufferTable[camera_idx].Load(0);
	//int cam = bufferTable[camera_idx].Load(0);
	//o.positionHCS.xyz = rds_pos_view;
	
	//texture0

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
	//color.r += rds_matrix_mvp[0][0];

	//color.r = rds_test_1;
	//color.g = rds_test_3;
	//color.b = rds_test_5;
	//color += texture2DTable[textureIdx.texture0].Sample(samplerTable[textureIdx.texture0], i.uv);
	color = loadTexture<float4>(i.uv);

	ByteAddressBuffer buffer = bufferTable[rscHnd.testBufferIndex];
	//TestBuffer ret = buffer.Load<TestBuffer>(sizeof(TestBuffer) * 1);
	TestBuffer ret = buffer.Load<TestBuffer>(0);
	color *= ret.color;

	//color.r -= rds_matrix_mvp[0][0];

    return color;
}