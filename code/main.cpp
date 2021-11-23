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

v3 Color(r32 R, r32 G, r32 B)
{
    v3 Color = {};
    Color.X = InvLerp(0.0f, 255.0f, R);
    Color.Y = InvLerp(0.0f, 255.0f, G);
    Color.Z = InvLerp(0.0f, 255.0f, B);
    return Color;
}

void InsertionSort(entity *Entities, i32 Length)
{
    for(i32 J = 1;
        J < Length;
        ++J)
    {
        entity Key = Entities[J];
        i32 I = J - 1;
        while(I > -1 && Entities[I].Position.Y < Key.Position.Y)
        {
            Entities[I + 1] = Entities[I];
            --I;
        }
        Entities[I + 1] = Key;
    }
}

i32 RenderString(game_state *GameState, char *String, i32 XPos, i32 YPos)
{
    i32 Counter = 0;
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
    return Counter;
}

i32 RenderString(game_state *GameState, char *String, i32 XPos, i32 YPos, r32 Width, r32 Height)
{
    i32 Counter = 0;
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
            mat4 Scale = ScaleMat4({Width, Height});
            mat4 Trans = TranslationMat4({(r32)XPos + (Width*Counter), (r32)YPos, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->FontTexture,
                        GameState->FrameConstBuffer, 7, 9, FrameX, FrameY);
            ++Counter;
        }
    }
    return Counter;
}



u32 RenderUInt(game_state *GameState, u32 Number, i32 XPos, i32 YPos)
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
    return Counter;
}

u32 RenderUInt(game_state *GameState, u32 Number, i32 XPos, i32 YPos, r32 Width, r32 Height)
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

        RenderString(GameState, &IntToChar, (r32)XPos + (Width*((Counter - 1) - Index)), (r32)YPos, Width, Height);
    }
    // Restart the Arena
    GameState->IntToCharTempArena.Used = 0;
    return Counter;
}

void RenderMemoryData(game_state *GameState, arena *Arena, char *Text, int *XPos, int *YPos)
{
    memory_const_buffer MemConstBufferData = {};
    RenderString(GameState, Text, *XPos, *YPos);
    *YPos -= 9;
    mat4 Trans = TranslationMat4({(r32)*XPos, (r32)*YPos, 0.0f});;
    mat4 Scale = ScaleMat4({200.0f, 9.0f, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);   
    MemConstBufferData.MemoryData = (r32)((r64)Arena->Used/(r64)Arena->Size);
    MapConstBuffer(GameState->Renderer, GameState->MemoryConstBuffer, &MemConstBufferData, sizeof(memory_const_buffer), 1); 
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->MemBarShader);
    *YPos -= 9;
}

