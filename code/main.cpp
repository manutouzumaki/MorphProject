#include "main.h"

void SetWorldMat4(game_state *GameState, mat4 World)
{
    GameState->ConstBufferData.World = World;
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                   &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
}

void SetViewMat4(game_state *GameState, mat4 View)
{
    GameState->ConstBufferData.View = View;
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                   &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
}

void SetProjMat4(game_state *GameState, mat4 Proj)
{
    GameState->ConstBufferData.Projection = Proj;
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                   &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
}

#include "tilemap.cpp"
#include "map_editor.cpp"
#include "hero.cpp"

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(100));
    InitArena(Memory, &GameState->MapEditorArena, Megabytes(50));

    GameState->Window = PlatformCreateWindow(0, 0, WND_WIDTH, WND_HEIGHT, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);

    GameState->MainShader = CompileShadersFromFile(GameState->Renderer,
                                                   "../code/shaders/main_vert.hlsl",  
                                                   "../code/shaders/main_frag.hlsl",
                                                   &GameState->EngineArena);
    GameState->FrameShader = CompileShadersFromFile(GameState->Renderer,
                                                    "../code/shaders/frame_vert.hlsl",  
                                                    "../code/shaders/frame_frag.hlsl",
                                                    &GameState->EngineArena);
    GameState->UIShader = CompileShadersFromFile(GameState->Renderer,
                                                 "../code/shaders/ui_vert.hlsl",  
                                                 "../code/shaders/ui_frag.hlsl",
                                                 &GameState->EngineArena);
    GameState->UIFrameShader = CompileShadersFromFile(GameState->Renderer,
                                                      "../code/shaders/ui_frame_vert.hlsl",  
                                                      "../code/shaders/ui_frame_frag.hlsl",
                                                      &GameState->EngineArena);

    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    
    GameState->CamPosition = {400.0f, 300.0f, -0.1f};
    GameState->CamTarget = {400.0f, 300.0f, 0.0f};
    
    SetWorldMat4(GameState, IdentityMat4());
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

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
    GameState->MapTexture = CreateTexture(GameState->Renderer, "../data/town_tileset.bmp", &GameState->EngineArena);
    GameState->HeroTexture = CreateTexture(GameState->Renderer, "../data/walk_cycle.bmp", &GameState->EngineArena);

    SetEntityPosition(&GameState->HeroEntity, 8, 11);
    GameState->HeroEntity.Facing = BIT(DOWN);


    // TODO(manuto): Initialize Editor...
    InitEditor(&GameState->Editor, GameState->MapTexture, 16, 16, &GameState->MapEditorArena, GameState);

    GameState->AppState = GAME_STATE;
}

void GameUpdateAndRender(memory *Memory, input *Input, r32 DeltaTime)
{
    game_state *GameState = (game_state *)Memory->Data;
    // TODO(manuto): Update...
    if(Input->Buttons->Debug.IsDown != Input->Buttons->Debug.WasDown)
    {
        if(Input->Buttons->Debug.IsDown)
        {
            if(GameState->AppState == GAME_STATE)
            {
                GameState->AppState = EDITOR_STATE;
            }
            else if(GameState->AppState == EDITOR_STATE)
            {
                GameState->AppState = GAME_STATE;
                SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
            }
        }
    }
    
    if(GameState->AppState == GAME_STATE)
    {
        GetHeroInput(Input ,&GameState->HeroEntity);
        MoveEntity(&GameState->HeroEntity, DeltaTime);

        GameState->CamPosition.X = GameState->HeroEntity.Position.X;
        GameState->CamPosition.Y = GameState->HeroEntity.Position.Y;
        GameState->CamTarget.X = GameState->HeroEntity.Position.X;
        GameState->CamTarget.Y = GameState->HeroEntity.Position.Y;
        SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

        // TODO(manuto): Render...    
        for(i32 Y = 0;
            Y < 16;
            ++Y)
        {
            for(i32 X = 0;
                X < 16;
                ++X)
            {
                i32 Index = Y * 16 + X;
                u32 TileSheetCols = 176 / 16;
                u32 XFrame = Tiles1[Index] % TileSheetCols;
                u32 YFrame = Tiles1[Index] / TileSheetCols;  
                
                mat4 Scale = ScaleMat4({16, 16, 0.0f});
                mat4 Trans = TranslationMat4({16.0f * X, 16.0f * Y, 0.0f});
                SetWorldMat4(GameState, Trans * Scale);
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, 16, 16, XFrame, YFrame);
            }
        } 

        mat4 Scale = ScaleMat4({16, 24, 0.0f});
        mat4 Trans = TranslationMat4({GameState->HeroEntity.Position.X, GameState->HeroEntity.Position.Y, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->HeroTexture,
                    GameState->FrameConstBuffer, 16, 24, GameState->HeroEntity.Frame, 0);
    }
    
    if(GameState->AppState == EDITOR_STATE)
    {
        UpdateAndRenderEditor(GameState, Input, DeltaTime);
    }
}
