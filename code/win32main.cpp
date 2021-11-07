#include "platform.h"
#include "utility.h"
#include "math.h"
#include "arena.h"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

// Vertex Shader
static char *VertexShaderSource =
"cbuffer CBufferProjection : register(b0)\n"
"{\n"
"   matrix Projection;\n"
"   matrix World;\n"
"   float3 Color;\n"
"};\n"
"struct VS_Input\n"
"{\n"
"   float4 pos : POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"};\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"   float3 tex1 : TEXCOORD1;\n"
"};\n"
"PS_Input VS_Main( VS_Input vertex )\n"
"{\n"
"   PS_Input vsOut = ( PS_Input )0;\n"
"   vsOut.pos = mul(vertex.pos, World);\n"
"   vsOut.pos = mul(vsOut.pos, Projection);\n"
"   vsOut.tex0 = vertex.tex0;\n"
"   vsOut.tex1 = Color;\n"
"   return vsOut;\n"
"}\0";

// Pixel Shader
static char *PixelShaderSource  =
"Texture2D colorMap_ : register( t0 );\n"
"SamplerState colorSampler_ : register( s0 );\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"   float3 tex1 : TEXCOORD1;\n"
"};\n"
"float4 PS_Main( PS_Input frag ) : SV_TARGET\n"
"{\n"
"   return float4(frag.tex1.rgb, 1);\n"
"}\0";

struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    v3 Color;
};

vs_constant_buffer GlobalConstBufferData;
static ID3D11Buffer *GlobalConstBuffer = 0;
ID3D11Buffer *VertexBuffer = 0;

static ID3D11VertexShader *VertexShader = 0;
static ID3D11PixelShader  *PixelShader  = 0;
static ID3D11InputLayout  *InputLayout  = 0;

static bool GlobalRunning;

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = {};
    switch(Message)
    {
        case WM_CLOSE:
        {
            GlobalRunning = false;
        }break;
        case WM_DESTROY:
        {
            GlobalRunning = false;
        }break;
        default:
        {
           Result = DefWindowProcA(Window, Message, WParam, LParam);
        }break; 
    }
    return Result;
}

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

void SetProjectionMat4(ID3D11DeviceContext *RenderContext, mat4 Projection)
{
    GlobalConstBufferData.Projection = Projection;
    D3D11_MAPPED_SUBRESOURCE GPUConstantBufferData = {};
    RenderContext->Map(GlobalConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GPUConstantBufferData);
    memcpy(GPUConstantBufferData.pData, &GlobalConstBufferData, sizeof(vs_constant_buffer));
    RenderContext->Unmap(GlobalConstBuffer, 0);
    RenderContext->VSSetConstantBuffers( 0, 1, &GlobalConstBuffer);
}

void SetWorldMat4(ID3D11DeviceContext *RenderContext, mat4 World)
{
    GlobalConstBufferData.World = World;
    D3D11_MAPPED_SUBRESOURCE GPUConstantBufferData = {};
    RenderContext->Map(GlobalConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GPUConstantBufferData);
    memcpy(GPUConstantBufferData.pData, &GlobalConstBufferData, sizeof(vs_constant_buffer));
    RenderContext->Unmap(GlobalConstBuffer, 0);
    RenderContext->VSSetConstantBuffers( 0, 1, &GlobalConstBuffer);
}

void SetColor(ID3D11DeviceContext *RenderContext, r32 R, r32 G, r32 B)
{
    v3 Color = {R, G, B};
    GlobalConstBufferData.Color = Color;
    D3D11_MAPPED_SUBRESOURCE GPUConstantBufferData = {};
    RenderContext->Map(GlobalConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GPUConstantBufferData);
    memcpy(GPUConstantBufferData.pData, &GlobalConstBufferData, sizeof(vs_constant_buffer));
    RenderContext->Unmap(GlobalConstBuffer, 0);
    RenderContext->VSSetConstantBuffers( 0, 1, &GlobalConstBuffer); 
}

