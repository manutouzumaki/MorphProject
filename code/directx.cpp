void InitializeD3D11(HWND Window, u32 Width, u32 Height, renderer *Renderer)
{
    // TODO: after create the window we have to initialize d3d11
    // define the device types and feature level we want to check for
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
    u32 DriversTypesCount = ArrayCount(DriverTypes);
    u32 FeatureLevelsCount = ArrayCount(FeatureLevels);

    HRESULT Result = {}; // initialize to zero
    // create device, rendering context, swap chain and BackBuffer
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.BufferDesc.Width = Width;
    SwapChainDesc.BufferDesc.Height = Height;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.OutputWindow = Window;
    SwapChainDesc.Windowed = true;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    // try to create the device and swap chain
    D3D_FEATURE_LEVEL FeatureLevel = {};
    D3D_DRIVER_TYPE DriverType = {};
    for(u32 Driver = 0;
        Driver < DriversTypesCount;
        ++Driver)
    {
        Result = D3D11CreateDeviceAndSwapChain(0, DriverTypes[Driver], 0, 0,
                                               FeatureLevels, FeatureLevelsCount,
                                               D3D11_SDK_VERSION, &SwapChainDesc,
                                               &Renderer->SwapChain, &Renderer->Device, &FeatureLevel,
                                               &Renderer->RenderContext);
        if(SUCCEEDED(Result))
        {
            DriverType = DriverTypes[Driver];
            break;
        } 
    }
    // create render target or BackBuffer
    ID3D11Texture2D *BackBufferTexture = 0;
    Result = Renderer->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
    Result = Renderer->Device->CreateRenderTargetView(BackBufferTexture, 0, &Renderer->BackBuffer);
    if(BackBufferTexture)
    {
        BackBufferTexture->Release();
    }
    
    // TODO: test if we need to create a viewport...
    D3D11_VIEWPORT Viewport;
    Viewport.Width = (r32)Width;
    Viewport.Height = (r32)Height;
    Viewport.MinDepth = 0.0f;
    Viewport.MaxDepth = 1.0f;
    Viewport.TopLeftX = 0.0f;
    Viewport.TopLeftY = 0.0f;
    Renderer->RenderContext->RSSetViewports(1, &Viewport);

}

void UninitializeD3D11(renderer *Renderer)
{
    if(Renderer->BackBuffer) Renderer->BackBuffer->Release();
    if(Renderer->SwapChain) Renderer->SwapChain->Release();
    if(Renderer->RenderContext) Renderer->RenderContext->Release();
    if(Renderer->Device) Renderer->Device->Release();
}

ID3DBlob *D3D11CompileShader(char *FileName, char *MainFuncName, char *ShaderVersion, arena *Arena)
{
    char * FileData = (char *)PlatformReadEntireFile(FileName, 0, Arena); 
    ID3DBlob *ShaderCompiled = 0;
    ID3DBlob *ErrorShader    = 0;

    int Length = StringCount(FileData);

    HRESULT Result = D3DCompile((void *)FileData,
                                (size_t)StringCount(FileData),
                                0, 0, 0, MainFuncName, ShaderVersion,
                                D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                &ShaderCompiled, &ErrorShader);
    if(ErrorShader == 0)
    {
        return ShaderCompiled;
    }
    OutputDebugString((char *)ErrorShader->GetBufferPointer());
    ErrorShader->Release();
    return 0;
}

bool D3D11CreateVertexShader(ID3D11Device *Device, char *FileName, char *MainFuncName,
                             ID3D11VertexShader **VertexShader,
                             ID3D11InputLayout **InputLayout,
                             D3D11_INPUT_ELEMENT_DESC *InputLayoutDesc,
                             u32 TotalLayoutElements, arena *Arena)
{
    HRESULT Result;
    ID3DBlob *ShaderCompiled = D3D11CompileShader(FileName, MainFuncName, "vs_4_0", Arena);
    Result = Device->CreateVertexShader(ShaderCompiled->GetBufferPointer(),
                                        ShaderCompiled->GetBufferSize(), 0,
                                        VertexShader);
    if(!SUCCEEDED(Result))
    {
        return false;
    }
    Result = Device->CreateInputLayout(InputLayoutDesc,
                                       TotalLayoutElements,
                                       ShaderCompiled->GetBufferPointer(),
                                       ShaderCompiled->GetBufferSize(),
                                       InputLayout);
    if(!SUCCEEDED(Result))
    {
        return false;
    }
    ShaderCompiled->Release();
    OutputDebugString("VERTEX_SHADER::CREATED\n");
    return true;
}

bool D3D11CreatePixelShader(ID3D11Device *Device, char *FileName, char *MainFuncName,
                            ID3D11PixelShader **PixelShader, arena *Arena)
{
    HRESULT Result;
    ID3DBlob *ShaderCompiled = D3D11CompileShader(FileName, MainFuncName, "ps_4_0", Arena);
    Result = Device->CreatePixelShader(ShaderCompiled->GetBufferPointer(),
                                       ShaderCompiled->GetBufferSize(), 0,
                                       PixelShader);
    if(!SUCCEEDED(Result))
    {
        return false;
    }
    ShaderCompiled->Release();
    OutputDebugString("PIXEL_SHADER::CREATED\n");
    return true;
}
