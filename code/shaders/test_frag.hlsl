// Pixel Shader
Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 tex1 : TEXCOORD1;
    float2 TexSize : TEXCOORD2;
    float2 TileSize : TEXCOORD3;
    float2 Frame : TEXCOORD4;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float2 TilesCount = frag.TexSize / frag.TileSize;
    float2 Range = frag.TileSize / frag.TexSize;
    frag.tex0 /= TilesCount;
    frag.tex0 += Range * frag.Frame;
    return colorMap.Sample(colorSampler, frag.tex0);
}
