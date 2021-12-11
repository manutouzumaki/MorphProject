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

void InitEntities00(game_state *GameState)
{
    // NPCS
    GameState->Entities[4] = EntitiesDef[THOMEX];
    GameState->Entities[4].ID = 6;
    SetEntityPosition(&GameState->Entities[4], &GameState->ActualTilemap, 15, 11);
    GameState->Entities[5] = EntitiesDef[BIG_DADDY];
    GameState->Entities[5].ID = 7;
    SetEntityPosition(&GameState->Entities[5], &GameState->ActualTilemap, 3, 3);
    GameState->EntitiesCount = 6;
}

void InitEntities01(game_state *GameState)
{
    // NPCS
    GameState->Entities[4] = EntitiesDef[THOMEX];
    GameState->Entities[4].ID = 6;
    SetEntityPosition(&GameState->Entities[4], &GameState->ActualTilemap, 15, 11);
    GameState->Entities[5] = EntitiesDef[BIG_DADDY];
    GameState->Entities[5].ID = 7;
    SetEntityPosition(&GameState->Entities[5], &GameState->ActualTilemap, 3, 3);
    // ENEMIES
    GameState->Entities[6] = EntitiesDef[GONZITRON];
    GameState->Entities[6].ID = 8;
    SetEntityPosition(&GameState->Entities[6], &GameState->ActualTilemap, 3, 4);
    GameState->Entities[7] = EntitiesDef[JHOSEP];
    GameState->Entities[7].ID = 9;
    SetEntityPosition(&GameState->Entities[7], &GameState->ActualTilemap, 3, 5);
    GameState->EntitiesCount = 8;
}

void InitEntities10(game_state *GameState)
{
    // NPCS
    GameState->Entities[4] = EntitiesDef[THOMEX];
    GameState->Entities[4].ID = 6;
    SetEntityPosition(&GameState->Entities[4], &GameState->ActualTilemap, 3, 2);
    GameState->Entities[5] = EntitiesDef[THOMEX];
    GameState->Entities[5].ID = 7;
    SetEntityPosition(&GameState->Entities[5], &GameState->ActualTilemap, 3, 3);
    GameState->Entities[6] = EntitiesDef[THOMEX];
    GameState->Entities[6].ID = 8;
    SetEntityPosition(&GameState->Entities[6], &GameState->ActualTilemap, 3, 4);
    GameState->Entities[7] = EntitiesDef[THOMEX];
    GameState->Entities[7].ID = 9;
    SetEntityPosition(&GameState->Entities[7], &GameState->ActualTilemap, 3, 5);
    GameState->EntitiesCount = 8;
}

void InitEntities11(game_state *GameState)
{
    // ENEMIES
    GameState->Entities[4] = EntitiesDef[JHOSEP];
    GameState->Entities[4].ID = 6;
    SetEntityPosition(&GameState->Entities[4], &GameState->ActualTilemap, 3, 3);
    GameState->Entities[5] = EntitiesDef[JHOSEP];
    GameState->Entities[5].ID = 7;
    SetEntityPosition(&GameState->Entities[5], &GameState->ActualTilemap, 4, 3);
    GameState->Entities[6] = EntitiesDef[JHOSEP];
    GameState->Entities[6].ID = 8;
    SetEntityPosition(&GameState->Entities[6], &GameState->ActualTilemap, 5, 3);
    GameState->Entities[7] = EntitiesDef[JHOSEP];
    GameState->Entities[7].ID = 9;
    SetEntityPosition(&GameState->Entities[7], &GameState->ActualTilemap, 6, 3);
    GameState->EntitiesCount = 8;
}

typedef void (*init_entites)(game_state *GameState);

init_entites InitFuncs[2][2] = {
    {{InitEntities00}, {InitEntities01}},
    {{InitEntities10}, {InitEntities11}}
};

char *MapToLoad[2][2] = {
    {{"../data/map0x0.save"}, {"../data/map0x1.save"}},
    {{"../data/map1x0.save"}, {"../data/map1x1.save"}}
};

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
