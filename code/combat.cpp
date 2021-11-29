void InitCombat(game_state *GameState, combat *Combat, entity *Player)
{
    //GameState->Combat.NumberOfEntities = 0;
    Combat->NumberOfHeros = 0;
    Combat->NumberOfEnemies = 0;
    Combat->NumberOfEnemiesKill = 0;
    Combat->NumberOfHerosKill = 0;
    
    // Init Players entities Array
    i32 Y = 1; 
    for(i32 Index = GameState->HeroPartyCount - 1;
        Index >= 0;
        --Index)
    {
        Combat->Players[Index] = GameState->Entities[Y - 1];
        Combat->Players[Index].Position = {-100.0f, (Y*32.0f) - 64.0f};
        Combat->Players[Index].Facing = BIT(RIGHT);
        Combat->Players[Index].Frame = 4;
        ++Combat->NumberOfHeros;
        ++Y;
    }
    
    // Init Enemies entities Array
    Y = 1; 
    for(i32 Index = Player->NumbOfActions - 1;
        Index >= 0;
        --Index)
    {
        Combat->Enemies[Index] = GameState->Entities[Player->Action[Index]];
        Combat->Enemies[Index].Position = {100.0f, (Y*32.0f) - 64.0f};
        Combat->Enemies[Index].Facing = BIT(LEFT);
        Combat->Enemies[Index].Frame = 12;
        ++Combat->NumberOfEnemies;
        ++Y;
    }

    Combat->SelectedOptions[0] = -1;
    Combat->SelectedOptions[1] = -1;
    Combat->SelectedOptions[2] = -1;

    Combat->NumberOfOptions = 4;
    Combat->OptionSelected = 0;
    Combat->PlayerTurnFinish = false;
    Combat->EnemyTurnFinish = false;
    Combat->NumberOfEnemiesKill = 0;
    Combat->NumberOfHerosKill = 0;
    Combat->HeroEventSet = 0;
}

