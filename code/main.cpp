#include "main.h"

void GameSetUp(memory *Memory)
{
    game_state *GameSate = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    // Initialize all the arenas
    InitArena(Memory, &GameSate->EngineArena, Kilobytes(1));
    InitArena(Memory, &GameSate->ConstBufferArena, Megabytes(10));
    
    // TODO(manuto):
    // create window and renderer
    // ...
    GameSate->Window = PlatformCreateWindow(0, 0, 800, 600, "Morph Project", &GameSate->EngineArena);
    if(!GameSate->Window) 
    {
        // TODO: Logger...
    }
    GameSate->Renderer = PlatformCreateRenderer(GameSate->Window, &GameSate->EngineArena);
    if(!GameSate->Renderer)
    {
        // TODO: Logger...
    }

    // Initialize const buffers
    CreateConstantBuffer(GameSate->WvpConstBuffer, wvp_const_buffer_data, GameSate->Renderer, &GameSate->ConstBufferArena);
    // map const buffer 
    wvp_const_buffer_data WvpData = {};
    WvpData.World = IdentityMat4();
    WvpData.View = ViewMat4({0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    WvpData.Proj = OrthogonalProjMat4(800, 600, 0.1f, 100.0f);
    MapConstantBuffer(GameSate->Renderer, WvpData, wvp_const_buffer_data, GameSate->WvpConstBuffer, 0);
}

void GameUpdateAndRender(memory *Memory, r32 DeltaTime)
{
    game_state *GameSate = (game_state *)Memory->Data;
    PlatformClearBuffer(0.5f, 0.5f, 0.1f, 1.0f, GameSate->Renderer);

    // TODO: Update and Render the game...

    PlatformPresent(GameSate->Renderer);
}

void GameShutDown(memory *Memory)
{
    game_state *GameSate = (game_state *)Memory->Data;
    PlatformDeleteRenderer(GameSate->Renderer);
}
