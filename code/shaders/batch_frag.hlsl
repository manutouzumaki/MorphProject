// Pixel Shader
Texture2D colorMap[3] : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float tex1 : TEXCOORD1;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    if(frag.tex1 > 0.0f && frag.tex1 <= 1.0f)
    {
        return colorMap[1].Sample(colorSampler, frag.tex0);
    }
    else if(frag.tex1 > 1.0f && frag.tex1 <= 2.0f)
    {
        return colorMap[2].Sample(colorSampler, frag.tex0);
    }
    else
    {
        return colorMap[0].Sample(colorSampler, frag.tex0);
    }
}
