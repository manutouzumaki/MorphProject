void InitCombat(game_state *GameState, entity *Player)
{
    GameState->Combat.NumberOfEntities = 0;

    GameState->Combat.Entities[0] = *Player;
    GameState->Combat.Entities[0].Position = {-100.0f, 0.0f};
    GameState->Combat.Entities[0].Facing = BIT(RIGHT);
    GameState->Combat.Entities[0].Frame = 4;
    ++GameState->Combat.NumberOfEntities;
    
    for(i32 Index = 1;
        Index < Player->NumbOfActions + 1;
        ++Index)
    {
        GameState->Combat.Entities[Index] = GameState->Entities[Player->Action[Index - 1]];
        GameState->Combat.Entities[Index].Position = {100.0f, (Index*32.0f) - 64.0f};
        GameState->Combat.Entities[Index].Facing = BIT(LEFT);
        GameState->Combat.Entities[Index].Frame = 12;
        ++GameState->Combat.NumberOfEntities;
    }

    GameState->Combat.NumberOfOptions = 4;
    GameState->Combat.OptionSelected = 0;
    GameState->Combat.OptionLevel = 0;
    GameState->Combat.PlayerTurnFinish = false;
    GameState->Combat.EnemyTurnFinish = false;
    GameState->Combat.NumberOfEnemiesKill = 0;
    GameState->Combat.NumberOfHerosKill = 0;
    GameState->Combat.Target = 0;
    GameState->Combat.SelectingTarget = false;
    GameState->Combat.TargetSelected = false;
}

