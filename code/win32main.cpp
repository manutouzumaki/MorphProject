#include "platform.h"
#include <Windows.h>

#include "directx.h"
#include "directx.cpp"

struct window
{
    HWND Win32Window;
    i32 Width;
    i32 Height;
};

// TODO: check for a better way to do this
struct wvp_const_buffer_data
{
    mat4 World;
    mat4 View;
    mat4 Proj; 
};

struct const_buffer
{
    ID3D11Buffer *Buffer;
};

static bool GlobalRunning;
static HINSTANCE GlobalInstance;
static u32 WindowWidth = 800;
static u32 WindowHeight = 600;

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

PLATFORM_CREATE_WINDOW(PlatformCreateWindow)
{
    window *Window = PushStruct(Arena, window);
    // create a window to display all our staff
    // fisrt we need to define and register the class of the window we want to create
    WNDCLASSEX WndClass = {}; // initialize to zero
    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.style = CS_VREDRAW | CS_HREDRAW;
    WndClass.lpfnWndProc = WindowProc;
    WndClass.hInstance = GlobalInstance;
    WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(0, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WndClass.lpszClassName = Name;
    // register the class
    RegisterClassEx(&WndClass);
    
    // adjust the client area to get the correct size of the window
    RECT WndRect = {0, 0, (LONG)Width, (LONG)Height};
    // TODO: see if this affects d3d11 render buffer
    //AdjustWindowRect(&WndRect, WS_OVERLAPPEDWINDOW, false);
    
    // create the window
    Window->Win32Window = CreateWindow(WndClass.lpszClassName, WndClass.lpszClassName,
                                       WS_OVERLAPPEDWINDOW, 0, 0,
                                       WndRect.right - WndRect.left,
                                       WndRect.bottom - WndRect.top,
                                       0, 0, GlobalInstance, 0);
    Window->Width = Width;
    Window->Height = Height;
    return Window;
}

PLATFORM_CREATE_RENDERER(PlatformCreateRenderer)
{
    renderer *Renderer = PushStruct(Arena, renderer);
    InitializeD3D11(Window->Win32Window, Window->Width, Window->Height, Renderer);
    return Renderer;
}

PLATFORM_DELETE_RENDERER(PlatformDeleteRenderer)
{
    UninitializeD3D11(Renderer);
}

PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
    HANDLE FileHandle =  CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle == INVALID_HANDLE_VALUE)
    {
        OutputDebugString("ERROR::OPENING::FILE\n");
        return 0;
    }
    LARGE_INTEGER FileSize;
    GetFileSizeEx(FileHandle, &FileSize);
    void *FileBuffer = PushData(Arena, FileSize.QuadPart);
    if(FileSizePtr) *FileSizePtr = (size_t)FileSize.QuadPart;
    if(!ReadFile(FileHandle, FileBuffer, (DWORD)FileSize.QuadPart, 0, 0))
    {
        OutputDebugString("ERROR::READING::FILE\n");
        return 0;
    }
    return FileBuffer;
}

PLATFORM_CLEAR_BUFFER(PlatformClearBuffer)
{
    r32 ClearColor[4] = {R, G, B, A};
    Renderer->RenderContext->ClearRenderTargetView(Renderer->BackBuffer, ClearColor);
}

PLATFORM_PRESENT(PlatformPresent)
{
    Renderer->SwapChain->Present(0, 0);
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, i32 ShowCmd)
{
    GlobalInstance = Instance;

    memory Memory = {};
    Memory.Size = Megabytes(256);
    Memory.Data = VirtualAlloc(0, Memory.Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 
    GameSetUp(&Memory);

    game_state *GameSate = (game_state *)Memory.Data;
    window *Window = GameSate->Window;
    if(!Window)
    {
        // TODO: Logger...
        return(1);
    }

    // show the actual window
    ShowWindow(Window->Win32Window, ShowCmd);

    LARGE_INTEGER Frequency = {};
    QueryPerformanceFrequency(&Frequency);
    bool SleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    r32 FPS = 60.0f;
    r32 TARGET_FPS = (1.0f / FPS);

    GlobalRunning = true;

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
        GameUpdateAndRender(&Memory, DeltaTime);
    }
    GameShutDown(&Memory);
    return(0);
}
