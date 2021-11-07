#include <Windows.h>

#include "platform.h"
#include "math.h"
#include "arena.h"
#include "utility.h"
#include "directx.h"

struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    v3 Color;
};

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
 
    // Alloc all the memory the program is going to use
    memory Memory = {};
    Memory.Size = Megabytes(256);
    Memory.Data = VirtualAlloc(0, Memory.Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    arena EngineArena = {};
    InitArena(&Memory, &EngineArena, Megabytes(10));

    shader *Shader = CompileShadersFromFile(Device,
                                            "../code/shaders/test_vert.hlsl",  
                                            "../code/shaders/test_frag.hlsl",
                                            &EngineArena);
    const_buffer *ConstBuffer;
    vs_constant_buffer ConstBufferData= {};
    CreateConstBuffer(Device, ConstBuffer, vs_constant_buffer, &EngineArena);
    ConstBufferData.Projection = OrthogonalProjMat4(Width, Height, 1.0f, 100.0f);
    ConstBufferData.World = IdentityMat4();
    ConstBufferData.Color = {1.0f, 1.0f, 1.0f};
    MapConstBuffer(RenderContext, ConstBuffer, ConstBufferData, vs_constant_buffer);

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
    mesh *Mesh = CreateMesh(Device, Vertices, ArrayCount(Vertices), &EngineArena);
    texture *Texture = CreateTexture(Device, RenderContext, "../data/DOGGIE.bmp", &EngineArena);

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
        ConstBufferData.World = ScaleMat4({100, 100, 0.0f});
        ConstBufferData.Color = {1.0f, 0.0f, 0.0f};
        MapConstBuffer(RenderContext, ConstBuffer, ConstBufferData, vs_constant_buffer);
        
        u32 Stride =  5*sizeof(r32);
        u32 Offset = 0;
        RenderContext->IASetInputLayout(Shader->InputLayout);
        RenderContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Stride, &Offset);
        RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        RenderContext->VSSetShader(Shader->VertexShader, 0, 0);
        RenderContext->PSSetShader(Shader->PixelShader,  0, 0);
        RenderContext->PSSetShaderResources(0, 1, &Texture->ColorMap);
        RenderContext->PSSetSamplers(0, 1, &Texture->ColorMapSampler);
        RenderContext->Draw(Mesh->VerticesCount/5, 0);

        SwapChain->Present(0, 0);
    }

    if(BackBuffer) BackBuffer->Release();
    if(SwapChain) SwapChain->Release();
    if(RenderContext) RenderContext->Release();
    if(Device) Device->Release();

    return(0);
}

