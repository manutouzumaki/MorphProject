void InitShaders(game_state *GameState)
{
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

}

void InitConstBuffers(game_state *GameState)
{
    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->ColorConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(color_const_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    GameState->MemoryConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(memory_const_buffer), &GameState->EngineArena);
    GameState->UITileSheetConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(tilesheet_ui_const_buffer), &GameState->EngineArena); 
}

void InitTextures(game_state *GameState)
{
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

}

void InitEntities(game_state *GameState)
{
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
    GameState->Entities[0].Stats.Speed = 7;
    GameState->Entities[0].Stats.Intelligence = 5;
    GameState->Entities[0].Weapon = 1;
    GameState->Entities[0].Spells[0] = 0;
    GameState->Entities[0].Spells[1] = -1;
    GameState->Entities[0].Spells[2] = -1;
    GameState->Entities[0].SpellsCount = 1;
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
    GameState->Entities[1].Spells[0] = 0;
    GameState->Entities[1].Spells[1] = -1;
    GameState->Entities[1].Spells[2] = -1;
    GameState->Entities[1].SpellsCount = 0;
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
    GameState->Entities[2].Stats.Speed = 10;
    GameState->Entities[2].Stats.Intelligence = 5;
    GameState->Entities[2].Weapon = 0;
    GameState->Entities[2].Spells[0] = 0;
    GameState->Entities[2].Spells[1] = -1;
    GameState->Entities[2].Spells[2] = -1;
    GameState->Entities[2].SpellsCount = 0;
    GameState->Entities[2].Items[0] = 0;
    GameState->Entities[2].Items[1] = 1;
    GameState->Entities[2].ItemsCount = 2;
}

void InitWeapons(game_state *GameState)
{
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

}

void InitSpells(game_state *GameState)
{
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
}

void InitItems(game_state *GameState)
{
    // TODO(manuto): Items
    GameState->Items[0].Name = "Life Potion";
    GameState->Items[0].HP_Modifire = 20;
    GameState->Items[0].MP_Modifire = 0;

    GameState->Items[1].Name = "Ether";
    GameState->Items[1].HP_Modifire = 0;
    GameState->Items[1].MP_Modifire = 10;
}
