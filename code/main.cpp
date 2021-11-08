struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    v3 Color;
};

struct frame_const_buffer
{
    v2 TexSize;
    v2 TileSize;
    v2 Frame;
};

struct button
{
    bool IsDown;
    bool WasDown; 
};

#define NUMB_OF_BUTTONS 6

union buttons
{
    struct
    {
        button Up;
        button Down;
        button Left;
        button Right;
        button Start;
        button Back;
    };
    button Buttons[NUMB_OF_BUTTONS];
};

struct input
{
    buttons *Buttons;
};

struct game_state
{
    window *Window;
    renderer *Renderer;

    arena EngineArena;

    shader *Shader;
    const_buffer *ConstBuffer;
    const_buffer *FrameConstBuffer;
    mesh *Mesh;
    texture *Texture;
};

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(10));

    GameState->Window = PlatformCreateWindow(0, 0, 800, 600, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);

    GameState->Shader = CompileShadersFromFile(GameState->Renderer,
                                               "../code/shaders/test_vert.hlsl",  
                                               "../code/shaders/test_frag.hlsl",
                                               &GameState->EngineArena);

    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    
    vs_constant_buffer ConstBufferData = {};
    ConstBufferData.Projection = OrthogonalProjMat4(800, 600, 1.0f, 100.0f);
    ConstBufferData.World = IdentityMat4();
    ConstBufferData.Color = {1.0f, 1.0f, 1.0f};
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &ConstBufferData, sizeof(vs_constant_buffer), 0);


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
    GameState->Mesh = CreateMesh(GameState->Renderer, Vertices, ArrayCount(Vertices), &GameState->EngineArena);
    GameState->Texture = CreateTexture(GameState->Renderer, "../data/town_tileset.bmp", &GameState->EngineArena);

}

void GameUpdateAndRender(memory *Memory, input *Input, r32 DeltaTime)
{
    game_state *GameState = (game_state *)Memory->Data;
    
    if(Input->Buttons->Start.IsDown != Input->Buttons->Start.WasDown)
    {
        if(Input->Buttons->Start.IsDown)
        {
            OutputDebugString("Start\n"); 
        }
    }    

    vs_constant_buffer ConstBufferData= {};
    ConstBufferData.Projection = OrthogonalProjMat4(800, 600, 1.0f, 100.0f);
    ConstBufferData.World = ScaleMat4({16*4, 16*4, 0.0f});
    ConstBufferData.Color = {1.0f, 0.0f, 0.0f};
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &ConstBufferData, sizeof(vs_constant_buffer), 0); 
    RenderFrame(GameState->Renderer, GameState->Mesh, GameState->Shader, GameState->Texture,
                GameState->FrameConstBuffer, 16, 16, 0, 6);
}
