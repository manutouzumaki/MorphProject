// Pixel Shader
struct PS_Input
{
   float4 pos : SV_POSITION;
   float2 tex0 : TEXCOORD0;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
   return float4(1, 0, 0, 1);
}
