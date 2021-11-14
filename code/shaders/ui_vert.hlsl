// Vertex Shader
cbuffer CBufferProjection : register(b0)
{
    matrix Projection;
    matrix World;
    matrix View;
};

cbuffer CBufferUI : register(b1)
{
    float2 TexSize;
    float2 Offset;
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
    float2 TexSize : TEXCOORD1;
    float2 Offset : TEXCOORD2;
};

PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul(float4(vertex.pos, 1.0f), World);
    vsOut.pos = mul(vsOut.pos, Projection);
    vsOut.tex0 = vertex.tex0;
    vsOut.TexSize = TexSize;
    vsOut.Offset = Offset;
    return vsOut;
}
