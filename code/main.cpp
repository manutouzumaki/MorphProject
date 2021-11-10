#include "tilemap.h"
#include "map_editor.h"
#include "hero.h"

struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    mat4 View;
};

struct frame_const_buffer
{
    v2 TexSize;
    v2 TileSize;
    v2 Frame;
};

enum app_state
{
    GAME_STATE,
    EDITOR_STATE
};

struct game_state
{
    window *Window;
    renderer *Renderer;

    app_state AppState; 

    arena EngineArena;

    const_buffer *ConstBuffer;
    const_buffer *FrameConstBuffer;
    vs_constant_buffer ConstBufferData= {};
    
    shader *MainShader;
    shader *FrameShader;
    shader *UIShader;

    mesh *Mesh;
    texture *MapTexture;
    texture *HeroTexture;

    v3 CamPosition;
    v3 CamTarget;

    entity HeroEntity;


    editor Editor;
};

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(100));

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


    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    
    GameState->CamPosition = {400.0f, 300.0f, -0.1f};
    GameState->CamTarget = {400.0f, 300.0f, 0.0f};

    GameState->ConstBufferData.Projection = OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f);
    GameState->ConstBufferData.World = IdentityMat4();
    GameState->ConstBufferData.View = ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f});
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);

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
    InitEditor(&GameState->Editor, GameState->MapTexture, 16, 16);

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
        GameState->ConstBufferData.View = ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f});
        MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);

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
                GameState->ConstBufferData.World = Trans * Scale;
                MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                               &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, 16, 16, XFrame, YFrame);
            }
        } 

        mat4 Scale = ScaleMat4({16, 24, 0.0f});
        mat4 Trans = TranslationMat4({GameState->HeroEntity.Position.X, GameState->HeroEntity.Position.Y, 0.0f});
        GameState->ConstBufferData.World = Trans * Scale;
        MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                       &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->HeroTexture,
                    GameState->FrameConstBuffer, 16, 24, GameState->HeroEntity.Frame, 0);
    }
    
    if(GameState->AppState == EDITOR_STATE)
    { 
        float CameraSpeed = 200.0f;
        if(Input->Buttons->Up.IsDown)
        {
            GameState->CamPosition.Y += CameraSpeed * DeltaTime; 
        }
        if(Input->Buttons->Down.IsDown)
        {
            GameState->CamPosition.Y -= CameraSpeed * DeltaTime; 
        }
        if(Input->Buttons->Left.IsDown)
        { 
            GameState->CamPosition.X -= CameraSpeed * DeltaTime; 
        }
        if(Input->Buttons->Right.IsDown)
        { 
            GameState->CamPosition.X += CameraSpeed * DeltaTime; 
        }
        GameState->CamTarget.X = GameState->CamPosition.X;
        GameState->CamTarget.Y = GameState->CamPosition.Y;
        GameState->ConstBufferData.View = ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f});
        MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);

        SetBrushValue(Input, &GameState->Editor);
        PaintTilemap(Input, &GameState->Editor, GameState->CamPosition);

        // TODO: Render Tilemaip...
        tilemap *Tilemap = &GameState->Editor.Tilemap;
        tilesheet *TileSheet = &GameState->Editor.TileSheet;

        for(i32 Y = 0;
            Y < Tilemap->Rows;
            ++Y)
        {
            for(i32 X = 0;
                X < Tilemap->Cols;
                ++X)
            {
                i32 Index = Y * Tilemap->Cols + X;
                u32 TileSheetCols = TileSheet->TexWidth / TileSheet->TileWidth;
                u32 XFrame = Tilemap->Tiles[Index].Base % TileSheetCols;
                u32 YFrame = Tilemap->Tiles[Index].Base / TileSheetCols;  
                
                mat4 Scale = ScaleMat4({(r32)Tilemap->TileWidth, (r32)Tilemap->TileHeight, 0.0f});
                mat4 Trans = TranslationMat4({(r32)Tilemap->TileWidth * X, (r32)Tilemap->TileHeight * Y, 0.0f});
                GameState->ConstBufferData.World = Trans * Scale;
                MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                               &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, TileSheet->TileWidth, TileSheet->TileHeight, XFrame, YFrame);
            }
        } 

        // TODO: Render TileSheet
        mat4 Scale = ScaleMat4({(r32)TileSheet->TexWidth, (r32)TileSheet->TexHeight, 0.0f});
        mat4 Trans = TranslationMat4({TileSheet->Position.X - WND_WIDTH*0.5f,
                                      TileSheet->Position.Y - WND_HEIGHT*0.5f,
                                      0.0f});
        GameState->ConstBufferData.World = Trans * Scale;
        MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                       &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);
        RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIShader, TileSheet->Texture);
    }
}
