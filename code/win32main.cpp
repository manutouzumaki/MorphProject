#include <Windows.h>
#include <Windowsx.h>
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
    AdjustWindowRect( &Rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, false ); //WS_OVERLAPPEDWINDOW WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU

    Window->Window = CreateWindowA(Name,
                                   Name,
                                   WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
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
    Memory.Size = Megabytes(512);
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

    input Input = {};
    buttons OldButtons = {};
    buttons ActualButtons = {};
    mouse_buttons OldMouseButtons = {};
    mouse_buttons ActualMouseButtons = {};

    LARGE_INTEGER LastCount = {};
    QueryPerformanceCounter(&LastCount);

    GlobalRunning = true;
    ShowWindow(Window->Window, ShowCmd);

    while(GlobalRunning)
    {
        LARGE_INTEGER WorkCount = {};
        QueryPerformanceCounter(&WorkCount);
        u64 DeltaWorkCount = WorkCount.QuadPart - LastCount.QuadPart;            
        r32 SecondElapseForFrame = ((r32)DeltaWorkCount / (r32)Frequency.QuadPart);
#if 1
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
#endif
        LARGE_INTEGER ActualCount = {};
        QueryPerformanceCounter(&ActualCount);
        u64 DeltaCount = ActualCount.QuadPart - LastCount.QuadPart;
        r32 DeltaTime = ((r32)DeltaCount / (r32)Frequency.QuadPart);
        LastCount = ActualCount;
        
        MSG Message = {}; // initialize to zero
        while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            switch(Message.message)
            {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                {
                    DWORD VKCode = (DWORD)Message.wParam;
                    if(VKCode == 'W' || VKCode == VK_UP)
                    {
                        ActualButtons.Up.IsDown = true;
                    }
                    if(VKCode == 'S' || VKCode == VK_DOWN)
                    { 
                        ActualButtons.Down.IsDown = true;
                    }
                    if(VKCode == 'A' || VKCode == VK_LEFT)
                    {
                        ActualButtons.Left.IsDown = true;
                    }
                    if(VKCode == 'D' || VKCode == VK_RIGHT)
                    {
                        ActualButtons.Right.IsDown = true;
                    }
                    if(VKCode == 'E')
                    {
                        ActualButtons.Debug.IsDown = true;
                    }
                    if(VKCode == VK_RETURN || VKCode == VK_SPACE)
                    {
                        ActualButtons.Start.IsDown = true;
                    }
                    if(VKCode == VK_BACK || VKCode == VK_ESCAPE)
                    {
                        ActualButtons.Back.IsDown = true; 
                    }
                }break;
                case WM_KEYUP:
                case WM_SYSKEYUP:
                {
                    DWORD VKCode = (DWORD)Message.wParam;
                    if(VKCode == 'W' || VKCode == VK_UP)
                    {
                        ActualButtons.Up.IsDown = false;
                    }
                    if(VKCode == 'S' || VKCode == VK_DOWN)
                    { 
                        ActualButtons.Down.IsDown = false;
                    }
                    if(VKCode == 'A' || VKCode == VK_LEFT)
                    {
                        ActualButtons.Left.IsDown = false;
                    }
                    if(VKCode == 'D' || VKCode == VK_RIGHT)
                    {
                        ActualButtons.Right.IsDown = false;
                    }
                    if(VKCode == 'E')
                    {
                        ActualButtons.Debug.IsDown = false;
                    }
                    if(VKCode == VK_RETURN || VKCode == VK_SPACE)
                    {
                        ActualButtons.Start.IsDown = false;
                    }
                    if(VKCode == VK_BACK || VKCode == VK_ESCAPE)
                    {
                        ActualButtons.Back.IsDown = false;
                    }
                }break;
                case WM_MOUSEMOVE:
                {
                    Input.MouseX = (int)GET_X_LPARAM(Message.lParam); 
                    Input.MouseY = WND_HEIGHT - (int)GET_Y_LPARAM(Message.lParam); 
                }break;
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                {
                    ActualMouseButtons.Left.IsDown = ((Message.wParam & MK_LBUTTON) != 0);
                    ActualMouseButtons.Middle.IsDown = ((Message.wParam & MK_MBUTTON) != 0);
                    ActualMouseButtons.Right.IsDown = ((Message.wParam & MK_RBUTTON) != 0);
                }break;
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }break;
            }
        }
        for(i32 Index = 0;
            Index < NUMB_OF_BUTTONS;
            ++Index)
        {
            ActualButtons.Buttons[Index].WasDown = false;
            if(OldButtons.Buttons[Index].IsDown)
            {
                ActualButtons.Buttons[Index].WasDown = true;
            } 
        }
        for(i32 Index = 0;
            Index < NUMB_OF_MOUSE_BUTTONS;
            ++Index)
        {
            ActualMouseButtons.Buttons[Index].WasDown = false;
            if(OldMouseButtons.Buttons[Index].IsDown)
            {
                ActualMouseButtons.Buttons[Index].WasDown = true;
            }
        }
        // TODO(manuto): Update and Render
        // Update
        Input.Buttons = &ActualButtons;
        Input.MouseButtons = &ActualMouseButtons;
        // Render
        r32 ClearColor[4] = {0.0f, 0.2f, 0.5f, 1.0f};
        Renderer->RenderContext->ClearRenderTargetView(Renderer->BackBuffer, ClearColor);
        GameUpdateAndRender(&Memory, &Input, DeltaTime);
        Renderer->SwapChain->Present(0, 0);
        OldButtons = ActualButtons;
        OldMouseButtons = ActualMouseButtons;
    }
    if(Renderer->BackBuffer) Renderer->BackBuffer->Release();
    if(Renderer->SwapChain) Renderer->SwapChain->Release();
    if(Renderer->RenderContext) Renderer->RenderContext->Release();
    if(Renderer->Device) Renderer->Device->Release();
    return(0);
}

