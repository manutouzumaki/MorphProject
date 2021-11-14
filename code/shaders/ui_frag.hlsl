// Pixel Shader
Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float2 TexSize : TEXCOORD1;
    float2 Offset : TEXCOORD2;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float2 FinalTex0 = float2(lerp(0.0f, 176.0f/frag.TexSize.x, frag.tex0.x),
                              lerp(0.0f, 288.0f/frag.TexSize.y, frag.tex0.y));
    FinalTex0 -= frag.Offset;
    return colorMap.Sample(colorSampler, FinalTex0);
}
