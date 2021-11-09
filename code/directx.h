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

void InitializeDirecX11(HWND                   Window,
                        ID3D11Device           **Device,
                        ID3D11DeviceContext    **RenderContext,
                        IDXGISwapChain         **SwapChain,
                        ID3D11RenderTargetView **BackBuffer)
{
    RECT ClientDimensions = {};
    GetClientRect(Window, &ClientDimensions);
    u32 Width  = ClientDimensions.right - ClientDimensions.left;
    u32 Height = ClientDimensions.bottom - ClientDimensions.top;

    // -1: Define the device types and feature level we want to check for.
    D3D_DRIVER_TYPE DriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_SOFTWARE
    };
    D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    u32 DriverTypesCout = ARRAYSIZE(DriverTypes);
    u32 FeatureLevelsCount = ARRAYSIZE(FeatureLevels);
    
    // -2: Create the Direct3D device, rendering context, and swap chain.
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.BufferDesc.Width = Width;
    SwapChainDesc.BufferDesc.Height = Height;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.OutputWindow = Window;
    SwapChainDesc.Windowed = true;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
   
    D3D_FEATURE_LEVEL FeatureLevel = {}; 
    D3D_DRIVER_TYPE DriverType = {};
    HRESULT Result = {};
    for(u32 Driver = 0;
        Driver < DriverTypesCout;
        ++Driver)
    {
        Result = D3D11CreateDeviceAndSwapChain(NULL, DriverTypes[Driver], NULL, 0,
                                               FeatureLevels, FeatureLevelsCount,
                                               D3D11_SDK_VERSION, &SwapChainDesc, 
                                               SwapChain, Device, &FeatureLevel,
                                               RenderContext);
        if(SUCCEEDED(Result))
        {
            DriverType = DriverTypes[Driver];
            break;
        } 
    }

    // -3: Create render target.
    ID3D11Texture2D *BackBufferTexture = 0;
    Result = (*SwapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&BackBufferTexture);
    Result = (*Device)->CreateRenderTargetView(BackBufferTexture, 0, BackBuffer);
    if(BackBufferTexture)
    {
        BackBufferTexture->Release();
    }
    (*RenderContext)->OMSetRenderTargets(1, BackBuffer, 0);


    // Create And Set the Alpha blend state
    // Turn on Alpha blending
    ID3D11BlendState *AlphaBlend = 0;
    D3D11_BLEND_DESC BlendStateDesc = {};
    BlendStateDesc.RenderTarget[0].BlendEnable = true;
    BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    (*Device)->CreateBlendState(&BlendStateDesc, &AlphaBlend);
    (*RenderContext)->OMSetBlendState(AlphaBlend, 0, 0XFFFFFFFF);
    if(AlphaBlend) AlphaBlend->Release();

    // -4: Set the viewport.
    D3D11_VIEWPORT Viewport;
    Viewport.Width  = (r32)Width;
    Viewport.Height = (r32)Height;
    Viewport.MinDepth = 0.0f;
    Viewport.MaxDepth = 1.0f;
    Viewport.TopLeftX = 0.0f;
    Viewport.TopLeftY = 0.0f;
    (*RenderContext)->RSSetViewports(1, &Viewport);
}

PLAFORM_CREATE_RENDERER(PlatformCreateRenderer)
{
    renderer *Renderer = PushStruct(Arena, renderer);
    InitializeDirecX11(Window->Window,
                       &Renderer->Device,
                       &Renderer->RenderContext,
                       &Renderer->SwapChain,
                       &Renderer->BackBuffer);
    return Renderer;
}

