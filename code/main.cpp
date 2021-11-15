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

void RenderString(game_state *GameState, char *String, i32 XPos, i32 YPos)
{
    int Counter = 0;
    while(*String)
    {
        i32 Letter = (i32)*String++;
        if(Letter >= 32 && Letter <= 126)
        {
            Letter -= 32;
            texture_info TextureInfo = GetTextureInfo(GameState->FontTexture);
            i32 TextureNumOfCols = (TextureInfo.Width / 7);
            i32 FrameX = Letter % TextureNumOfCols;
            i32 FrameY = Letter / TextureNumOfCols;
            mat4 Scale = ScaleMat4({7.0f, 9.0f});
            mat4 Trans = TranslationMat4({(r32)XPos + (7*Counter), (r32)YPos, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->FontTexture,
                        GameState->FrameConstBuffer, 7, 9, FrameX, FrameY);
            ++Counter;
        }
    }
}

void RenderUInt(game_state *GameState, u32 Number, i32 XPos, i32 YPos)
{
    u32 *Digits;
    u32 UnitToGet = 10;
    u32 Divisor = 1;
    u32 Counter = 0;
    while((Number / Divisor) >= 10)
    {
        u32 Result = Number % UnitToGet;
        Result /= Divisor;
        UnitToGet *= 10;
        Divisor *= 10;
        Digits = PushStruct(&GameState->IntToCharTempArena, u32);
        *Digits = Result;
        ++Counter;
    }
    Digits = PushStruct(&GameState->IntToCharTempArena, u32);
    *Digits = Number / Divisor;
    ++Counter;
    
    u32 *FirstDigit = Digits;
    FirstDigit -= (Counter - 1);

    for(i32 Index = (Counter - 1);
        Index >= 0;
        --Index)
    {
        u32 *ActualDigit = FirstDigit + Index; 
        char IntToChar = (char)(*ActualDigit + 48);

        RenderString(GameState, &IntToChar, (r32)XPos + (7*((Counter - 1) - Index)), (r32)YPos);
    }
    // Restart the Arena
    GameState->IntToCharTempArena.Used = 0;
}

#include "tilemap.cpp"
#include "map_editor.cpp"
#include "hero.cpp"

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(10));
    InitArena(Memory, &GameState->TileMapArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorSaves, Megabytes(2));
    InitArena(Memory, &GameState->IntToCharTempArena, sizeof(u32) * 10);
    InitArena(Memory, &GameState->TextureArena, Kilobytes(1));

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
    GameState->MemBarShader = CompileShadersFromFile(GameState->Renderer,
                                                     "../code/shaders/memory_vert.hlsl",  
                                                     "../code/shaders/memory_frag.hlsl",
                                                     &GameState->EngineArena);

    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    GameState->MemoryConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(memory_const_buffer), &GameState->EngineArena);
    GameState->UITileSheetConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(tilesheet_ui_const_buffer), &GameState->EngineArena); 

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
    // TODO(manuto): Make a texture array to load the map whit the correct texture
    GameState->TilesheetTextures = CreateTextureOnList(GameState, "../data/town_tileset.bmp", &GameState->TextureArena);
    ++GameState->TilesheetTexturesCount; 
    GameState->TilesheetTextures = CreateTextureOnList(GameState, "../data/tileset_arena.bmp", &GameState->TextureArena);
    ++GameState->TilesheetTexturesCount;
    GameState->TilesheetTextures = CreateTextureOnList(GameState, "../data/Map.bmp", &GameState->TextureArena);
    ++GameState->TilesheetTexturesCount; 
    GameState->HeroTexture = CreateTexture(GameState->Renderer, "../data/walk_cycle.bmp", &GameState->EngineArena);
    GameState->FontTexture = CreateTexture(GameState->Renderer, "../data/font.bmp", &GameState->EngineArena);

    // TODO(manuto): Try to load Maps
    GameState->Tilemap = LoadMap(GameState, "../data/map.save");

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
            }
        }
    }
    
    if(GameState->AppState == GAME_STATE)
    {
        GetHeroInput(Input ,&GameState->HeroEntity, &GameState->Tilemap);
        MoveEntity(&GameState->HeroEntity, DeltaTime);

        GameState->CamPosition.X = GameState->HeroEntity.Position.X;
        GameState->CamPosition.Y = GameState->HeroEntity.Position.Y;
        GameState->CamTarget.X = GameState->CamPosition.X;
        GameState->CamTarget.Y = GameState->CamPosition.Y;
        SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));
        SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));

        // TODO(manuto): Render...
        // TODO: Render Tilemaip...

        // TODO: Get the texture from the file data
        tilemap *Tilemap = &GameState->Tilemap;        
        if(Tilemap->LayersCount > 0)
        {
            for(i32 Index = 0;
                Index < Tilemap->LayersCount;
                ++Index)
            {
                layer *FirstLayer = Tilemap->Layers;
                FirstLayer -= (Tilemap->LayersCount - 1);
                layer *ActualLayer = FirstLayer + Index;
                for(i32 Y = 0;
                    Y < Tilemap->Rows;
                    ++Y)
                {
                    for(i32 X = 0;
                        X < Tilemap->Cols;
                        ++X)
                    {
                        i32 Index = Y * Tilemap->Cols + X; 
                        mat4 Scale = ScaleMat4({(r32)Tilemap->TileWidth, (r32)Tilemap->TileHeight, 0.0f});
                        mat4 Trans = TranslationMat4({(r32)Tilemap->TileWidth * X, (r32)Tilemap->TileHeight * Y, 0.0f});
                        SetWorldMat4(GameState, Trans * Scale);

                        if(ActualLayer->Tiles[Index].Base != 0)
                        {
                            texture *FirstTexture = GameState->TilesheetTextures;
                            FirstTexture -= (GameState->TilesheetTexturesCount - 1);
                            texture *ActualTexture = FirstTexture + ActualLayer->Tiles[Index].BaseTexIndex;
                            u32 TileSheetCols = ActualTexture->Width / Tilemap->TileWidth;

                            u32 XFrame = ActualLayer->Tiles[Index].Base % TileSheetCols;
                            u32 YFrame = ActualLayer->Tiles[Index].Base / TileSheetCols; 
                            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, ActualTexture,
                                        GameState->FrameConstBuffer, 16, 16, XFrame, YFrame);
                        }
                        if(ActualLayer->Tiles[Index].Decoration != 0)
                        {
                            texture *FirstTexture = GameState->TilesheetTextures;
                            FirstTexture -= (GameState->TilesheetTexturesCount - 1);
                            texture *ActualTexture = FirstTexture + ActualLayer->Tiles[Index].DecorationTexIndex;
                            u32 TileSheetCols = ActualTexture->Width / Tilemap->TileWidth;

                            u32 XFrame = ActualLayer->Tiles[Index].Decoration % TileSheetCols;
                            u32 YFrame = ActualLayer->Tiles[Index].Decoration / TileSheetCols; 
                            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, ActualTexture,
                                        GameState->FrameConstBuffer, 16, 16, XFrame, YFrame);
                        }                                    
                    }
                }
                if(Index == 0)
                {
                    mat4 Scale = ScaleMat4({16, 24, 0.0f});
                    mat4 Trans = TranslationMat4({GameState->HeroEntity.Position.X, GameState->HeroEntity.Position.Y, 0.0f});
                    SetWorldMat4(GameState, Trans * Scale);
                    RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->HeroTexture,
                                GameState->FrameConstBuffer, 16, 24, GameState->HeroEntity.Frame, 0);

                }
            }
        }
    }
    
    if(GameState->AppState == EDITOR_STATE)
    {
        UpdateAndRenderEditor(GameState, Input, DeltaTime);
    }

