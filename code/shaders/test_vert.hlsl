cbuffer CBufferProjection : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
};

struct VS_Input
{
   float3 pos : POSITION;
   float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
    float4 pos : SV_POSITION;  // clip space position
    float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main( VS_Input vertex )
{   
    PS_Input vsOut = (PS_Input)0;
    // calculate the position of the vertex against the world, and projection matrix
    float4 worldPos = mul(float4(vertex.pos, 1.0f), World);
    vsOut.pos = mul(worldPos, View);
    vsOut.pos = mul(vsOut.pos, Proj);
    return vsOut;
}