void EndCombat(game_state *GameState, combat *Combat)
{
    for(i32 Index = 0;
        Index < Combat->NumberOfHeros;
        ++Index)
    {
        entity *Hero = GetEntitiByID(GameState->Entities, Combat->Players[Index].ID);
        Hero->Stats = Combat->Players[Index].Stats;
    }
    for(i32 Index = 0;
        Index < Combat->NumberOfEnemies;
        ++Index)
    {
        entity *Enemy = GetEntitiByID(GameState->Entities, Combat->Enemies[Index].ID);
        Enemy->Stats = Combat->Enemies[Index].Stats;
        Enemy->Alive = Combat->Enemies[Index].Alive;
        if(!Enemy->Alive)
        {
            UnsetCollision(&GameState->Tilemap, Enemy->Position, Enemy->Layer);
        }
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

void MeleAttack(game_state *GameState, combat *Combat, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
{
    if(Entity->Stage == 0)
    {
        Entity->OldPosiotion = Entity->Position;
        v2 Direction = NormalizeV2(Target->Position - Entity->Position);
        Entity->NextPosition = Target->Position;
        Entity->NextPosition.X -= (Direction.X * 16.0f);
        Entity->IsWalking = true;
        Entity->WalkDistance = 0.0f;
        Entity->AnimTimer = 0.0f;
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
        if(Combat->Timer < 6.0f)
        {
            i32 Frame = (i32)floorf(Combat->Timer) % 3;
            mat4 Scale = ScaleMat4({32, 32, 0.0f});
            mat4 Trans = TranslationMat4({Target->Position.X - 8.0f, Target->Position.Y - 8.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                        GameState->FrameConstBuffer, 64, 64, Frame, 0);
            Combat->Timer += 20.0f*DeltaTime; 
        }
        else
        {
            // TODO: Calculate the attack
            weapon_stats EntityWeapon = GameState->Weapons[Entity->Weapon];
            weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
            u32 Damage = Entity->Stats.Strength + EntityWeapon.AttackPower - TargetWeapon.DefensePower;
            Target->Stats.HP_Now -= Damage;
            Combat->Timer = 0.0f;
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
        Combat->ProcessingEvent = false; 
    }
}

void CastRangeSpell(spells_stats Spell, game_state *GameState, combat *Combat, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
{
    if(Entity->Stage == 0)
    { 
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
        v2 FireballPosition = Lerp(Entity->Position, Target->Position, Combat->Timer);


        // TODO: Draw Fireball
        v2 Direction = NormalizeV2(Target->Position - Entity->Position);  
        color_const_buffer ColorBuffer = {};
        mat4 Trans = TranslationMat4({FireballPosition.X, FireballPosition.Y, 0.0f});
        mat4 Scale = ScaleMat4({64.0f, 32.0f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        i32 Frame = (i32)floorf(Combat->AnimTimer) % 4;
        i32 Row = 0;
        if(Direction.X < 0.0f)
        {
            Row = 1;
        }
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                    GameState->FrameConstBuffer, 32, 16, Frame, Row);

        if(Combat->Timer > 1.0f)
        {
            weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
            TargetWeapon.DefensePower += Spell.Defense;
            u32 Damage = Entity->Stats.Intelligence + Spell.Power - TargetWeapon.DefensePower;
            Target->Stats.HP_Now -= Damage;
            if(Target->Stats.HP_Now < 0)
            {
                Target->Stats.HP_Now = 0;
            }

            Combat->ProcessingEvent = false;  
            Combat->Timer = 0.0f;
            Combat->AnimTimer = 0.0f;
            Entity->Stage = 0; 
        }
        Combat->Timer += DeltaTime;
        Combat->AnimTimer += 20.0f*DeltaTime;
         
    }
}

void UseItemOnTarget(item_stats Item, game_state *GameState, combat *Combat, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
{
    if(Entity->Stage == 0)
    {
        if(Combat->AnimTimer < 3.0f)
        {
            i32 Frame = (i32)floorf(Combat->AnimTimer) % 5;
            mat4 Scale = ScaleMat4({32, 32, 0.0f});
            mat4 Trans = TranslationMat4({Entity->Position.X - 8.0f, Entity->Position.Y - 8.0f, 0.0f});  // then we should be able to choos our target
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                        GameState->FrameConstBuffer, 16, 16, Frame, 0);
            Combat->AnimTimer += 8.0f*DeltaTime; 
        }
        else
        {
            ++Entity->Stage;
            Combat->AnimTimer = 0.0f;
        }
    }
    else if(Entity->Stage == 1)
    {
        Entity->Stats.HP_Now += Item.HP_Modifire;
        Entity->Stats.MP_Now += Item.MP_Modifire;
        Entity->Stage = 0;
        Combat->ProcessingEvent = false;
    }
}

void SortEventQueueByEntitySpeed(combat *Combat, u64 *Entities, u64 *Actions)
{   
    for(i32 I = 0; I < 64; I += 8)
    {
        for(i32 J = I; J < 64; J += 8)
        {   
            if(J == 24 && I == 16)
            {
                int StopHere = 0;
            }         
            u64 AIndex = ((*Entities >> I) & 0xFull);
            u64 BIndex = ((*Entities >> J) & 0xFull);
            if((AIndex != 0) && (BIndex != 0) && (AIndex != BIndex))
            { 
                entity *AEntity =  0;
                entity *BEntity =  0;
                if(AIndex < 5)
                {
                    AEntity = &Combat->Players[AIndex - 1];
                }
                else
                { 
                    AEntity = &Combat->Enemies[AIndex - 5];
                }
                if(BIndex < 5)
                {
                    BEntity = &Combat->Players[BIndex - 1];
                }
                else
                { 
                    BEntity = &Combat->Enemies[BIndex - 5];
                }

                if(AEntity->Stats.Speed < BEntity->Stats.Speed)
                {
                    u64 TempEntity = ((*Entities >> J) & 0xFFull);
                    *Entities &= ~(0xFFull << J);
                    *Entities  |= ((*Entities >> I) & 0xFFull) << J;
                    u64 TempAction = ((*Actions >> J) & 0xFFull);
                    *Actions &= ~(0xFFull << J);
                    *Actions |= ((*Actions >> I) & 0xFFull) << J;

                    *Entities &= ~(0xFFull << I);
                    *Entities  |= (TempEntity << I);
                    *Actions &= ~(0xFFull << I);
                    *Actions |= (TempAction << I);
                }
            }
        }
    }
}

void UpdateCombatEventQueue(game_state *GameState, combat *Combat, u64 *Entities, u64 *Actions, r32 DeltaTime)
{
    if(!Combat->ProcessingEvent)
    {
        SortEventQueueByEntitySpeed(Combat, Entities, Actions);
        if((*Entities & 0xFFull) != 0)
        {
            if(Combat->Timer > 0.5f)
            {
                Combat->ProcessingEvent = true;
                Combat->EventData = 0;
                Combat->EventData |= (*Entities & 0xFull);
                Combat->EventData |= ((*Entities & 0xF0ull) >> 4) << 8;
                *Entities = *Entities >> 8;
                Combat->EventData |= (*Actions & 0xFull) << 16;
                Combat->EventData |= ((*Actions & 0xF0ull) >> 4) << 24;
                *Actions = *Actions >> 8;
                Combat->Timer = 0.0f;
            }
            else
            {
                Combat->Timer += DeltaTime;
            }
        }
        else
        {
            Combat->PlayerTurnFinish = false;
            Combat->EnemyTurnFinish = false;
        }
    }
    else
    {
        u8 EntityIndex = (Combat->EventData & 0x000000FF) >> 0;
        u8 TargetIndex = (Combat->EventData & 0x0000FF00) >> 8;
        u8 ActionIndex = (Combat->EventData & 0x00FF0000) >> 16;
        u8 OptionIndex = (Combat->EventData & 0xFF000000) >> 24;
        // Process the Event...
        entity *Entity = 0;
        entity *Target = 0;
        if(EntityIndex < 5)
        {
            Entity = &Combat->Players[EntityIndex - 1];
        }
        else
        { 
            Entity = &Combat->Enemies[EntityIndex - 5];
        }
        if(TargetIndex < 5)
        {
            Target = &Combat->Players[TargetIndex - 1];
        }
        else
        { 
            Target = &Combat->Enemies[TargetIndex - 5];
        }

        if(!Entity->Alive)
        { 
            Combat->ProcessingEvent = false;
            return;
        }
        
        if(ActionIndex == ATTACK)
        {
            MeleAttack(GameState, Combat, Entity, Target, DeltaTime, GameState->CombatSlashTexture);
        }
        if(ActionIndex == SPELLS)
        {
            if(OptionIndex - 1 == 0) // FIREBALL
            {            
                spells_stats Spell = GameState->Spells[OptionIndex - 1];
                CastRangeSpell(Spell, GameState, Combat, Entity, Target, DeltaTime, GameState->FireTexture);
            }
            else if(OptionIndex - 1 == 1) // WATER CANNON
            {
                Combat->ProcessingEvent = false;  
            }
            else if(OptionIndex - 1 == 2) // BARRIER
            {
                Combat->ProcessingEvent = false;   
            }
        }
        if(ActionIndex == ITEMS)
        {
            // TODO: this if is for the texture
            if(OptionIndex - 1 == 0) // Life Potion
            {   
                item_stats Item = GameState->Items[OptionIndex - 1];
                UseItemOnTarget(Item, GameState, Combat, Entity, Target, DeltaTime, GameState->RestHPTexture);
            }
            else if(OptionIndex - 1 == 1) // Ether
            {
                item_stats Item = GameState->Items[OptionIndex - 1];
                UseItemOnTarget(Item, GameState, Combat, Entity, Target, DeltaTime, GameState->RestMPTexture);
            }
        }
    }    
}


void SetEntityRandomAction(game_state *GameState, combat *Combat, i32 EntityIndex)
{
    entity *Enemy = &Combat->Enemies[EntityIndex];
    i32 RandomChoise = rand() % 2;
    i32 Target = (rand() % Combat->NumberOfHeros) + 1;
    if(RandomChoise == 0)
    {
        SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, EntityIndex + 5, Target, ATTACK, 1);
    }
    else if(RandomChoise == 1)
    {
        if(Enemy->Stats.MP_Now - GameState->Spells[0].MP_Cost >= 0)
        {
            u32 SpellIndex = Enemy->Spells[0] + 1;
            SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, EntityIndex + 5, Target, SPELLS, SpellIndex);        
        }
    }
}


void EndCombatIfNecessary(game_state *GameState, combat *Combat)
{
    if((Combat->NumberOfEnemiesKill >= Combat->NumberOfEnemies) ||
       (Combat->NumberOfHerosKill >= Combat->NumberOfHeros))
    {
        if(!Combat->PlayerTurnFinish)
        {
            Combat->EntitiesEventQueue = 0;
            Combat->ActionsEventQueue = 0;
            Combat->ProcessingEvent = false;
            GameState->GamePlayState = WORLD;
            EndCombat(GameState, Combat);
        }
    }
}


void UpdatePlayer(combat *Combat, entity *Players)
{

    for(i32 Index = 0;
        Index < Combat->NumberOfHeros;
        ++Index)
    {
        // check the enemies alive
        if(Players[Index].Stats.HP_Now <= 0 && Players[Index].Alive)
        {
            Players[Index].Alive = false;
            ++Combat->NumberOfHerosKill;
        }
    }

}

void UpdateEnemies(game_state *GameState, combat *Combat, entity *Enemies)
{
    for(i32 Index = 0;
        Index < Combat->NumberOfEnemies;
        ++Index)
    {
        // check the enemies alive
        if(Enemies[Index].Stats.HP_Now <= 0 && Enemies[Index].Alive)
        {
            Enemies[Index].Alive = false;
            ++Combat->NumberOfEnemiesKill;
        }

        if(!Combat->EnemyTurnFinish)
        {
            if(Enemies[Index].Alive)
            {
                SetEntityRandomAction(GameState, Combat, Index);
            }  
            if(Index == (Combat->NumberOfEnemies - 1))
            { 
                Combat->EnemyTurnFinish = true;
            }  
        }
    }
}

void GetCombatImput(combat *Combat, input *Input)
{
    if(Combat->PlayerTurnFinish)
    {
        return;
    }
    if(OnKeyDown(Input->Buttons->Up))
    {
        --Combat->OptionSelected;
    }
    else if(OnKeyDown(Input->Buttons->Down))
    {
        ++Combat->OptionSelected;
    }
    else if(OnKeyDown(Input->Buttons->Start))
    {     
        for(i32 Index = 0;
            Index < 3;
            ++Index)
        {
            if(Combat->SelectedOptions[Index] == -1)
            {
                Combat->SelectedOptions[Index] = Combat->OptionSelected;
                break;
            }
        }
    }
    else if(OnKeyDown(Input->Buttons->Back))
    {
        Combat->NumberOfOptions = 4;
        Combat->OptionSelected = 0;
        Combat->SelectedOptions[0] = -1;
        Combat->SelectedOptions[1] = -1;
        Combat->SelectedOptions[2] = -1;
    }

    if(Combat->OptionSelected < 0)
    {
        Combat->OptionSelected = 0;    
    }
    if(Combat->OptionSelected > Combat->NumberOfOptions - 1)
    {
        Combat->OptionSelected = Combat->NumberOfOptions - 1;
    }
}

void NextHeroTurn(combat *Combat)
{
    Combat->SelectedOptions[0] = -1;
    Combat->SelectedOptions[1] = -1;
    Combat->SelectedOptions[2] = -1;
    Combat->OptionSelected = 0;    
    Combat->NumberOfOptions = 4;
    ++Combat->HeroEventSet;
}

void UpdateAndRenderCombat(game_state *GameState, combat *Combat, input *Input, r32 DeltaTime)
{
    // TODO(manuto): ...
    entity *Players = Combat->Players;
    entity *Enemies = Combat->Enemies;
    inventory *Inventory = &GameState->Inventory;
    
    UpdatePlayer(Combat, Players);
    UpdateEnemies(GameState, Combat, Enemies);
    UpdateItems(Inventory);
    
    EndCombatIfNecessary(GameState, Combat);
    
    if(Combat->HeroEventSet < Combat->NumberOfHeros)
    {
        entity *Player = &Combat->Players[Combat->HeroEventSet];
        
        GetCombatImput(Combat, Input);
        switch(Combat->SelectedOptions[0])
        {
            case 0:
            {
                i32 Target = Combat->SelectedOptions[1];
                if((Target != -1) && Enemies[Target].Alive)
                { 
                    SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->HeroEventSet + 1, Target + 5, ATTACK, 1);
                    NextHeroTurn(Combat);
                }
                else
                {
                    Combat->SelectedOptions[1] = -1;
                    Combat->NumberOfOptions = Combat->NumberOfEnemies;
                }
            }break;
            case 1:
            {
                i32 Option = Combat->SelectedOptions[1];
                i32 Target = Combat->SelectedOptions[2];
                if((Option != -1) && (Target != -1) && Enemies[Target].Alive)
                {
                    u32 SpellIndex = Player->Spells[Option] + 1;
                    SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->HeroEventSet + 1, Target + 5, SPELLS, SpellIndex);  
                    NextHeroTurn(Combat);
                }
                else if(Option == -1) 
                {
                     
                    Combat->NumberOfOptions = Player->SpellsCount;
                }
                else
                { 
                    Combat->SelectedOptions[2] = -1;
                    Combat->NumberOfOptions = Combat->NumberOfEnemies;
                }
            }break;
            case 2:
            {
                i32 Option = Combat->SelectedOptions[1];
                if(Option != -1)
                {
                    inventory_item *Item = &Inventory->Items[Option];
                    if(Item->Count > 0)
                    {
                        --Item->Count;
                        u32 ItemIndex = Item->ID;
                        SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->HeroEventSet + 1, 1, ITEMS, ItemIndex); 
                        NextHeroTurn(Combat);
                    }
                }
                else if(Option == -1) 
                { 
                    Combat->NumberOfOptions = Inventory->ItemsCount;
                }
            }break;
            case 3:
            {
                Combat->NumberOfOptions = 4;
                Combat->EntitiesEventQueue = 0;
                Combat->ActionsEventQueue = 0;
                Combat->ProcessingEvent = false;
                GameState->GamePlayState = WORLD;
            }break; 
        }
    }
    else
    {
        Combat->HeroEventSet = 0;
        Combat->PlayerTurnFinish = true;
    }

    entity *Player = &Combat->Players[Combat->HeroEventSet];
    if(Combat->HeroEventSet >= Combat->NumberOfHeros)
    {
        Player = &Combat->Players[0];
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
    
    if((Combat->SelectedOptions[0] < 0) || (Combat->SelectedOptions[0] > 0) && (Combat->SelectedOptions[1] == -1))
    {
        YPos -= 9.0f*2.0f;
        YPos += 64.0f*2.0f - (Combat->OptionSelected * (9.0f*2.0f));
        ColorBuffer.Color = Color(134.0f, 165.0f, 217.0f);
        MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1);
        Trans = TranslationMat4({XPos, YPos, 0.0f});
        Scale = ScaleMat4({400.0f, 9.0f*2.0f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
    }

    YPos = -WND_HEIGHT*0.5f;
    YPos += (BackPannel.Y*0.5f) - 64.0f;
    YPos += 64.0f*2.0f;
    RenderString(GameState, "Actions:", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
    YPos -= 9.0f*2.0f;
    switch(Combat->SelectedOptions[0])
    {
        case -1:
        {
            RenderString(GameState, "-Atack", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Spells", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Item", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Run", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
        }break;
        case 0:
        {
            RenderString(GameState, "-Atack", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Spells", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Item", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
            YPos -= 9.0f*2.0f;
            RenderString(GameState, "-Run", XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
        }break;
        case 1:
        {
            for(i32 Index = 0;
                Index < 1;
                ++Index)
            {
                RenderString(GameState, GameState->Spells[Player->Spells[Index]].Name, XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
            } 
        }break;
        case 2:
        {
            for(i32 Index = 0;
                Index < Inventory->ItemsCount;
                ++Index)
            {
                inventory_item *Item = &Inventory->Items[Index];
                i32 InnerXPos = RenderString(GameState, GameState->Items[Item->ID - 1].Name, XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                RenderUInt(GameState, Item->Count, InnerXPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
            }
        }break; 
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
    
    // Render Player
    for(i32 Index = 0;
        Index < Combat->NumberOfHeros;
        ++Index)
    {
        Scale = ScaleMat4({16, 24, 0.0f});
        Trans = TranslationMat4({Players[Index].Position.X, Players[Index].Position.Y, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                    GameState->FrameConstBuffer, 16, 24, Players[Index].Frame, Players[Index].Skin);
    }
    
    // Render Enemies
    for(i32 Index = 0;
        Index < Combat->NumberOfEnemies;
        ++Index)
    {
        if(Enemies[Index].Alive)
        {
            Trans = TranslationMat4({Enemies[Index].Position.X, Enemies[Index].Position.Y, 0.0f});
            Scale = ScaleMat4({16, 24, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Enemies[Index].Frame, Enemies[Index].Skin);
        }
        
        i32 Target = -1;
        switch(Combat->SelectedOptions[0])
        {
            case 0:
            {
                Target = Combat->OptionSelected;
            }break;
            default:
            {
                if(Combat->SelectedOptions[1] != -1)
                {
                    Target = Combat->OptionSelected; 
                }
            }break;
        } 
        if(Target == Index)
        {
            ColorBuffer.Color = Color(255.0f, 0.0f, 0.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1); 
            Trans = TranslationMat4({Enemies[Index].Position.X + 4.0f, Enemies[Index].Position.Y + 24.0f, 0.0f});
            Scale = ScaleMat4({8, 8, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
        }

    }
    
    if(Combat->PlayerTurnFinish && Combat->EnemyTurnFinish)
    {
        UpdateCombatEventQueue(GameState, Combat, &Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, DeltaTime);
    }
}