#include "tilemap.cpp"
#include "map_editor.cpp"
#include "hero.cpp"
#include "combat.cpp"
#include "init.cpp"

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);
    
    // Get memory for the game
    InitArena(Memory, &GameState->EngineArena, Megabytes(10));
    InitArena(Memory, &GameState->BatchArena, (64*64)*(24*6));  // (64*64) * (24*6)
    InitArena(Memory, &GameState->TileMapArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorSaves, Megabytes(2));
    InitArena(Memory, &GameState->IntToCharTempArena, sizeof(u32) * 10);
    InitArena(Memory, &GameState->TextureArena, Kilobytes(1));
    InitArena(Memory, &GameState->TexListArena, Kilobytes(1));
    
    // Init window and renderer stuff
    GameState->Window = PlatformCreateWindow(0, 0, WND_WIDTH, WND_HEIGHT, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);
    InitShaders(GameState);
    InitConstBuffers(GameState);
    InitTextures(GameState);

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
    GameState->TilemapBatch = CreateBatch(GameState->Renderer, &GameState->BatchArena, 64*64, &GameState->EngineArena);
    GameState->Tilemap = LoadMap(GameState, "../data/map.save");
    
    // Init gameplay
    InitEntities(GameState);
    InitWeapons(GameState);
    InitSpells(GameState);
    InitItems(GameState);
    InitEditor(&GameState->Editor, 16, 16, &GameState->MapEditorArena, GameState);

    // Initialize Camera Position and Target
    GameState->CamPosition.X = GameState->Entities[2].Position.X;
    GameState->CamPosition.Y = GameState->Entities[2].Position.Y;
    GameState->CamPosition.Z = -0.1f;
    if(GameState->CamPosition.X - (WND_WIDTH*0.5f*0.5f) < 0)
    {
        GameState->CamPosition.X -= GameState->CamPosition.X - (WND_WIDTH*0.5f*0.5f);
    } 
    if(GameState->CamPosition.X + (WND_WIDTH*0.5f*0.5f) > 64*16)
    {
        GameState->CamPosition.X -= (GameState->CamPosition.X + (WND_WIDTH*0.5f*0.5f) - (64*16));
    }
    if(GameState->CamPosition.Y - (WND_HEIGHT*0.5f*0.5f) < 0)
    {
        GameState->CamPosition.Y -= GameState->CamPosition.Y - (WND_HEIGHT*0.5f*0.5f); 
    }
    if(GameState->CamPosition.Y + (WND_HEIGHT*0.5f*0.5f) > 64*16)
    { 
        GameState->CamPosition.Y -= (GameState->CamPosition.Y + (WND_HEIGHT*0.5f*0.5f) - (64*16));
    }
    GameState->CamTarget.X = GameState->CamPosition.X;
    GameState->CamTarget.Y = GameState->CamPosition.Y;
    GameState->CamTarget.Z = 0.0f;
    SetWorldMat4(GameState, IdentityMat4());
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

    // Set up the game state
    GameState->AppState = GAME_STATE;
    GameState->GamePlayState = WORLD;

    GameState->Combat.NumberOfOptions = 4;
    GameState->Combat.EntitiesEventQueue = 0;
    GameState->Combat.ActionsEventQueue = 0;
    GameState->Combat.ProcessingEvent = false;
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
        // Update...
        if(GameState->GamePlayState == WORLD)
        {
            GetHeroInput(Input ,&GameState->Entities[0], &GameState->Tilemap);
            for(i32 Index = 0;
                Index < ArrayCount(GameState->Entities);
                ++Index)
            {
                if(Index != 0)
                {
                    SetEntityInRandomDirection(&GameState->Entities[Index], &GameState->Tilemap);
                }
                MoveEntity(&GameState->Entities[Index], &GameState->Tilemap, DeltaTime);
            }
            
            v2 NewCamPos = GameState->Entities[0].Position;

            if(NewCamPos.X - (WND_WIDTH*0.5f*0.5f) < 0 ||
               NewCamPos.X + (WND_WIDTH*0.5f*0.5f) > 64*16)
            {
                NewCamPos.X = GameState->CamPosition.X;
            }
            if(NewCamPos.Y - (WND_HEIGHT*0.5f*0.5f) < 0 ||
               NewCamPos.Y + (WND_HEIGHT*0.5f*0.5f) > 64*16)
            { 
                NewCamPos.Y = GameState->CamPosition.Y;
            }
            GameState->CamPosition.X = NewCamPos.X;
            GameState->CamPosition.Y = NewCamPos.Y;
            GameState->CamTarget.X = GameState->CamPosition.X;
            GameState->CamTarget.Y = GameState->CamPosition.Y;
            SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));
            
            SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));

            i32 EntityID = GameState->Entities[0].Action;
            if(EntityID >= 0 && EntityID != (GameState->Entities[0].ID - 2))
            {
                // TODO(manuto): make some COMBAT
                GameState->GamePlayState = COMBAT;
                InitCombat(GameState, &GameState->Entities[0], &GameState->Entities[EntityID]);
            }

            // Render...
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

                    RenderLayer(GameState, Tilemap, ActualLayer, false, 0.5f);
                    entity Entities[3];
                    memcpy(Entities, GameState->Entities, 3*sizeof(entity));
                    InsertionSort(Entities, 3); 
                    for(i32 I = 0;
                        I < ArrayCount(Entities);
                        ++I)
                    {
                        if(Index == Entities[I].Layer)
                        {
                            mat4 Scale = ScaleMat4({16, 24, 0.0f});
                            mat4 Trans = TranslationMat4({Entities[I].Position.X, Entities[I].Position.Y, 0.0f});
                            SetWorldMat4(GameState, Trans * Scale);
                            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->HeroTexture,
                                        GameState->FrameConstBuffer, 16, 24, Entities[I].Frame, Entities[I].Skin); 
                        }
                    }
                }
            }
        }
        else if(GameState->GamePlayState == COMBAT)
        {
            UpdateAndRenderCombat(GameState, Input, DeltaTime);
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
    i32 YPos = WND_HEIGHT*0.5f - (9);
    RenderMemoryData(GameState, &GameState->EngineArena, "Engine Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TextureArena, "Texture Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TileMapArena, "Tilemap Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->MapEditorArena, "Map Editor Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->MapEditorSaves, "Map Editor Saves Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TexListArena, "TexList Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->BatchArena, "Batch Renderer Arena", &XPos, &YPos);
#endif 

}