COMPILE_SHADERS_FROM_FILE(CompileShadersFromFile)
{
    shader *Shader = PushStruct(Arena, shader);
    HRESULT Result;
    // Vertex Shader and Input Layout
    void *VertexShaderSource = ReadEntireFile(VSFileName, 0, Arena);
    ID3DBlob *VertexShaderCompiled = 0;
    ID3DBlob *ErrorVertexShader    = 0;
    Result = D3DCompile(VertexShaderSource,
                        (size_t)StringLength((char *)VertexShaderSource),
                        0, 0, 0, "VS_Main", "vs_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &VertexShaderCompiled, &ErrorVertexShader);
    if(ErrorVertexShader != 0)
    {
        ErrorVertexShader->Release();
    }
    // Create the Vertex Shader.
    Result = Renderer->Device->CreateVertexShader(VertexShaderCompiled->GetBufferPointer(),
                                        VertexShaderCompiled->GetBufferSize(), 0,
                                        &Shader->VertexShader);
    // Create the Input layout.
    D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
         0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    u32 TotalLayoutElements = ArrayCount(InputLayoutDesc);
    Result = Renderer->Device->CreateInputLayout(InputLayoutDesc,
                                       TotalLayoutElements,
                                       VertexShaderCompiled->GetBufferPointer(),
                                       VertexShaderCompiled->GetBufferSize(),
                                       &Shader->InputLayout);
    VertexShaderCompiled->Release();

    void *PixelShaderSource = ReadEntireFile(PSFileName, 0, Arena);
    ID3DBlob *PixelShaderCompiled = 0;
    ID3DBlob *ErrorPixelShader    = 0;
    Result = D3DCompile(PixelShaderSource,
                        (size_t)StringLength((char *)PixelShaderSource),
                        0, 0, 0, "PS_Main", "ps_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &PixelShaderCompiled, &ErrorPixelShader);
    if(ErrorPixelShader != 0)
    {
        ErrorPixelShader->Release();
    }
    // Create Pixel Shader.
    Result = Renderer->Device->CreatePixelShader(PixelShaderCompiled->GetBufferPointer(),
                                       PixelShaderCompiled->GetBufferSize(), 0,
                                       &Shader->PixelShader); 
    PixelShaderCompiled->Release();
    return Shader;
}

CREATE_MESH(CreateMesh)
{
    mesh *Mesh = PushStruct(Arena, mesh);
    Mesh->Vertices = PushArray(Arena, r32, VerticesCount);
    Mesh->VerticesCount = VerticesCount;
    memcpy(Mesh->Vertices, Vertices, VerticesCount*sizeof(r32));
    D3D11_BUFFER_DESC VertexDesc = {};
    VertexDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexDesc.ByteWidth = sizeof(r32) * Mesh->VerticesCount;
    // pass the buffer data (Vertices).
    D3D11_SUBRESOURCE_DATA ResourceData = {};
    ResourceData.pSysMem = Mesh->Vertices;
    // Create the VertexBuffer
    HRESULT Result = Renderer->Device->CreateBuffer(&VertexDesc, &ResourceData, &Mesh->VertexBuffer);
    return Mesh;
}

CREATE_TEXTURE(CreateTexture)
{
    texture *Texture = PushStruct(Arena, texture);
    bitmap Bitmap = LoadBMP(FileName, Arena);
    D3D11_SUBRESOURCE_DATA Data = {};
    Data.pSysMem = (void *)Bitmap.Pixels;
    Data.SysMemPitch = Bitmap.Width*sizeof(u32);
    Data.SysMemSlicePitch = 0;

    D3D11_TEXTURE2D_DESC TextureDesc = {}; 
    TextureDesc.Width = Bitmap.Width;
    TextureDesc.Height = Bitmap.Height;
    TextureDesc.MipLevels = 0; // generate a full set of subtextures
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D *TempTexture;
    HRESULT Result = Renderer->Device->CreateTexture2D(&TextureDesc, 0, &TempTexture);
    if(SUCCEEDED(Result))
    {
        OutputDebugString("SUCCEEDED Creating texture\n");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc = {};
    ShaderResourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;
    ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    ShaderResourceDesc.Texture2D.MipLevels = -1;
    ShaderResourceDesc.Texture2D.MostDetailedMip = 0;
    Result = Renderer->Device->CreateShaderResourceView(TempTexture, &ShaderResourceDesc, &Texture->ColorMap);
    if(SUCCEEDED(Result))
    {
        OutputDebugString("SUCCEEDED Creating Shader resource view\n");
    }
    Renderer->RenderContext->UpdateSubresource(TempTexture, 0, 0, Data.pSysMem, Data.SysMemPitch, 0);
    Renderer->RenderContext->GenerateMips(Texture->ColorMap);

    TempTexture->Release();

    D3D11_SAMPLER_DESC ColorMapDesc = {};
    ColorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    ColorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    ColorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    ColorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    ColorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_MIN_MAG_MIP_LINEAR | D3D11_FILTER_MIN_MAG_MIP_POINT
    ColorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    Result = Renderer->Device->CreateSamplerState(&ColorMapDesc, &Texture->ColorMapSampler);
    if(SUCCEEDED(Result))
    {
        OutputDebugString("SUCCEEDED Creating sampler state\n");
    }
    Texture->Pixels = Bitmap.Pixels;
    Texture->Width = Bitmap.Width;
    Texture->Height = Bitmap.Height;
    return Texture;
}

CREATE_CONST_BUFFER(CreateConstBuffer)
{
    const_buffer *ConstBuffer = PushStruct(Arena, const_buffer);
    D3D11_BUFFER_DESC ConstantBufferDesc;
    ConstantBufferDesc.ByteWidth = DataSize + (16 - (DataSize % 16));
    ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ConstantBufferDesc.MiscFlags = 0;
    ConstantBufferDesc.StructureByteStride = 0;
    Renderer->Device->CreateBuffer(&ConstantBufferDesc, 0, &ConstBuffer->Buffer);
    return ConstBuffer;
}

MAP_CONST_BUFFER(MapConstBuffer)
{
    D3D11_MAPPED_SUBRESOURCE GPUConstantBufferData = {};
    Renderer->RenderContext->Map(ConstBuffer->Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GPUConstantBufferData);
    memcpy(GPUConstantBufferData.pData, BufferData, DataSize);
    Renderer->RenderContext->Unmap(ConstBuffer->Buffer, 0);
    Renderer->RenderContext->VSSetConstantBuffers(Register, 1, &ConstBuffer->Buffer);
}

RENDER_MESH(RenderMesh)
{
    u32 Stride =  5 * sizeof(r32);
    u32 Offset = 0;
    Renderer->RenderContext->IASetInputLayout(Shader->InputLayout);
    Renderer->RenderContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Stride, &Offset);
    Renderer->RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer->RenderContext->VSSetShader(Shader->VertexShader, 0, 0);
    Renderer->RenderContext->PSSetShader(Shader->PixelShader,  0, 0);
    Renderer->RenderContext->PSSetShaderResources(0, 1, &Texture->ColorMap);
    Renderer->RenderContext->PSSetSamplers(0, 1, &Texture->ColorMapSampler);
    Renderer->RenderContext->Draw(Mesh->VerticesCount/5, 0);
}

RENDER_FRAME(RenderFrame)
{
    frame_const_buffer ConstBufferData = {};
    ConstBufferData.TexSize.X = Texture->Width;
    ConstBufferData.TexSize.Y = Texture->Height;
    ConstBufferData.TileSize.X = TileWidth;
    ConstBufferData.TileSize.Y = TileHeight;
    ConstBufferData.Frame.X = FrameX;
    ConstBufferData.Frame.Y = FrameY;
    MapConstBuffer(Renderer, ConstBuffer, &ConstBufferData, sizeof(frame_const_buffer), 1);
    RenderMesh(Renderer, Mesh, Shader, Texture);
}
