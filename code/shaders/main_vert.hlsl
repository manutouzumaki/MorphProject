// Vertex Shader
cbuffer CBufferProjection : register(b0)
{
    matrix Projection;
    matrix World;
    matrix View;
};

struct VS_Input
{
    float3 pos : POSITION;
    float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul(float4(vertex.pos, 1.0f), World);
    vsOut.pos = mul(vsOut.pos, View);
    vsOut.pos = mul(vsOut.pos, Projection);
    vsOut.tex0 = vertex.tex0;
    return vsOut;
}
