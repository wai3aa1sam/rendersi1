struct VertexIn
{
    float4 position : SV_POSITION;
    //float4 color    : COLOR;
    uint vertexId   : SV_VertexID;
};

struct PixelIn 
{
	//float2 positionOS : POSITION;
    float3 color      : COLOR;
};

PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    float2 positions[3] = 
    {
        float2(0.0, -0.5),
        float2(0.5,  0.5),
        float2(-0.5, 0.5)
    };
    float3 colors[3] = 
    {
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0)
    };
    //o.positionOS    = positions[i.vertexId];
    o.color         = colors[i.vertexId];

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float4 color = float4(i.color, 1.0);
    return color;
}