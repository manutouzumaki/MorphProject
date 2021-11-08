struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    v3 Color;
};

struct game_state
{
    window *Window;
    renderer *Renderer;

    arena EngineArena;

    shader *Shader;
    const_buffer *ConstBuffer;
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

    vs_constant_buffer ConstBufferData= {};
    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    ConstBufferData.Projection = OrthogonalProjMat4(800, 600, 1.0f, 100.0f);
    ConstBufferData.World = IdentityMat4();
    ConstBufferData.Color = {1.0f, 1.0f, 1.0f};
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &ConstBufferData, sizeof(vs_constant_buffer));

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
    GameState->Texture = CreateTexture(GameState->Renderer, "../data/DOGGIE.bmp", &GameState->EngineArena);

}

void GameUpdateAndRender(memory *Memory, r32 DeltaTime)
{
    game_state *GameState = (game_state *)Memory->Data;
    
    vs_constant_buffer ConstBufferData= {};
    ConstBufferData.Projection = OrthogonalProjMat4(800, 600, 1.0f, 100.0f);
    ConstBufferData.World = ScaleMat4({100, 100, 0.0f});
    ConstBufferData.Color = {1.0f, 0.0f, 0.0f};
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &ConstBufferData, sizeof(vs_constant_buffer));  

    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->Shader, GameState->Texture);
}