void EndCombat(game_state *GameState)
{
    for(i32 Index = 0;
        Index < GameState->Combat.NumberOfEntities;
        ++Index)
    {
        GameState->Entities[GameState->Combat.Entities[Index].ID - 2].Stats = GameState->Combat.Entities[Index].Stats;
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

void MeleAttack(game_state *GameState, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
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
        if(GameState->Combat.Timer < 6.0f)
        {
            i32 Frame = (i32)floorf(GameState->Combat.Timer) % 3;
            mat4 Scale = ScaleMat4({32, 32, 0.0f});
            mat4 Trans = TranslationMat4({Target->Position.X - 8.0f, Target->Position.Y - 8.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                        GameState->FrameConstBuffer, 64, 64, Frame, 0);
            GameState->Combat.Timer += 20.0f*DeltaTime; 
        }
        else
        {
            // TODO: Calculate the attack
            weapon_stats EntityWeapon = GameState->Weapons[Entity->Weapon];
            weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
            u32 Damage = Entity->Stats.Strength + EntityWeapon.AttackPower - TargetWeapon.DefensePower;
            Target->Stats.HP_Now -= Damage;
            GameState->Combat.Timer = 0.0f;
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
        GameState->Combat.ProcessingEvent = false; 
    }
}

void CastRangeSpell(spells_stats Spell, game_state *GameState, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
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
        v2 FireballPosition = Lerp(Entity->Position, Target->Position, GameState->Combat.Timer);


        // TODO: Draw Fireball
        v2 Direction = NormalizeV2(Target->Position - Entity->Position);  
        color_const_buffer ColorBuffer = {};
        mat4 Trans = TranslationMat4({FireballPosition.X, FireballPosition.Y, 0.0f});
        mat4 Scale = ScaleMat4({64.0f, 32.0f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        i32 Frame = (i32)floorf(GameState->Combat.AnimTimer) % 4;
        i32 Row = 0;
        if(Direction.X < 0.0f)
        {
            Row = 1;
        }
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                    GameState->FrameConstBuffer, 32, 16, Frame, Row);

        if(GameState->Combat.Timer > 1.0f)
        {
            weapon_stats TargetWeapon = GameState->Weapons[Target->Weapon];
            TargetWeapon.DefensePower += Spell.Defense;
            u32 Damage = Entity->Stats.Intelligence + Spell.Power - TargetWeapon.DefensePower;
            Target->Stats.HP_Now -= Damage;
            if(Target->Stats.HP_Now < 0)
            {
                Target->Stats.HP_Now = 0;
            }

            GameState->Combat.ProcessingEvent = false;  
            GameState->Combat.Timer = 0.0f;
            GameState->Combat.AnimTimer = 0.0f;
            Entity->Stage = 0; 
        }
        GameState->Combat.Timer += DeltaTime;
        GameState->Combat.AnimTimer += 20.0f*DeltaTime;
         
    }
}

void UseItemOnTarget(item_stats Item, game_state *GameState, entity *Entity, entity *Target, r32 DeltaTime, texture *Texture)
{
    if(Entity->Stage == 0)
    {
        if(GameState->Combat.AnimTimer < 3.0f)
        {
            i32 Frame = (i32)floorf(GameState->Combat.AnimTimer) % 5;
            mat4 Scale = ScaleMat4({32, 32, 0.0f});
            mat4 Trans = TranslationMat4({Entity->Position.X - 8.0f, Entity->Position.Y - 8.0f, 0.0f});  // then we should be able to choos our target
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                        GameState->FrameConstBuffer, 16, 16, Frame, 0);
            GameState->Combat.AnimTimer += 8.0f*DeltaTime; 
        }
        else
        {
            ++Entity->Stage;
            GameState->Combat.AnimTimer = 0.0f;
        }
    }
    else if(Entity->Stage == 1)
    {
        Entity->Stats.HP_Now += Item.HP_Modifire;
        Entity->Stats.MP_Now += Item.MP_Modifire;
        Entity->Stage = 0;
        GameState->Combat.ProcessingEvent = false;
    }
}

void SortEventQueueByEntitySpeed(game_state *GameState, u64 *Entities, u64 *Actions)
{   
    for(i32 I = 0; I < 64; I += 8)
    {
        for(i32 J = I; J < 64; J += 8)
        {            
            u64 AIndex = ((*Entities >> I) & 0xF);
            u64 BIndex = ((*Entities >> J) & 0xF);
            if(AIndex != 0 && BIndex != 0 && AIndex != BIndex)
            { 
                entity *AEntity = &GameState->Combat.Entities[AIndex - 1];
                entity *BEntity = &GameState->Combat.Entities[BIndex - 1];
                if(AEntity->Stats.Speed < BEntity->Stats.Speed)
                {
                    u64 TempEntity = ((*Entities >> J) & 0xFF);
                    *Entities &= ~(0xFF << J);
                    *Entities  |= ((*Entities >> I) & 0xFF) << J;
                    u64 TempAction = ((*Actions >> J) & 0xFF);
                    *Actions &= ~(0xFF << J);
                    *Actions |= ((*Actions >> I) & 0xFF) << J;

                    *Entities &= ~(0xFF << I);
                    *Entities  |= (TempEntity << I);
                    *Actions &= ~(0xFF << I);
                    *Actions |= (TempAction << I);
                }
            }
        }
    }
}

void UpdateCombatEventQueue(game_state *GameState, u64 *Entities, u64 *Actions, r32 DeltaTime)
{
    if(!GameState->Combat.ProcessingEvent)
    {
        SortEventQueueByEntitySpeed(GameState, Entities, Actions);
        if((*Entities & 0xFF) != 0)
        {
            if(GameState->Combat.Timer > 0.5f)
            {
                GameState->Combat.ProcessingEvent = true;
                GameState->Combat.EventData = 0;
                GameState->Combat.EventData |= (*Entities & 0xF);
                GameState->Combat.EventData |= ((*Entities & 0xF0) >> 4) << 8;
                *Entities = *Entities >> 8;
                GameState->Combat.EventData |= (*Actions & 0xF) << 16;
                GameState->Combat.EventData |= ((*Actions & 0xF0) >> 4) << 24;
                *Actions = *Actions >> 8;
                GameState->Combat.Timer = 0.0f;
            }
            else
            {
                GameState->Combat.Timer += DeltaTime;
            }
        }
        else
        {
            GameState->Combat.PlayerTurnFinish = false;
            GameState->Combat.EnemyTurnFinish = false;
        }
    }
    else
    {
        u8 EntityIndex = (GameState->Combat.EventData & 0x000000FF) >> 0;
        u8 TargetIndex = (GameState->Combat.EventData & 0x0000FF00) >> 8;
        u8 ActionIndex = (GameState->Combat.EventData & 0x00FF0000) >> 16;
        u8 OptionIndex = (GameState->Combat.EventData & 0xFF000000) >> 24;
        // Process the Event...
        entity *Entity = &GameState->Combat.Entities[EntityIndex - 1];
        entity *Target = &GameState->Combat.Entities[TargetIndex - 1];
        
        if(ActionIndex == ATTACK)
        {
            MeleAttack(GameState, Entity, Target, DeltaTime, GameState->CombatSlashTexture);
        }
        if(ActionIndex == SPELLS)
        {
            if(OptionIndex - 1 == 0) // FIREBALL
            {            
                spells_stats Spell = GameState->Spells[OptionIndex - 1];
                CastRangeSpell(Spell, GameState, Entity, Target, DeltaTime, GameState->FireTexture);
            }
            else if(OptionIndex - 1 == 1) // WATER CANNON
            {
                GameState->Combat.ProcessingEvent = false;  
            }
            else if(OptionIndex - 1 == 2) // BARRIER
            {
                GameState->Combat.ProcessingEvent = false;   
            }
        }
        if(ActionIndex == ITEMS)
        {
            if(OptionIndex - 1 == 0) // Life Potion
            {   
                item_stats Item = GameState->Items[OptionIndex - 1];
                UseItemOnTarget(Item, GameState, Entity, Target, DeltaTime, GameState->RestHPTexture);
            }
            else if(OptionIndex - 1 == 1) // Ether
            {
                item_stats Item = GameState->Items[OptionIndex - 1];
                UseItemOnTarget(Item, GameState, Entity, Target, DeltaTime, GameState->RestMPTexture);
            }
        }
    }
}


void SetEntityRandomAction(game_state *GameState, i32 EntityIndex)
{
    entity *Enemy = &GameState->Combat.Entities[EntityIndex];
    i32 RandomChoise = rand() % 3;
    if(RandomChoise == 0)
    {
        SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, EntityIndex + 1, 1, ATTACK, 1);
    }
    else if(RandomChoise == 1)
    {
        if(Enemy->Stats.MP_Now - GameState->Spells[0].MP_Cost >= 0)
        {
            u32 SpellIndex = Enemy->Spells[0] + 1;
            SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, EntityIndex + 1, 1, SPELLS, SpellIndex);        
        }
        else  
        {
            RandomChoise = rand() % 3;
        }
    }
    else if(RandomChoise == 2)
    {
        RandomChoise = rand() % 2;
        u32 ItemIndex = Enemy->Items[RandomChoise] + 1;
        SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, EntityIndex + 1, 1, ITEMS, ItemIndex);         
    }   
}

