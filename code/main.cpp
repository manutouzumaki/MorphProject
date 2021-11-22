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

void InitCombat(game_state *GameState, entity *Player, entity *Enemy)
{
    GameState->CombatNumberOfOptions = 4;
    GameState->CombatOptionSelected = 0;
    GameState->CombatOptionLevel = 0;
    GameState->CombatEntities[0] = *Player;
    GameState->CombatEntities[1] = *Enemy;
    GameState->CombatEntities[0].Position = {-100.0f, 0.0f};
    GameState->CombatEntities[1].Position = {100.0f, 0.0f};
    GameState->CombatEntities[0].Facing = BIT(RIGHT);
    GameState->CombatEntities[0].Frame = 4;
    GameState->CombatEntities[1].Facing = BIT(LEFT);
    GameState->CombatEntities[1].Frame = 12;
}

void EndCombat(game_state *GameState)
{
    // TODO(manuto): make this function
    for(i32 Index = 0;
        Index < 2;
        ++Index)
    {
        GameState->Entities[GameState->CombatEntities[Index].ID - 2].Stats = GameState->CombatEntities[Index].Stats;
    } 
}


void AddEvent(u64 *EventQueue, u8 Event)
{
    i32 Counter = 0;
    u64 Iterator = *EventQueue;
    u64 Result = (u64)Event;
    while((Iterator & 0xF) != 0 && Counter < 16)
    {
        Iterator = Iterator >> 4;
        Result = Result << 4;
        ++Counter;
    }
    *EventQueue |= Result;
}

void SetCombatEventQueue(u64 *Entities, u64 *Actions,
                         u8 Entity, u8 Target,
                         u8 Action, u8 Option)
{
    AddEvent(Entities, Entity);
    AddEvent(Entities, Target);
    AddEvent(Actions, Action);
    AddEvent(Actions, Option); 
}

