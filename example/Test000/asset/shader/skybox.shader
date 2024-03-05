#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		Front

		DepthTest	LessEqual

//		DepthTest	Always
		DepthWrite	false

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
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float3 uv           : TEXCOORD0;
};

RDS_TEXTURE_CUBE(skybox);

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    float4x4 matView = rds_matrix_view;
	matView[0][3] = 0.0;
	matView[1][3] = 0.0;
	matView[2][3] = 0.0;
    float4x4 matVp  = mul(rds_matrix_proj, matView);

    o.positionHCS   = mul(matVp, i.positionOS);              // view mat is camera translation only
    o.positionHCS   = o.positionHCS.xyww;
    o.uv            = i.positionOS.xyz;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o;
	float3 uv = i.uv;
	//uv.y = -uv.y;
    o = RDS_SAMPLE_TEXTURE_CUBE(skybox, uv).rgb;
    //o.rgb = RDS_TEXTURE_CUBE_SAMPLE_LOD(skybox, uv, 1.2);
	
    
    //o = ToneMapping_reinhard(o);
    //o = PostProc_gammaEncoding(o);

    return float4(o, 1.0);
}