void UpdateAndRenderCombat(game_state *GameState, input *Input, r32 DeltaTime)
{
    // TODO(manuto): ...
    entity *Player = &GameState->Combat.Entities[0];
    entity *Enemy[4] = {};
    for(i32 Index = 0;
        Index < GameState->Combat.NumberOfEntities - 1;
        ++Index)
    {
        Enemy[Index] = &GameState->Combat.Entities[Index + 1];
    }

    if(GameState->Combat.NumberOfEnemiesKill >= (GameState->Combat.NumberOfEntities - 1) ||
       Player->Stats.HP_Now <= 0)
    {
        GameState->Combat.EntitiesEventQueue = 0;
        GameState->Combat.ActionsEventQueue = 0;
        GameState->Combat.ProcessingEvent = false;
        GameState->GamePlayState = WORLD;
        EndCombat(GameState);
    }

    if(!GameState->Combat.EnemyTurnFinish)
    {
        for(i32 Index = 0;
            Index < GameState->Combat.NumberOfEntities - 1;
            ++Index)
        {
            // check the enemies alive
            if(Enemy[Index]->Stats.HP_Now <= 0 && Enemy[Index]->Alive)
            {
                Enemy[Index]->Alive = false;
                ++GameState->Combat.NumberOfEnemiesKill;
            }
            
            // set enemies actions
            if(Enemy[Index]->Alive)
            {
                SetEntityRandomAction(GameState, Index + 1);
            }
        }
        GameState->Combat.EnemyTurnFinish = true;
    }

    if(GameState->Combat.NumberOfOptions > 0 && !GameState->Combat.PlayerTurnFinish)
    { 
        if(OnKeyDown(Input->Buttons->Up))
        {
            if(GameState->Combat.SelectingTarget)
            {
                ++GameState->Combat.Target;
                if(GameState->Combat.Target > GameState->Combat.NumberOfEntities - 2)
                {
                    GameState->Combat.Target = GameState->Combat.NumberOfEntities - 2;
                }   
            }
            else
            {
                --GameState->Combat.OptionSelected;
                if(GameState->Combat.OptionSelected < 0)
                {
                    GameState->Combat.OptionSelected = 0;    
                }
            } 
        }
        if(OnKeyDown(Input->Buttons->Down))
        {
            if(GameState->Combat.SelectingTarget)
            {
                --GameState->Combat.Target;
                if(GameState->Combat.Target < 0)
                {
                    GameState->Combat.Target = 0;    
                }
            }
            else
            {
                ++GameState->Combat.OptionSelected;
                if(GameState->Combat.OptionSelected > GameState->Combat.NumberOfOptions - 1)
                {
                    GameState->Combat.OptionSelected = GameState->Combat.NumberOfOptions - 1;
                }
            }
        }
        if(OnKeyDown(Input->Buttons->Start))
        {
            if(GameState->Combat.SelectingTarget)
            {
                GameState->Combat.TargetSelected = true;
                GameState->Combat.SelectingTarget = false;
            }

            if(GameState->Combat.OptionLevel == 0)
            {

                GameState->Combat.ActualOption = GameState->Combat.OptionSelected;
            }
            if(GameState->Combat.OptionSelected == 1 && GameState->Combat.OptionLevel == 0)
            {
                GameState->Combat.NumberOfOptions = Player->SpellsCount;
                GameState->Combat.OptionSelected = 0;
                ++GameState->Combat.OptionLevel;
                if(GameState->Combat.OptionLevel > 1)
                {
                    GameState->Combat.OptionLevel = 1;
                }
            }
            else if(GameState->Combat.OptionSelected == 2 && GameState->Combat.OptionLevel == 0)
            {
                GameState->Combat.NumberOfOptions = Player->ItemsCount;
                GameState->Combat.OptionSelected = 0;
                ++GameState->Combat.OptionLevel;
                if(GameState->Combat.OptionLevel > 1)
                {
                    GameState->Combat.OptionLevel = 1;
                } 
            }
            else if(GameState->Combat.OptionSelected == 0 && GameState->Combat.OptionLevel == 0)
            {
                if(GameState->Combat.TargetSelected)
                {
                    SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, 1, GameState->Combat.Target + 2, ATTACK, 1);
                    GameState->Combat.PlayerTurnFinish = true;
                    GameState->Combat.TargetSelected = false;
                }
                else
                {
                    GameState->Combat.SelectingTarget = true;
                }
            }
            else if(GameState->Combat.ActualOption == 1 && GameState->Combat.OptionLevel == 1)
            {
                if(GameState->Combat.TargetSelected)
                {
                    if(Player->Stats.MP_Now - GameState->Spells[Player->Spells[GameState->Combat.OptionSelected]].MP_Cost >= 0)
                    {
                        u32 SpellIndex = Player->Spells[GameState->Combat.OptionSelected] + 1;
                        SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, 1, GameState->Combat.Target + 2, SPELLS, SpellIndex);        
                        GameState->Combat.PlayerTurnFinish = true;
                        GameState->Combat.TargetSelected = false;
                    }
                }
                else
                {
                    GameState->Combat.SelectingTarget = true; 
                }
            }
            else if(GameState->Combat.ActualOption == 2 && GameState->Combat.OptionLevel == 1)
            {
                u32 ItemIndex = Player->Items[GameState->Combat.OptionSelected] + 1;
                SetCombatEventQueue(&GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, 1, GameState->Combat.Target + 2, ITEMS, ItemIndex); 
                GameState->Combat.PlayerTurnFinish = true;
            }
            else if(GameState->Combat.OptionSelected == 3 && GameState->Combat.OptionLevel == 0)
            {
                GameState->Combat.NumberOfOptions = 4;
                GameState->Combat.EntitiesEventQueue = 0;
                GameState->Combat.ActionsEventQueue = 0;
                GameState->Combat.ProcessingEvent = false;
                GameState->GamePlayState = WORLD;
            }
        }
    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        GameState->Combat.OptionLevel = 0;
        GameState->Combat.NumberOfOptions = 4;
        GameState->Combat.OptionSelected = 0;
        --GameState->Combat.OptionLevel;
        if(GameState->Combat.OptionLevel < 0)
        {
            GameState->Combat.OptionLevel = 0;
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
    YPos += 64.0f*2.0f - (GameState->Combat.OptionSelected * (9.0f*2.0f));
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
    if(GameState->Combat.OptionLevel == 0)
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
    if(GameState->Combat.OptionLevel == 1)
    {
        YPos = -WND_HEIGHT*0.5f;
        YPos += (BackPannel.Y*0.5f) - 64.0f;
        YPos += 64.0f*2.0f;
        YPos -= 9.0f*2.0f;
        if(GameState->Combat.ActualOption == 1)
        {
            for(i32 Index = 0;
                Index < GameState->Combat.NumberOfOptions;
                ++Index)
            {
                RenderString(GameState, GameState->Spells[Player->Spells[Index]].Name, XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
                YPos -= 9.0f*2.0f;
            } 
        }
        else if(GameState->Combat.ActualOption == 2)
        {
            for(i32 Index = 0;
                Index < GameState->Combat.NumberOfOptions;
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
    for(i32 Index = 0;
        Index < GameState->Combat.NumberOfEntities - 1;
        ++Index)
    {
        if(Enemy[Index]->Alive)
        {
            Trans = TranslationMat4({Enemy[Index]->Position.X, Enemy[Index]->Position.Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, GameState->HeroTexture,
                        GameState->FrameConstBuffer, 16, 24, Enemy[Index]->Frame, Enemy[Index]->Skin);
        }
        if(GameState->Combat.SelectingTarget && GameState->Combat.Target == Index)
        {
            ColorBuffer.Color = Color(255.0f, 0.0f, 0.0f);
            MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1); 
            Trans = TranslationMat4({Enemy[Index]->Position.X, Enemy[Index]->Position.Y + 8.0f, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
        }
    }
    
    if(GameState->Combat.PlayerTurnFinish && GameState->Combat.EnemyTurnFinish)
    {
        UpdateCombatEventQueue(GameState, &GameState->Combat.EntitiesEventQueue, &GameState->Combat.ActionsEventQueue, DeltaTime);
    }
}