void UpdateCombatEventQueue(game_state *GameState, u64 *Entities, u64 *Actions, r32 DeltaTime)
{
    if(!GameState->CombatProcessingEvent)
    {
        if((*Entities & 0xFF) != 0)
        {
            GameState->CombatProcessingEvent = true;
            GameState->CombatEventData = 0;
            GameState->CombatEventData |= (*Entities & 0xF);
            GameState->CombatEventData |= ((*Entities & 0xF0) >> 4) << 8;
            *Entities = *Entities >> 8;
            GameState->CombatEventData |= (*Actions & 0xF) << 16;
            GameState->CombatEventData |= ((*Actions & 0xF0) >> 4) << 24;
            *Actions = *Actions >> 8;
            GameState->CombatTimer = 0.0f;
        }
    }
    else
    {
        u8 EntityIndex = (GameState->CombatEventData & 0x000000FF) >> 0;
        u8 TargetIndex = (GameState->CombatEventData & 0x0000FF00) >> 8;
        u8 ActionIndex = (GameState->CombatEventData & 0x00FF0000) >> 16;
        u8 OptionIndex = (GameState->CombatEventData & 0xFF000000) >> 24;
        // TODO(manuto): Process the Event...
        if(ActionIndex == 1)
        {
            // TODO: Attack
            entity *Entity = &GameState->CombatEntities[EntityIndex - 1];
            entity *Target = &GameState->CombatEntities[TargetIndex - 1];
            
            if(Entity->Stage == 0)
            {
                Entity->OldPosiotion = Entity->Position;
                v2 Direction = NormalizeV2(Target->Position - Entity->Position);
                Entity->NextPosition = Target->Position;
                Entity->NextPosition.X -= (Direction.X * 16.0f);
                Entity->IsWalking = true;
                ++Entity->Stage;
            }
            else if(Entity->Stage == 1)
            {
                if(!MoveEntityInCombat(Entity, DeltaTime))
                {
                    ++Entity->Stage;
                }
            }
            else if(Entity->Stage == 2)
            {
                // TODO(manuto): Attack...
                if(GameState->CombatTimer < 6.0f)
                {
                    i32 Frame = (i32)floorf(GameState->CombatTimer) % 3;
                    mat4 Scale = ScaleMat4({32, 32, 0.0f});
                    mat4 Trans = TranslationMat4({Target->Position.X - 8.0f, Target->Position.Y - 8.0f, 0.0f});
                    SetWorldMat4(GameState, Trans * Scale);
                    RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->CombatSlashTexture,
                                GameState->FrameConstBuffer, 64, 64, Frame, 0);
                    GameState->CombatTimer += 20.0f*DeltaTime; 
                }
                else
                {
                    // TODO: Calculate the attack
                    weapon_stats EntityWeapon = GameState->Weapons[Entity->Weapon];
                    weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
                    u32 Damage = Entity->Stats.Strength + EntityWeapon.AttackPower - TargetWeapon.DefensePower;
                    Target->Stats.HP_Now -= Damage;
                    GameState->CombatTimer = 0.0f;
                    ++Entity->Stage; 
                }

            }
            else if(Entity->Stage == 3)
            { 
                Entity->NextPosition = Entity->OldPosiotion;
                Entity->OldPosiotion = Entity->Position;
                Entity->IsWalking = true;
                if(GET_BIT(Entity->Facing, LEFT)) Entity->Facing = BIT(RIGHT);
                else Entity->Facing = BIT(LEFT);
                ++Entity->Stage;
            }
            else if(Entity->Stage == 4)
            {
                if(!MoveEntityInCombat(Entity, DeltaTime))
                {
                    Entity->Frame = 0;
                    if(GET_BIT(Entity->Facing, LEFT)) Entity->Facing = BIT(RIGHT);
                    else Entity->Facing = BIT(LEFT);
                    if(GET_BIT(Entity->Facing, UP))
                    {
                        Entity->Frame += 0; 
                    }
                    if(GET_BIT(Entity->Facing, DOWN))
                    {
                        Entity->Frame += 8; 
                    }
                    if(GET_BIT(Entity->Facing, LEFT))
                    {
                        Entity->Frame += 12; 
                    }
                    if(GET_BIT(Entity->Facing, RIGHT))
                    {
                        Entity->Frame += 4; 
                    }
                    ++Entity->Stage;
                }
            }
            else if(Entity->Stage == 5)
            {
                Entity->Position = Entity->NextPosition;
                Entity->IsWalking = false;
                Entity->Stage = 0;
                GameState->CombatProcessingEvent = false; 
            } 
        }
        if(ActionIndex == 2)
        {
            // TODO: Spell
            entity *Entity = &GameState->CombatEntities[EntityIndex - 1];
            entity *Target = &GameState->CombatEntities[TargetIndex - 1];
            if(OptionIndex - 1 == 0) // FIREBALL
            {            
                if(Entity->Stage == 0)
                { 
                    spells_stats Spell = GameState->Spells[OptionIndex - 1];
                    Entity->Stats.MP_Now -= Spell.MP_Cost;
                    if(Entity->Stats.MP_Now < 0)
                    {
                        Entity->Stats.MP_Now = 0;
                    }
                    ++Entity->Stage;
                }
                else if(Entity->Stage == 1)
                {
                    Entity->Position;
                    v2 FireballPosition = Lerp(Entity->Position, Target->Position, GameState->CombatTimer);


                    // TODO: Draw Fireball
                    color_const_buffer ColorBuffer = {};
                    mat4 Trans = TranslationMat4({FireballPosition.X, FireballPosition.Y, 0.0f});
                    mat4 Scale = ScaleMat4({64.0f, 32.0f, 0.0f});
                    SetWorldMat4(GameState, Trans * Scale);
                    i32 Frame = (i32)floorf(GameState->CombatAnimTimer) % 4;
                    RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->FireTexture,
                                GameState->FrameConstBuffer, 32, 16, Frame, 0);

                    if(GameState->CombatTimer > 1.0f)
                    {
                        spells_stats Spell = GameState->Spells[OptionIndex - 1];
                        weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
                        TargetWeapon.DefensePower += Spell.Defense;
                        u32 Damage = Entity->Stats.Intelligence + Spell.Power - TargetWeapon.DefensePower;
                        Target->Stats.HP_Now -= Damage;
                        if(Target->Stats.HP_Now < 0)
                        {
                            Target->Stats.HP_Now = 0;
                        }

                        GameState->CombatProcessingEvent = false;  
                        GameState->CombatTimer = 0.0f;
                        GameState->CombatAnimTimer = 0.0f;
                        Entity->Stage = 0; 
                    }
                    GameState->CombatTimer += DeltaTime;
                    GameState->CombatAnimTimer += 20.0f*DeltaTime;
                     
                }

            }
            else if(OptionIndex - 1 == 1) // WATER CANNON
            {
                GameState->CombatProcessingEvent = false;  
            }
            else if(OptionIndex - 1 == 2) // BARRIER
            {
                GameState->CombatProcessingEvent = false;   
            }

        }
        if(ActionIndex == 3)
        {
            // TODO: Item;
            entity *Entity = &GameState->CombatEntities[EntityIndex - 1];
            entity *Target = &GameState->CombatEntities[TargetIndex - 1];
            if(OptionIndex - 1 == 0) // Life Potion
            {           
                if(Entity->Stage == 0)
                {
                    if(GameState->CombatAnimTimer < 3.0f)
                    {
                        i32 Frame = (i32)floorf(GameState->CombatAnimTimer) % 5;
                        mat4 Scale = ScaleMat4({32, 32, 0.0f});
                        mat4 Trans = TranslationMat4({Entity->Position.X - 8.0f, Entity->Position.Y - 8.0f, 0.0f});  // then we should be able to choos our target
                        SetWorldMat4(GameState, Trans * Scale);
                        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->RestHPTexture,
                                    GameState->FrameConstBuffer, 16, 16, Frame, 0);
                        GameState->CombatAnimTimer += 8.0f*DeltaTime; 
                    }
                    else
                    {
                        ++Entity->Stage;
                        GameState->CombatAnimTimer = 0.0f;
                    }
                }
                else if(Entity->Stage == 1)
                {
                    item_stats Item = GameState->Items[OptionIndex - 1];
                    Entity->Stats.HP_Now += Item.HP_Modifire;
                    Entity->Stats.MP_Now += Item.MP_Modifire;
                    Entity->Stage = 0;
                    GameState->CombatProcessingEvent = false;
                }
            }
            else if(OptionIndex - 1 == 1) // Ether
            {
                if(Entity->Stage == 0)
                {
                    if(GameState->CombatAnimTimer < 3.0f)
                    {
                        i32 Frame = (i32)floorf(GameState->CombatAnimTimer) % 5;
                        mat4 Scale = ScaleMat4({32, 32, 0.0f});
                        mat4 Trans = TranslationMat4({Entity->Position.X - 8.0f, Entity->Position.Y - 8.0f, 0.0f});  // then we should be able to choos our target
                        SetWorldMat4(GameState, Trans * Scale);
                        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->RestMPTexture,
                                    GameState->FrameConstBuffer, 16, 16, Frame, 0);
                        GameState->CombatAnimTimer += 8.0f*DeltaTime; 
                    }
                    else
                    {
                        ++Entity->Stage;
                        GameState->CombatAnimTimer = 0.0f;
                    }
                }
                else if(Entity->Stage == 1)
                {
                    item_stats Item = GameState->Items[OptionIndex - 1];
                    Entity->Stats.HP_Now += Item.HP_Modifire;
                    Entity->Stats.MP_Now += Item.MP_Modifire;
                    Entity->Stage = 0;
                    GameState->CombatProcessingEvent = false;
                }
            }
        }
    }
}

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
    GameState->CombatSlashTexture = CreateTexture(GameState->Renderer, "../data/combat_slash.bmp", &GameState->EngineArena);
    GameState->FireTexture = CreateTexture(GameState->Renderer, "../data/fireball.bmp", &GameState->EngineArena);
    GameState->RestHPTexture = CreateTexture(GameState->Renderer, "../data/fx_restore_hp.bmp", &GameState->EngineArena);
    GameState->RestMPTexture = CreateTexture(GameState->Renderer, "../data/fx_restore_mp.bmp", &GameState->EngineArena);
    GameState->ReviveTexture = CreateTexture(GameState->Renderer, "../data/fx_revive.bmp", &GameState->EngineArena);
    GameState->UseItemTexture = CreateTexture(GameState->Renderer, "../data/fx_use_item.bmp", &GameState->EngineArena);
    
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
    GameState->Entities[0].ID = 2;
    GameState->Entities[0].Facing = BIT(DOWN);
    GameState->Entities[0].Layer = 0;
    GameState->Entities[0].Skin = 1;
    GameState->Entities[0].TimeToWait = 2.0f;
    GameState->Entities[0].Stats.HP_Max = 100;
    GameState->Entities[0].Stats.HP_Now = 100;
    GameState->Entities[0].Stats.MP_Max = 25;
    GameState->Entities[0].Stats.MP_Now = 25;
    GameState->Entities[0].Stats.Strength = 12;
    GameState->Entities[0].Stats.Speed = 5;
    GameState->Entities[0].Stats.Intelligence = 5;
    GameState->Entities[0].Weapon = 1;
    GameState->Entities[0].Spells[0] = 0;
    GameState->Entities[0].Spells[1] = 1;
    GameState->Entities[0].Spells[2] = 2;
    GameState->Entities[0].SpellsCount = 3;
    GameState->Entities[0].Items[0] = 0;
    GameState->Entities[0].Items[1] = 1;
    GameState->Entities[0].ItemsCount = 2;

    SetEntityPosition(&GameState->Entities[1], &GameState->Tilemap, 15, 11);
    GameState->Entities[1].Name = "Thomex";
    GameState->Entities[1].ID = 3;
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
    GameState->Entities[1].Weapon = 2;
    GameState->Entities[1].Spells[0] = 2;
    GameState->Entities[1].Spells[1] = 1;
    GameState->Entities[1].Spells[2] = 0;
    GameState->Entities[1].SpellsCount = 3;
    GameState->Entities[1].Items[0] = 0;
    GameState->Entities[1].Items[1] = 1;
    GameState->Entities[1].ItemsCount = 2;

    SetEntityPosition(&GameState->Entities[2], &GameState->Tilemap, 3, 3);
    GameState->Entities[2].Name = "Big Daddy";
    GameState->Entities[2].ID = 4;
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
    GameState->Entities[2].Weapon = 0;
    GameState->Entities[2].Spells[0] = 1;
    GameState->Entities[2].Spells[1] = 0;
    GameState->Entities[2].Spells[2] = 2;
    GameState->Entities[2].SpellsCount = 3;
    GameState->Entities[2].Items[0] = 0;
    GameState->Entities[2].Items[1] = 1;
    GameState->Entities[2].ItemsCount = 2;

    // TODO(manuto): Weapons
    GameState->Weapons[0].Name = "NoWeapon";
    GameState->Weapons[0].AttackType = MELE;
    GameState->Weapons[0].AttackPower = 0;
    GameState->Weapons[0].DefensePower = 0;
    GameState->Weapons[0].Weight = 0;

    GameState->Weapons[1].Name = "Dagger";
    GameState->Weapons[1].AttackType = MELE;
    GameState->Weapons[1].AttackPower = 2;
    GameState->Weapons[1].DefensePower = 0;
    GameState->Weapons[1].Weight = 1;

    GameState->Weapons[2].Name = "Shield";
    GameState->Weapons[2].AttackType = DEFENSIVE;
    GameState->Weapons[2].AttackPower = 0;
    GameState->Weapons[2].DefensePower = 4;
    GameState->Weapons[2].Weight = 3;

    // TODO(manuto): Spells
    GameState->Spells[0].Name = "Fireball";
    GameState->Spells[0].MP_Cost = 5;
    GameState->Spells[0].Power = 10;
    GameState->Spells[0].Defense = 0;
    GameState->Spells[0].TextureIndex = 0; // Not use for now

    GameState->Spells[1].Name = "Water Canon";
    GameState->Spells[1].MP_Cost = 10;
    GameState->Spells[1].Power = 15;
    GameState->Spells[1].Defense = 0;
    GameState->Spells[1].TextureIndex = 1; // Not use for now

    GameState->Spells[2].Name = "Barrier";
    GameState->Spells[2].MP_Cost = 5;
    GameState->Spells[2].Power = 0;
    GameState->Spells[2].Defense = 6;
    GameState->Spells[2].TextureIndex = 2; // Not use for now
    
    // TODO(manuto): Items
    GameState->Items[0].Name = "Life Potion";
    GameState->Items[0].HP_Modifire = 20;
    GameState->Items[0].MP_Modifire = 0;

    GameState->Items[1].Name = "Ether";
    GameState->Items[1].HP_Modifire = 0;
    GameState->Items[1].MP_Modifire = 10;

    // TODO(manuto): Initialize Camera Position
    GameState->CamPosition.X = GameState->Entities[2].Position.X;
    GameState->CamPosition.Y = GameState->Entities[2].Position.Y;
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

    InitEditor(&GameState->Editor, 16, 16, &GameState->MapEditorArena, GameState);

    GameState->AppState = GAME_STATE;
    GameState->GamePlayState = WORLD;

    GameState->CombatNumberOfOptions = 4;
    GameState->CombatEntitiesEventQueue = 0;
    GameState->CombatActionsEventQueue = 0;
    GameState->CombatProcessingEvent = false;
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
        else if(GameState->GamePlayState == COMBAT)
        {
            // TODO(manuto): ...
            entity *Player = &GameState->CombatEntities[0];
            entity *Enemy = &GameState->CombatEntities[1];

            if(Enemy->Stats.HP_Now <= 0)
            {
                Enemy->Stats.HP_Now = 0;
                GameState->CombatEntitiesEventQueue = 0;
                GameState->CombatActionsEventQueue = 0;
                GameState->CombatProcessingEvent = false;
                GameState->GamePlayState = WORLD;
                EndCombat(GameState);
            }
            
            if(GameState->CombatNumberOfOptions > 0)
            { 
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
                        if(GameState->CombatOptionSelected > GameState->CombatNumberOfOptions - 1)
                        {
                            GameState->CombatOptionSelected = GameState->CombatNumberOfOptions - 1;
                        }
                    }
                }
                if(Input->Buttons->Start.IsDown != Input->Buttons->Start.WasDown)
                {
                    if(Input->Buttons->Start.IsDown)
                    {
                        if(GameState->CombatOptionLevel == 0)
                        {

                            GameState->CombatActualOption = GameState->CombatOptionSelected;
                        }
                        if(GameState->CombatOptionSelected == 1 && GameState->CombatOptionLevel == 0)
                        {
                            GameState->CombatNumberOfOptions = Player->SpellsCount;
                            GameState->CombatOptionSelected = 0;
                            ++GameState->CombatOptionLevel;
                            if(GameState->CombatOptionLevel > 1)
                            {
                                GameState->CombatOptionLevel = 1;
                            }
                        }
                        else if(GameState->CombatOptionSelected == 2 && GameState->CombatOptionLevel == 0)
                        {
                            GameState->CombatNumberOfOptions = Player->ItemsCount;
                            GameState->CombatOptionSelected = 0;
                            ++GameState->CombatOptionLevel;
                            if(GameState->CombatOptionLevel > 1)
                            {
                                GameState->CombatOptionLevel = 1;
                            } 
                        }
                        else if(GameState->CombatOptionSelected == 0 && GameState->CombatOptionLevel == 0)
                        {
                            // TODO(manuto): Attack...
                            SetCombatEventQueue(&GameState->CombatEntitiesEventQueue, &GameState->CombatActionsEventQueue, 1, 2, 1, 1);
                        }
                        else if(GameState->CombatActualOption == 1 && GameState->CombatOptionLevel == 1)
                        {
                            u32 SpellIndex = Player->Spells[GameState->CombatOptionSelected] + 1;
                            SetCombatEventQueue(&GameState->CombatEntitiesEventQueue, &GameState->CombatActionsEventQueue, 1, 2, 2, SpellIndex);        
                        }
                        else if(GameState->CombatActualOption == 2 && GameState->CombatOptionLevel == 1)
                        {
                            u32 ItemIndex = Player->Items[GameState->CombatOptionSelected] + 1;
                            SetCombatEventQueue(&GameState->CombatEntitiesEventQueue, &GameState->CombatActionsEventQueue, 1, 2, 3, ItemIndex);         
                        }
                        else if(GameState->CombatOptionSelected == 3 && GameState->CombatOptionLevel == 0)
                        {
                            GameState->CombatNumberOfOptions = 4;
                            GameState->CombatEntitiesEventQueue = 0;
                            GameState->CombatActionsEventQueue = 0;
                            GameState->CombatProcessingEvent = false;
                            GameState->GamePlayState = WORLD;
                        }
                    }
                }
            }
            if(Input->Buttons->Back.IsDown != Input->Buttons->Back.WasDown)
            {
                if(Input->Buttons->Back.IsDown)
                {
                    GameState->CombatOptionLevel = 0;
                    GameState->CombatNumberOfOptions = 4;
                    GameState->CombatOptionSelected = 0;
                    --GameState->CombatOptionLevel;
                    if(GameState->CombatOptionLevel < 0)
                    {
                        GameState->CombatOptionLevel = 0;
                    }
                }
            }
            
            // Render Combat UI 
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
            if(GameState->CombatOptionLevel == 0)
            {
                YPos = -WND_HEIGHT*0.5f;
                YPos += (BackPannel.Y*0.5f) - 64.0f;
                YPos += 64.0f*2.0f;
                YPos -= 9.0f*2.0f;
                RenderString(GameState, "-Atack", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
                RenderString(GameState, "-Spells", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
                RenderString(GameState, "-Item", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
                RenderString(GameState, "-Run", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            }
            if(GameState->CombatOptionLevel == 1)
            {
                YPos = -WND_HEIGHT*0.5f;
                YPos += (BackPannel.Y*0.5f) - 64.0f;
                YPos += 64.0f*2.0f;
                YPos -= 9.0f*2.0f;
                if(GameState->CombatActualOption == 1)
                {
                    for(i32 Index = 0;
                        Index < GameState->CombatNumberOfOptions;
                        ++Index)
                    {
                        RenderString(GameState, GameState->Spells[Player->Spells[Index]].Name, XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                        YPos -= 9.0f*2.0f;
                    } 
                }
                else if(GameState->CombatActualOption == 2)
                {
                    for(i32 Index = 0;
                        Index < GameState->CombatNumberOfOptions;
                        ++Index)
                    {
                        RenderString(GameState, GameState->Items[Player->Items[Index]].Name, XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                        YPos -= 9.0f*2.0f;
                    } 
                }
            }
           
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
            
            // Render Combat Scene
            SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
            
            Scale = ScaleMat4({16, 24, 0.0f});
            Trans = TranslationMat4({Player->Position.X, Player->Position.Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Player->Frame, Player->Skin);
            Trans = TranslationMat4({Enemy->Position.X, Enemy->Position.Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Enemy->Frame, Enemy->Skin);

            UpdateCombatEventQueue(GameState, &GameState->CombatEntitiesEventQueue, &GameState->CombatActionsEventQueue, DeltaTime);

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
