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

cbuffer rds_common : register(b0, space0)
{
	float4x4	rds_matrix_model;
	float4x4	rds_matrix_view;
	float4x4	rds_matrix_proj;
	float4x4	rds_matrix_mvp;
};

#define RDS_RegisterUniform(name, structure) \
	cbuffer name structure; \
	uint 	name ## idx; \
//---

// RDS_RegisterUniform(camera, 
// {
// 	float3		rds_pos_view;
// 	float4x4 	rds_mat_view;
// });


//cbuffer 		cbufferTable {} cbufferTables[]  : register(t0, RDS_TEX_2D_SPACE);

#if	1

#define RDS_CONCAT_IMPL(A, B) A ## B
#define RDS_CONCAT(A, B) RDS_CONCAT_IMPL(A, B)

#define RDS_BUFFER_SPACE 	space2

#define RDS_TEX_2D_SPACE 	space3
#define RDS_TEX_CUBE_SPACE 	RDS_TEX_2D_SPACE
#define RDS_SAMPLER_SPACE 	RDS_TEX_2D_SPACE

#define RDS_IMAGE_SPACE 	space4

//#define RDS_K_SAMPLER_COUNT 1		// set in compiler
#define RDS_TEXTURE_BINDING RDS_CONCAT(t, RDS_K_SAMPLER_COUNT)

// ByteAddressBuffer
ByteAddressBuffer 	bufferTable		[] 						: register(t0, RDS_BUFFER_SPACE);

SamplerState    	samplerTable	[RDS_K_SAMPLER_COUNT] 	: register(s0, 					RDS_SAMPLER_SPACE);
Texture2D 			texture2DTable	[]  					: register(RDS_TEXTURE_BINDING, RDS_TEX_2D_SPACE);
TextureCube 		textureCubeTable[]  					: register(RDS_TEXTURE_BINDING, RDS_TEX_CUBE_SPACE);

RWTexture2D<float>	image2DTable	[]						: register(u0, RDS_IMAGE_SPACE);

#endif

#define RDS_SAMPLER_NAME(TEX_NAME) RDS_CONCAT(RDS_CONCAT(_rds_, TEX_NAME), _sampler)
#define RDS_TEXTURE_2D(NAME) uint NAME; uint RDS_SAMPLER_NAME(NAME)

struct ResourceHandle
{
	RDS_TEXTURE_2D(texture0);
};

ConstantBuffer<ResourceHandle> rscHnd : register(b1, space1);

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
	//color.r -= rds_matrix_mvp[0][0];

    return color;
}