#if 1
    // Show information about the memory of the system and debug info
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));

    RenderString(GameState, "FPS: ", -WND_WIDTH*0.5f, (WND_HEIGHT*0.5f - 9));
    RenderUInt(GameState, (u32)Input->FPS, -WND_WIDTH*0.5f + 5*7, (WND_HEIGHT*0.5f - 9));
    
    i32 XPos = WND_WIDTH*0.5f - 205;
    i32 YPos = WND_HEIGHT*0.5f - (9+5);
    mat4 Trans = {};
    mat4 Scale = ScaleMat4({200.0f, 9.0f, 0.0f});
    memory_const_buffer MemConstBufferData = {};
    // Engine Arena
    RenderString(GameState, "Engine Arena:", XPos, YPos);
    YPos -= 9;
    MemConstBufferData.MemoryData = (r32)((r64)GameState->EngineArena.Used/(r64)GameState->EngineArena.Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    Trans = TranslationMat4({(r32)XPos, (r32)YPos, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);
    YPos -= 9;
    // Texture Arena
    RenderString(GameState, "Texture Arena:", XPos, YPos);
    YPos -= 9;
    MemConstBufferData.MemoryData = (r32)((r64)GameState->TextureArena.Used/(r64)GameState->TextureArena.Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    Trans = TranslationMat4({(r32)XPos, (r32)YPos, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);
    YPos -= 9;
    // TileMap Arena
    RenderString(GameState, "Tilemap Arena:", XPos, YPos);
    YPos -= 9;
    MemConstBufferData.MemoryData = (r32)((r64)GameState->TileMapArena.Used/(r64)GameState->TileMapArena.Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    Trans = TranslationMat4({(r32)XPos, (r32)YPos, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);
    YPos -= 9;
    // MapEditor Arena
    RenderString(GameState, "Map Editor Arena:", XPos, YPos);
    YPos -= 9;
    MemConstBufferData.MemoryData = (r32)((r64)GameState->MapEditorArena.Used/(r64)GameState->MapEditorArena.Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    Trans = TranslationMat4({(r32)XPos, (r32)YPos, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);   
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);
    YPos -= 9;
    // MapEditorSaves Arena
    RenderString(GameState, "Map Editor Saves Arena:", XPos, YPos);
    YPos -= 9;
    MemConstBufferData.MemoryData = (r32)((r64)GameState->MapEditorSaves.Used/(r64)GameState->MapEditorSaves.Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    Trans = TranslationMat4({(r32)XPos, (r32)YPos, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);   
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);  
#endif  
}
