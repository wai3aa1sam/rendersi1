struct VertexIn
{
    float4 positionOS   : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;

    uint vertexId       : SV_VertexID;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD0;
};

float4x4	rds_matrix_model;
float4x4	rds_matrix_view;
float4x4	rds_matrix_proj;
float4x4	rds_matrix_mvp;

Texture2D       texture0 : register(t1);       // texture is a reserved word
SamplerState    texture0_Sampler : register(s1);

//Texture2D       texture2 : register(t2);       // texture is a reserved word
//SamplerState    texture2_Sampler : register(s2);

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    
    //o.positionOS    = positions[i.vertexId];
    //o.color         = colors[i.vertexId];
    o.positionHCS = mul(rds_matrix_mvp, i.positionOS);
    o.color       = i.color;
    o.uv          = i.uv;
    
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    //float2 pos2f = i.positionOS;
	float4 o = texture0.Sample(texture0_Sampler, i.uv);
	//o += i.color * texture2.Sample(texture2_Sampler, i.uv);

    o.a = 1;
    return o;
}