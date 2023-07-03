struct VertexIn
{
    float4 position : SV_POSITION;
    //float4 color    : COLOR;
    uint vertexId   : SV_VertexID;
};

struct PixelIn 
{
    float4 position   : SV_POSITION;    // gl_Position
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
    o.position      = float4(positions[i.vertexId], 0.0, 1.0);

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    //float2 pos2f = i.positionOS;
    float4 pos4f = i.position;

    float4 color = float4(i.color, 1.0);
    return color;
}