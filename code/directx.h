#include <d3d11.h>
#include <d3dcompiler.h>

struct renderer
{
    ID3D11Device *Device;
    ID3D11DeviceContext *RenderContext;
    IDXGISwapChain *SwapChain;
    ID3D11RenderTargetView *BackBuffer;
};

struct shader
{
    ID3D11VertexShader *VertexShader;
    ID3D11PixelShader  *PixelShader;
    ID3D11InputLayout  *InputLayout;   
};

struct const_buffer
{
    ID3D11Buffer *Buffer;
};

struct mesh
{
    ID3D11Buffer *VertexBuffer;
    r32 *Vertices;
    u32 VerticesCount;
};

struct texture
{
    void *Pixels;
    u32 Width;
    u32 Height;
    ID3D11ShaderResourceView *ColorMap;
    ID3D11SamplerState *ColorMapSampler;
};

struct texture_list
{
    ID3D11ShaderResourceView **ColorMap;
    ID3D11SamplerState *ColorMapSampler;
    u32 Count;
};

struct input_layout_desc
{
    char *SemanticName;
    u32 SemanticIndex;
    input_layout_format Format;
    u32 InputSlot;
    u32 AlignedByteOffset;
    input_layout_classification InputSlotClass;
    u32 InstanceDataStepRate;
};

struct batch
{
    arena *Arena;
    u32 MaxNumberOfQuads;
    u32 QuadCount;
    ID3D11Buffer *VertexBuffer;
};
