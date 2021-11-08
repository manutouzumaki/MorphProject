#include <Windows.h>
#include "platform.h"
#include "main.cpp"

struct window
{
    HWND Window;
};

#include "directx.h"

static bool GlobalRunning;
static HINSTANCE GlobalInstace;

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

PLAFORM_CREATE_WINDOW(PlatformCreateWindow)
{
    window *Window = PushStruct(Arena, window);
    WNDCLASSEX WindowClass = { 0 };
    WindowClass.cbSize = sizeof( WNDCLASSEX ) ;
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = GlobalInstace;
    WindowClass.hCursor = LoadCursor( 0, IDC_ARROW );
    WindowClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    WindowClass.lpszMenuName = 0;
    WindowClass.lpszClassName = Name;

    RegisterClassEx(&WindowClass);

    RECT Rect = { (LONG)X, (LONG)Y, (LONG)Width, (LONG)Height };
    AdjustWindowRect( &Rect, WS_OVERLAPPEDWINDOW, false );

    Window->Window = CreateWindowA(Name,
                                   Name,
                                   WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   Rect.right - Rect.left,
                                   Rect.bottom - Rect.top,
                                   0, 0, GlobalInstace, 0);
    return Window;
}

i32 WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, i32 ShowCmd)
{
    GlobalInstace = Instance;
    
    // Alloc all the memory the program is going to use
    memory Memory = {};
    Memory.Size = Megabytes(256);
    Memory.Data = VirtualAlloc(0, Memory.Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    GameSetUp(&Memory);

    game_state *GameState = (game_state *)Memory.Data;
    window *Window = GameState->Window;
    renderer *Renderer = GameState->Renderer;
    if(!Window || !Renderer)
    {
        // TODO: Logger...
        return 1;
    }

    LARGE_INTEGER Frequency = {};
    QueryPerformanceFrequency(&Frequency);
    bool SleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    r32 FPS = 60.0f;
    r32 TARGET_FPS = (1.0f / FPS);

    GlobalRunning = true;
    ShowWindow(Window->Window, ShowCmd);

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
        Renderer->RenderContext->ClearRenderTargetView(Renderer->BackBuffer, ClearColor);
        
        GameUpdateAndRender(&Memory, DeltaTime);

        Renderer->SwapChain->Present(0, 0);
    }

    if(Renderer->BackBuffer) Renderer->BackBuffer->Release();
    if(Renderer->SwapChain) Renderer->SwapChain->Release();
    if(Renderer->RenderContext) Renderer->RenderContext->Release();
    if(Renderer->Device) Renderer->Device->Release();

    return(0);
}

