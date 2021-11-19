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

void InitCombat(game_state *GameState, entity *Player, entity *Enemy)
{
    GameState->CombatOptionSelected = 0;
    GameState->CombatOptionLevel = 0;
    GameState->Player = Player;
    GameState->Enemy = Enemy;
}

#include "tilemap.cpp"
#include "map_editor.cpp"
#include "hero.cpp"

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(10));
    InitArena(Memory, &GameState->BatchArena, (64*64)*(24*6));  // (64*64) * (24*6)
    InitArena(Memory, &GameState->TileMapArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorSaves, Megabytes(2));
    InitArena(Memory, &GameState->IntToCharTempArena, sizeof(u32) * 10);
    InitArena(Memory, &GameState->TextureArena, Kilobytes(1));
    InitArena(Memory, &GameState->TexListArena, Kilobytes(1));

    GameState->Window = PlatformCreateWindow(0, 0, WND_WIDTH, WND_HEIGHT, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);

    input_layout_desc InputLayoutDesc[] = 
    {
        {"POSITION", 0, FORMAT_R32G32B32_FLOAT,
         0, 0, INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, FORMAT_R32G32_FLOAT,
         0, 12, INPUT_PER_VERTEX_DATA, 0}
    };
    u32 InputLayoutElementCount = ArrayCount(InputLayoutDesc);
    input_layout_desc BatchInputLayoutDesc[] = 
    {
        {"POSITION", 0, FORMAT_R32G32B32_FLOAT,
         0, 0, INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, FORMAT_R32G32_FLOAT,
         0, 12, INPUT_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT", 0, FORMAT_R32_FLOAT,
         0, 20, INPUT_PER_VERTEX_DATA, 0}
    };
    u32 BatchElementCount = ArrayCount(BatchInputLayoutDesc);
    GameState->MainShader = CompileShadersFromFile(GameState->Renderer,
                                                   InputLayoutDesc,
                                                   InputLayoutElementCount, 
                                                   "../code/shaders/main_vert.hlsl",  
                                                   "../code/shaders/main_frag.hlsl",
                                                   &GameState->EngineArena);
    GameState->FrameShader = CompileShadersFromFile(GameState->Renderer,
                                                    InputLayoutDesc,
                                                    InputLayoutElementCount,
                                                    "../code/shaders/frame_vert.hlsl",  
                                                    "../code/shaders/frame_frag.hlsl",
                                                    &GameState->EngineArena);
    GameState->UIShader = CompileShadersFromFile(GameState->Renderer,
                                                 InputLayoutDesc,
                                                 InputLayoutElementCount,
                                                 "../code/shaders/ui_vert.hlsl",  
                                                 "../code/shaders/ui_frag.hlsl",
                                                 &GameState->EngineArena);
    GameState->UIFrameShader = CompileShadersFromFile(GameState->Renderer,
                                                      InputLayoutDesc,
                                                      InputLayoutElementCount,
                                                      "../code/shaders/ui_frame_vert.hlsl",  
                                                      "../code/shaders/ui_frame_frag.hlsl",
                                                      &GameState->EngineArena);
    GameState->UISimpleShader = CompileShadersFromFile(GameState->Renderer,
                                                       InputLayoutDesc,
                                                       InputLayoutElementCount,
                                                       "../code/shaders/ui_simple_vert.hlsl",  
                                                       "../code/shaders/ui_simple_frag.hlsl",
                                                       &GameState->EngineArena);
    GameState->UIColorShader = CompileShadersFromFile(GameState->Renderer,
                                                      InputLayoutDesc,
                                                      InputLayoutElementCount,
                                                      "../code/shaders/ui_color_vert.hlsl",  
                                                      "../code/shaders/ui_color_frag.hlsl",
                                                      &GameState->EngineArena);
    GameState->MemBarShader = CompileShadersFromFile(GameState->Renderer,
                                                     InputLayoutDesc,
                                                     InputLayoutElementCount,
                                                     "../code/shaders/memory_vert.hlsl",  
                                                     "../code/shaders/memory_frag.hlsl",
                                                     &GameState->EngineArena);
    GameState->BatchShader = CompileShadersFromFile(GameState->Renderer,
                                                    BatchInputLayoutDesc,
                                                    BatchElementCount,
                                                    "../code/shaders/batch_vert.hlsl",  
                                                    "../code/shaders/batch_frag.hlsl",
                                                    &GameState->EngineArena);

    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->ColorConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(color_const_buffer), &GameState->EngineArena);
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
    GameState->HeroPortraitTexture = CreateTexture(GameState->Renderer, "../data/hero_portrait.bmp", &GameState->EngineArena);
    GameState->CombatBgTexture = CreateTexture(GameState->Renderer, "../data/combat_bg_field.bmp", &GameState->EngineArena);
    
    AddTextureToList(GameState->Renderer, "../data/town_tileset.bmp",
                     &GameState->TexList, &GameState->TexListArena, &GameState->EngineArena);
    AddTextureToList(GameState->Renderer, "../data/tileset_arena.bmp",
                     &GameState->TexList, &GameState->TexListArena, &GameState->EngineArena);
    AddTextureToList(GameState->Renderer, "../data/Map.bmp",
                     &GameState->TexList, &GameState->TexListArena, &GameState->EngineArena);

    GameState->TilemapBatch = CreateBatch(GameState->Renderer, &GameState->BatchArena, 64*64, &GameState->EngineArena);

    GameState->Tilemap = LoadMap(GameState, "../data/map.save");

    // The first element on the array always has to be the hero
    SetEntityPosition(&GameState->Entities[0], &GameState->Tilemap, 8, 11);
    GameState->Entities[0].Name = "Manuto";
    GameState->Entities[0].ID = 1;
    GameState->Entities[0].Facing = BIT(DOWN);
    GameState->Entities[0].Layer = 0;
    GameState->Entities[0].Skin = 1;
    GameState->Entities[0].Stats.HP_Max = 100;
    GameState->Entities[0].Stats.HP_Now = 100;
    GameState->Entities[0].Stats.MP_Max = 25;
    GameState->Entities[0].Stats.MP_Now = 25;
    GameState->Entities[0].Stats.Strength = 12;
    GameState->Entities[0].Stats.Speed = 5;
    GameState->Entities[0].Stats.Intelligence = 5;

    SetEntityPosition(&GameState->Entities[1], &GameState->Tilemap, 15, 11);
    GameState->Entities[1].Name = "Thomex";
    GameState->Entities[1].ID = 2;
    GameState->Entities[1].Facing = BIT(DOWN);
    GameState->Entities[1].Layer = 0;
    GameState->Entities[1].Skin = 2;
    GameState->Entities[1].TimeToWait = 1.0f;
    GameState->Entities[1].Stats.HP_Max = 100;
    GameState->Entities[1].Stats.HP_Now = 100;
    GameState->Entities[1].Stats.MP_Max = 25;
    GameState->Entities[1].Stats.MP_Now = 25;
    GameState->Entities[1].Stats.Strength = 12;
    GameState->Entities[1].Stats.Speed = 5;
    GameState->Entities[1].Stats.Intelligence = 5;

    SetEntityPosition(&GameState->Entities[2], &GameState->Tilemap, 15, 6);
    GameState->Entities[2].Name = "Big Daddy";
    GameState->Entities[2].ID = 3;
    GameState->Entities[2].Facing = BIT(DOWN);
    GameState->Entities[2].Layer = 0;
    GameState->Entities[2].Skin = 3;
    GameState->Entities[2].TimeToWait = 3.0f;
    GameState->Entities[2].Stats.HP_Max = 100;
    GameState->Entities[2].Stats.HP_Now = 100;
    GameState->Entities[2].Stats.MP_Max = 25;
    GameState->Entities[2].Stats.MP_Now = 25;
    GameState->Entities[2].Stats.Strength = 12;
    GameState->Entities[2].Stats.Speed = 5;
    GameState->Entities[2].Stats.Intelligence = 5;

    InitEditor(&GameState->Editor, 16, 16, &GameState->MapEditorArena, GameState);

    GameState->AppState = GAME_STATE;
    GameState->GamePlayState = WORLD;
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
            GameState->CamPosition.X = GameState->Entities[0].Position.X;
            GameState->CamPosition.Y = GameState->Entities[0].Position.Y;
            GameState->CamTarget.X = GameState->CamPosition.X;
            GameState->CamTarget.Y = GameState->CamPosition.Y;
            SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));
            SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));

            i32 EntityID = GameState->Entities[0].Action;
            if(EntityID > 0)
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

                    RenderLayer(GameState, Tilemap, ActualLayer, false);
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
        if(GameState->GamePlayState == COMBAT)
        {
            // TODO(manuto): ...
            entity *Player = GameState->Player;
            entity *Enemy = GameState->Enemy;
            
            if(Input->Buttons->Up.IsDown != Input->Buttons->Up.WasDown)
            {
                if(Input->Buttons->Up.IsDown)
                {
                    --GameState->CombatOptionSelected;
                    if(GameState->CombatOptionSelected < 0)
                    {
                        GameState->CombatOptionSelected = 0;    
                    } 
                }
            }
            if(Input->Buttons->Down.IsDown != Input->Buttons->Down.WasDown)
            {
                if(Input->Buttons->Down.IsDown)
                {
                    ++GameState->CombatOptionSelected;
                    if(GameState->CombatOptionSelected > 3)
                    {
                        GameState->CombatOptionSelected = 3;
                    }
                }
            }
            if(Input->Buttons->Start.IsDown != Input->Buttons->Start.WasDown)
            {
                if(Input->Buttons->Start.IsDown)
                {
                    if(GameState->CombatOptionSelected == 0)
                    {
                        // TODO(manuto): Attack...     
                    }
                    if(GameState->CombatOptionSelected == 1)
                    {
                        ++GameState->CombatOptionLevel;
                        if(GameState->CombatOptionLevel > 1)
                        {
                            GameState->CombatOptionLevel = 1;
                        }
                    }
                    if(GameState->CombatOptionSelected == 2)
                    {
                        ++GameState->CombatOptionLevel;
                        if(GameState->CombatOptionLevel > 1)
                        {
                            GameState->CombatOptionLevel = 1;
                        } 
                    }
                    if(GameState->CombatOptionSelected == 3)
                    {
                        GameState->GamePlayState = WORLD;
                    }
                }
            }
            if(Input->Buttons->Back.IsDown != Input->Buttons->Back.WasDown)
            {
                if(Input->Buttons->Back.IsDown)
                {
                    --GameState->CombatOptionLevel;
                    if(GameState->CombatOptionLevel < 0)
                    {
                        GameState->CombatOptionLevel = 0;
                    }
                }
            }
            
            SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));
            
            r32 XPos = -WND_WIDTH*0.5f;
            r32 YPos = -WND_HEIGHT*0.5f;
            color_const_buffer ColorBuffer = {};
                       
            mat4 Trans = TranslationMat4({XPos, YPos + 192.0f, 0.0f});
            mat4 Scale = ScaleMat4({(r32)WND_WIDTH, WND_HEIGHT- 192.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UISimpleShader, GameState->CombatBgTexture);       
            
            v2 BackPannel = {(r32)WND_WIDTH, 192.0f};
            Trans = TranslationMat4({XPos, YPos, 0.0f});
            Scale = ScaleMat4({BackPannel.X, BackPannel.Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            ColorBuffer.Color = Color(22.0f, 25.0f, 37.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
            
            XPos += 32.0f;
            YPos += (BackPannel.Y*0.5f) - 64.0f;
            ColorBuffer.Color = Color(35.0f, 57.0f, 91.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
            Trans = TranslationMat4({XPos, YPos, 0.0f});
            Scale = ScaleMat4({100.0f, 64*2.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
            RenderString(GameState, Player->Name, XPos, YPos + 64.0f*2.0f, 7.0f*2.0f, 9.0f*2.0f);
            Trans = TranslationMat4({XPos + 50.0f - (81.0f*0.5f), YPos + 64.0f - (76.0f*0.5f), 0.0f});
            Scale = ScaleMat4({81.0f, 76.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UISimpleShader, GameState->HeroPortraitTexture);       

            XPos += 100.0f;
            XPos += 32.0f;
            ColorBuffer.Color = Color(35.0f, 57.0f, 91.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
            Trans = TranslationMat4({XPos, YPos, 0.0f});
            Scale = ScaleMat4({400.0f, 64*2.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);

            YPos -= 9.0f*2.0f;
            YPos += 64.0f*2.0f - (GameState->CombatOptionSelected * (9.0f*2.0f));
            ColorBuffer.Color = Color(134.0f, 165.0f, 217.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
            Trans = TranslationMat4({XPos, YPos, 0.0f});
            Scale = ScaleMat4({400.0f, 9.0f*2.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);

            YPos = -WND_HEIGHT*0.5f;
            YPos += (BackPannel.Y*0.5f) - 64.0f;
            YPos += 64.0f*2.0f;
            RenderString(GameState, "Actions:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Atack", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Spells", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Item", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Run", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
           
            YPos = -WND_HEIGHT*0.5f;
            YPos += (BackPannel.Y*0.5f) - 64.0f;
            XPos += 400.0f;
            XPos += 32.0f;
            ColorBuffer.Color = Color(35.0f, 57.0f, 91.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
            Trans = TranslationMat4({XPos, YPos, 0.0f});
            Scale = ScaleMat4({BackPannel.X - (XPos + WND_WIDTH*0.5f) - 32.0f, 64*2.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
            YPos += 64.0f*2.0f;
            RenderString(GameState, "Stats:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f); 
            YPos -= 9.0f*2.0f;
            i32 XOffset = RenderString(GameState, "-HP:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            i32 InnerXPos = XPos + (XOffset*(7.0f*2.0f));
            XOffset = RenderUInt(GameState, Player->Stats.HP_Now, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos += (XOffset*(7.0f*2.0f));
            XOffset = RenderString(GameState, "/", InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos += (XOffset*(7.0f*2.0f));
            RenderUInt(GameState, Player->Stats.HP_Max, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            XOffset = RenderString(GameState, "-MP:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos = XPos + (XOffset*(7.0f*2.0f));
            XOffset = RenderUInt(GameState, Player->Stats.MP_Now, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos += (XOffset*(7.0f*2.0f));
            XOffset = RenderString(GameState, "/", InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos += (XOffset*(7.0f*2.0f));
            RenderUInt(GameState, Player->Stats.MP_Max, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            XOffset = RenderString(GameState, "-ST:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos = XPos + (XOffset*(7.0f*2.0f));
            RenderUInt(GameState, Player->Stats.Strength, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            XOffset = RenderString(GameState, "-SP:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos = XPos + (XOffset*(7.0f*2.0f));
            RenderUInt(GameState, Player->Stats.Speed, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            XOffset = RenderString(GameState, "-IN:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            InnerXPos = XPos + (XOffset*(7.0f*2.0f));
            RenderUInt(GameState, Player->Stats.Intelligence, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);


            Scale = ScaleMat4({16, 24, 0.0f});
            Trans = TranslationMat4({-100.0f, 0.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Player->Frame, Player->Skin);
            Trans = TranslationMat4({100.0f, 0.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Enemy->Frame, Enemy->Skin);
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
