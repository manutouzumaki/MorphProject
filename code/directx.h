#ifndef DIRECTX_H
#define DIRECTX_H

#include <d3d11.h>
#include <d3dcompiler.h>

struct renderer
{
    ID3D11Device *Device;
    ID3D11DeviceContext *RenderContext;
    IDXGISwapChain *SwapChain;
    ID3D11RenderTargetView *BackBuffer; 
};

struct const_buffer
{
    ID3D11Buffer *Buffer;
};

void InitializeD3D11(HWND Window, u32 Width, u32 Height, renderer *Renderer);
void UninitializeD3D11(renderer *Renderer);
ID3DBlob *D3D11CompileShader(char *FileName, char *MainFuncName, char *ShaderVersion, arena *Arena);
bool D3D11CreateVertexShader(ID3D11Device *Device, char *FileName, char *MainFuncName,
                             ID3D11VertexShader **VertexShader,
                             ID3D11InputLayout **InputLayout,
                             D3D11_INPUT_ELEMENT_DESC *InputLayoutDesc,
                             u32 TotalLayoutElements, arena *Arena);
bool D3D11CreatePixelShader(ID3D11Device *Device, char *FileName, char *MainFuncName,
                            ID3D11PixelShader **PixelShader, arena *Arena);

#define CreateConstantBuffer(ConstBuffer, Type, Renderer, Arena) \
    do { \
        ConstBuffer = (const_buffer *)PushStruct(Arena, const_buffer); \
        D3D11_BUFFER_DESC BufferDesc = {}; \
        BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; \
        BufferDesc.ByteWidth = (u32)(sizeof(Type) + (16 - (sizeof(Type) % 16))); \
        BufferDesc.Usage = D3D11_USAGE_DYNAMIC; \
        BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; \
        BufferDesc.MiscFlags = 0; \
        BufferDesc.StructureByteStride = 0; \
        HRESULT Result = Renderer->Device->CreateBuffer(&BufferDesc, 0, &ConstBuffer->Buffer); \
        if(SUCCEEDED(Result)) \
        { \
            OutputDebugString("Constant Buffer Created!\n"); \
        } \
    } while(0)

#define MapConstantBuffer(Renderer, ConstBufferData, Type, ConstBuffer, Register) \
    do { \
        D3D11_MAPPED_SUBRESOURCE GPUConstantBufferData = {}; \
        Renderer->RenderContext->Map(ConstBuffer->Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GPUConstantBufferData); \
        memcpy(GPUConstantBufferData.pData, &ConstBufferData, sizeof(Type)); \
        Renderer->RenderContext->Unmap(ConstBuffer->Buffer, 0); \
        Renderer->RenderContext->VSSetConstantBuffers( Register, 1, &ConstBuffer->Buffer); \
    } while(0)

#endif