i32 WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, i32 ShowCmd)
{
    WNDCLASSEX WindowClass = { 0 };
    WindowClass.cbSize = sizeof( WNDCLASSEX ) ;
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor( 0, IDC_ARROW );
    WindowClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    WindowClass.lpszMenuName = 0;
    WindowClass.lpszClassName = "MorphProject";

    RegisterClassEx(&WindowClass);

    RECT Rect = { 0, 0, 640, 480 };
    AdjustWindowRect( &Rect, WS_OVERLAPPEDWINDOW, FALSE );

    HWND Window = CreateWindowA("MorphProject",
                                "MorphProject",
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                Rect.right - Rect.left,
                                Rect.bottom - Rect.top,
                                0, 0, Instance, 0);
    if(!Window)
    {
        // TODO: Logger...
        return 1;
    }

    LARGE_INTEGER Frequency = {};
    QueryPerformanceFrequency(&Frequency);
    bool SleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    r32 FPS = 60.0f;
    r32 TARGET_FPS = (1.0f / FPS);

    RECT ClientDimensions = {};
    GetClientRect(Window, &ClientDimensions);
    u32 Width  = ClientDimensions.right - ClientDimensions.left;
    u32 Height = ClientDimensions.bottom - ClientDimensions.top;

    // Initialize DirectX
    ID3D11Device           *Device        = 0;
    ID3D11DeviceContext    *RenderContext = 0;
    IDXGISwapChain         *SwapChain     = 0;
    ID3D11RenderTargetView *BackBuffer    = 0;
    InitializeDirecX11(Window, &Device, &RenderContext, &SwapChain, &BackBuffer);
 
    // Create a vertex Buffer.
    r32 Vertices[] = 
    {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };

    // buffer description
    D3D11_BUFFER_DESC VertexDesc = {};
    VertexDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexDesc.ByteWidth = sizeof(r32) * 30;
    // pass the buffer data (Vertices).
    D3D11_SUBRESOURCE_DATA ResourceData = {};
    ResourceData.pSysMem = Vertices;
    // Create the VertexBuffer
    HRESULT Result = Device->CreateBuffer(&VertexDesc, &ResourceData, &VertexBuffer);

    // Compile and Load Shaders
    ID3DBlob *VertexShaderCompiled = 0;
    ID3DBlob *ErrorVertexShader    = 0;
    Result = D3DCompile((void *)VertexShaderSource,
                        (SIZE_T)StringLength(VertexShaderSource),
                        0, 0, 0, "VS_Main", "vs_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &VertexShaderCompiled, &ErrorVertexShader);
    if(ErrorVertexShader != 0)
    {
        ErrorVertexShader->Release();
    }

    ID3DBlob *PixelShaderCompiled = 0;
    ID3DBlob *ErrorPixelShader    = 0;
    Result = D3DCompile((void *)PixelShaderSource,
                        (SIZE_T)StringLength(PixelShaderSource),
                        0, 0, 0, "PS_Main", "ps_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &PixelShaderCompiled, &ErrorPixelShader);
    if(ErrorPixelShader != 0)
    {
        ErrorPixelShader->Release();
    }

    // Create the Vertex Shader.
    Result = Device->CreateVertexShader(VertexShaderCompiled->GetBufferPointer(),
                                        VertexShaderCompiled->GetBufferSize(), 0,
                                        &VertexShader);
    // Create the Input layout.
    D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    u32 TotalLayoutElements = ArrayCount(InputLayoutDesc);
    Result = Device->CreateInputLayout(InputLayoutDesc,
                                       TotalLayoutElements,
                                       VertexShaderCompiled->GetBufferPointer(),
                                       VertexShaderCompiled->GetBufferSize(),
                                       &InputLayout);
    // Create Pixel Shader.
    Result = Device->CreatePixelShader(PixelShaderCompiled->GetBufferPointer(),
                                       PixelShaderCompiled->GetBufferSize(), 0,
                                       &PixelShader); 
    VertexShaderCompiled->Release();
    PixelShaderCompiled->Release();
    
    // Create constant Buffers and  
    D3D11_BUFFER_DESC ConstantBufferDesc;
    ConstantBufferDesc.ByteWidth = sizeof(vs_constant_buffer) + (16 - (sizeof(vs_constant_buffer) % 16));
    ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ConstantBufferDesc.MiscFlags = 0;
    ConstantBufferDesc.StructureByteStride = 0;
    Result = Device->CreateBuffer(&ConstantBufferDesc, 0, &GlobalConstBuffer);
    // Set World And Projection Matrix
    mat4 Projection = OrthogonalProjMat4(Width, Height, 1.0f, 100.0f);
    SetProjectionMat4(RenderContext, Projection);
    mat4 World = ScaleMat4({100.0f, 100.0f, 0.0f});
    SetWorldMat4(RenderContext, World);
    SetColor(RenderContext, 1.0f, 1.0f, 1.0f);

    // Alloc all the memory the program is going to use
    memory Memory = {};
    Memory.Size = Megabytes(256);
    Memory.Data = VirtualAlloc(0, Memory.Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 

    GlobalRunning = true;
    ShowWindow(Window, ShowCmd);

    LARGE_INTEGER LastCount = {};
    QueryPerformanceCounter(&LastCount);
    while(GlobalRunning)
    {
        LARGE_INTEGER WorkCount = {};
        QueryPerformanceCounter(&WorkCount);
        u64 DeltaWorkCount = WorkCount.QuadPart - LastCount.QuadPart;            
        r32 SecondElapseForFrame = ((r32)DeltaWorkCount / (r32)Frequency.QuadPart);
        while(SecondElapseForFrame < TARGET_FPS)
        {
            if(SleepIsGranular)
            {
                DWORD SleepMS = (DWORD)(1000.0f*(TARGET_FPS - SecondElapseForFrame));
                if(SleepMS > 0)
                {
                    Sleep(SleepMS);
                }
                QueryPerformanceCounter(&WorkCount);
                DeltaWorkCount = WorkCount.QuadPart - LastCount.QuadPart;            
                SecondElapseForFrame = ((r32)DeltaWorkCount / (r32)Frequency.QuadPart);
            }
        } 
        LARGE_INTEGER ActualCount = {};
        QueryPerformanceCounter(&ActualCount);
        u64 DeltaCount = ActualCount.QuadPart - LastCount.QuadPart;            
        r32 DeltaTime = ((r32)DeltaCount / (r32)Frequency.QuadPart);
        LastCount = ActualCount;
        
        MSG Message = {}; // initialize to zero
        while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        // TODO(manuto): Update and Render
        r32 ClearColor[4] = {0.0f, 0.2f, 0.5f, 1.0f};
        RenderContext->ClearRenderTargetView(BackBuffer, ClearColor);
        // TODO: render Quad...
        mat4 World = ScaleMat4({100, 100, 0.0f});;
        SetWorldMat4(RenderContext, World);
        SetColor(RenderContext, 0.0f, 1.0f, 0.5f);
        u32 Stride =  5*sizeof(r32);
        u32 Offset = 0;
        RenderContext->IASetInputLayout(InputLayout);
        RenderContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
        RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        RenderContext->VSSetShader(VertexShader, 0, 0);
        RenderContext->PSSetShader(PixelShader,  0, 0);
        RenderContext->Draw(6, 0);

        SwapChain->Present(0, 0);
    }

    if(BackBuffer) BackBuffer->Release();
    if(SwapChain) SwapChain->Release();
    if(RenderContext) RenderContext->Release();
    if(Device) Device->Release();

    return(0);
}

