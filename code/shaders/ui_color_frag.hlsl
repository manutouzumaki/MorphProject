// Pixel Shader
Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 tex1 : TEXCOORD1;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    return float4(frag.tex1.xyz, 1.0f);
}
