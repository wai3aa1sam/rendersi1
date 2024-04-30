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

#include "built-in/shader/rds_shader.hlsl"

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

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

#define RDS_RegisterUniform(name, structure) \
	cbuffer name structure; \
	uint 	name ## idx; \

struct TestBuffer
{
	float4 color;
};

//struct ResourceHandle
//{
//};
//RDS_CONSTANT_BUFFER(ResourceHandle, rscHnd, 1);

RDS_TEXTURE_2D(texture0);

//ConstantBuffer<ResourceHandle> rscHnd : register(b1, RDS_CONSTANT_BUFFER_SPACE);

RDS_BUFFER(TestBuffer, 		testBuffer);

// dont test in ps, since it has sync problem, but I don't want to think a example for atomic usage
RDS_RW_BUFFER(TestBuffer, 	testRwBuffer);	

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
	//color.r += RDS_MATRIX_MVP[0][0];

	//color.r = rds_test_1;
	//color.g = rds_test_3;
	//color.b = rds_test_5;
	//color += texture2DTable[textureIdx.texture0].Sample(samplerTable[textureIdx.texture0], i.uv);
	color = RDS_TEXTURE_2D_SAMPLE(texture0, i.uv);

	TestBuffer temp;
	temp.color = color;
	//RDS_RW_BUFFER_STORE_I(TestBuffer, testRwBuffer, 0, temp);
	//RDS_RW_BUFFER_STORE_I(TestBuffer, testRwBuffer, 1, temp);
	//RDS_RW_BUFFER_STORE_I(TestBuffer, testRwBuffer, 2, temp);
	//RDS_RW_BUFFER_STORE_I(TestBuffer, testRwBuffer, 3, temp);

	TestBuffer ret = RDS_BUFFER_LOAD(TestBuffer, testBuffer);
	
	//color.r = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 0).color.r * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 0).color.r;
	//color.g = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 1).color.g * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 1).color.g;
	//color.b = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 2).color.b * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 2).color.b;
	//color.a = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 3).color.a * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 3).color.a;

	#if 1

	color.r = color.r * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 0).color.r;
	color.g = color.g * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 1).color.g;
	color.b = color.b * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 2).color.b;
	color.a = color.a * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, 3).color.a;

	color = temp.color * RDS_BUFFER_LOAD_I(TestBuffer, testBuffer, rds_perObjectParam.id).color;

	#else
	color.r = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 0).color.r;
	color.g = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 1).color.g;
	color.b = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 2).color.b;
	color.a = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 3).color.a;

	color = RDS_RW_BUFFER_LOAD_I(TestBuffer, testRwBuffer, 0).color;
	
	rds_rwBufferTable[NonUniformResourceIndex(testRwBuffer)].Store<TestBuffer>(0, temp);
	color = rds_rwBufferTable[NonUniformResourceIndex(testRwBuffer)].Load<TestBuffer>(0).color;

	#endif
	
	//RWStructuredBuffer<TestBuffer> outBuf = RDS_RW_BUFFER_GET(testRwBuffer).Load<TestBuffer>(0);
	//outBuf.color;

	//ret.color = color;
	//color.r -= RDS_MATRIX_MVP[0][0];

    return color;
}