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
    // Texture
    float2 FinalTex0 = float2(lerp(0.0f, 176.0f/frag.TexSize.x, frag.tex0.x),
                              lerp(0.0f, 288.0f/frag.TexSize.y, frag.tex0.y));
    FinalTex0 -= frag.Offset;
    float4 Color = colorMap.Sample(colorSampler, FinalTex0);
    
    // Grid
    float2 Coord = frag.tex0;
    // adjust the size of the cells in the grid
    Coord.x *= (176.0f / 16.0f);
    Coord.y *= (288.0f / 16.0f);
    // move the grid at the same speed that the texture
    Coord.x -= frag.Offset.x * (frag.TexSize.x / 16.0f);
    Coord.y -= frag.Offset.y * (frag.TexSize.y / 16.0f);
    
    float2 Derivative = fwidth(Coord);
    float2 Grid = abs(frac(Coord - 0.5f) - 0.5f) / Derivative;
    float Line = min(Grid.x, Grid.y);
    float GridMask = min(Line, 1.0f);
    if(GridMask < 1.0f)
    {
        GridMask = 0.0f;
    } 

    Color.xyz = Color.xyz * GridMask;
    if(GridMask == 0.0)
    {
        Color.w = 1.0f;
    }
    
    return Color;
}
