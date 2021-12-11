void CreateCombatUI(game_state *GameState, combat *Combat, tree *Tree, tree *EntityTree)
{
    // Build Menu 
    i32 ActionsID = Tree->AddChildByID(0, "Actions", 0);
    // Actions
    i32 AttacksID = Tree->AddChildByID(0, "Attack", ActionsID);
    i32 SpellsID  = Tree->AddChildByID(1, "Spells", ActionsID);
    i32 ItemsID   = Tree->AddChildByID(2, "Items", ActionsID);
    i32 RunID     = Tree->AddChildByID(3, "Run", ActionsID);
    // Spells
    i32 FireballID = Tree->AddChildByID(0, "Fireball", SpellsID);
    // Items
    //
    for(i32 Index = 0;
        Index < GameState->Inventory.ItemsCount;
        ++Index)
    {
        inventory_item *Item = &GameState->Inventory.Items[Index];
        item_stats ItemStat = GameState->Items[Item->ID - 1];
        Tree->AddChildByID(Index, ItemStat.Name, ItemsID);
    }

    // Build Hero and Target selector
    i32 Root    = EntityTree->AddChildByID(0, "Entities", 0);
    i32 Heroes  = EntityTree->AddChildByID(0, "Heroes", Root);
    i32 Enemies = EntityTree->AddChildByID(1, "Enemies", Root);
    for(i32 Index = 0;
        Index < Combat->NumberOfHeros;
        ++Index)
    {
        if(Index == 0)
        {
            Combat->FirstHeroID = EntityTree->AddChildByID(Index, "Hero", Heroes);
        }
        else
        {
            EntityTree->AddChildByID(Index, "Hero", Heroes);
        }
    }
    for(i32 Index = 0;
        Index < Combat->NumberOfEnemies;
        ++Index)
    {
        if(Index == 0)
        {
           Combat->FirstEnemyID = EntityTree->AddChildByID(Index, "Enemy", Enemies);
        }
        else
        {
            EntityTree->AddChildByID(Index, "Enemy", Enemies);
        }
    } 
}

void RenderCombatHeroUI(game_state *GameState, entity *Hero)
{
    i32 Width = (WND_WIDTH*0.5f);
    i32 Height = (WND_HEIGHT*0.5f);
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    i32 XAbsPos = -Width; 
    i32 YAbsPos = -Height; 
    char *StatsNames[] = {"HP: ", "MP: ", "ST: ", "SP: ", "IN: "};
    RenderUIQuad(GameState, XAbsPos + 10,  YAbsPos + 10, (Width*0.5f) - 20, (Height*0.5f) - 20, 35.0f, 57.0f, 91.0f);
    i32 YPos = Height*0.5f - FontSize.Y  - 10;
    RenderString(GameState, Hero->Name, XAbsPos + 10, YPos - Height, FontSize.X, FontSize.Y);
    RenderUIQuad(GameState, XAbsPos + 40, YAbsPos + 20, (Width*0.5f) - 80, (Height*0.5f) - 30 - FontSize.Y - 10, GameState->HeroPortraitTexture);
    XAbsPos = -Width + (WND_WIDTH*0.75f); 
    RenderUIQuad(GameState, XAbsPos + 10,  YAbsPos + 10, (Width*0.5f) - 20, (Height*0.5f) - 20, 35.0f, 57.0f, 91.0f);
    i32 XPos = XAbsPos + 10;
    YPos -= Height;
    i32 NameIndex = 0;
    for(i32 Index = 0;
        Index < 4;
        Index += 2)
    {
        i32 XOffset = RenderString(GameState, StatsNames[NameIndex++], XPos, YPos, FontSize.X, FontSize.Y);
        i32 InnerXPos = XPos + (XOffset*(FontSize.X));
        XOffset = RenderUInt(GameState, Hero->Stats.Stat[Index], InnerXPos, YPos, FontSize.X, FontSize.Y);
        InnerXPos += (XOffset*(FontSize.X));
        XOffset = RenderString(GameState, "/", InnerXPos, YPos, FontSize.X, FontSize.Y);
        InnerXPos += (XOffset*(FontSize.X));
        RenderUInt(GameState, Hero->Stats.Stat[Index + 1], InnerXPos, YPos, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
    }
    for(i32 Index = 0;
        Index < 3;
        ++Index)
    {
        i32 XOffset = RenderString(GameState, StatsNames[Index + 2], XPos, YPos, FontSize.X, FontSize.Y);
        i32 InnerXPos = XPos + (XOffset*(FontSize.X));
        RenderUInt(GameState, Hero->Stats.Stat[Index + 4], InnerXPos, YPos, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
    }
}

void RenderCombatOptionUI(game_state *GameState, tree::node *Node)
{
    i32 Width = (WND_WIDTH*0.5f);
    i32 Height = (WND_HEIGHT*0.5f);
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    i32 XAbsPos = Width*0.5 - Width; 
    i32 YAbsPos = -Height; 
    RenderUIQuad(GameState, XAbsPos + 10,  YAbsPos + 10, Width - 20, (Height*0.5f) - 20, 35.0f, 57.0f, 91.0f);
    tree::node *FirstSibling = Node;
    while(FirstSibling->BackSibling)
    {
        FirstSibling = FirstSibling->BackSibling;
    }
    i32 YPos = Height*0.5f - FontSize.Y  - 10;
    while(FirstSibling)
    {
        if(FirstSibling->ID == Node->ID && GameState->Combat.SelectingAction)
        { 
            RenderUIQuad(GameState, XAbsPos + 10, YPos -Height, Width - 20, FontSize.Y, 134.0f, 165.0f, 217.0f);
        }
        RenderString(GameState, FirstSibling->Name, XAbsPos + 10, YPos -Height, FontSize.X, FontSize.Y);
        if(Node->Parent && Node->Parent->ID == ITEMS)
        {
            inventory_item *Item = &GameState->Inventory.Items[FirstSibling->Value];
            RenderUInt(GameState, Item->Count, XAbsPos + Width - 10 - FontSize.X, YPos - Height, FontSize.X, FontSize.Y);
        }
        YPos -= FontSize.Y;
        FirstSibling = FirstSibling->NextSibling;
    }
}


void InitCombat(game_state *GameState, combat *Combat, entity *Player)
{
    Combat->NumberOfHeroSet = 0;
    Combat->NumberOfHeros = 0;
    Combat->NumberOfEnemies = 0;
    Combat->NumberOfEnemiesKill = 0;
    Combat->NumberOfHerosKill = 0;
    Combat->EntitiesEventQueue = 0;
    Combat->ActionsEventQueue = 0;
    Combat->EnemiesActionSet = false;
    Combat->ProcessingEvent = false;
    Combat->SelectingAction = false;
    Combat->Action = 0;
    Combat->Hero = 0;
    Combat->Target_ = 0;
    
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
    
    Combat->Tree.Init(&GameState->TreeArena);
    Combat->EntityTree.Init(&GameState->EntityTreeArena);

    CreateCombatUI(GameState, Combat, &Combat->Tree, &Combat->EntityTree);
    
    Combat->Node = Combat->Tree.FindNodeByID(1);
    Combat->EntityNode = Combat->EntityTree.FindNodeByID(Combat->FirstHeroID);
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
            UnsetCollision(&GameState->ActualTilemap, Enemy->Position, Enemy->Layer);
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
            Combat->NumberOfHeroSet = 0;
            Combat->EnemiesActionSet = false;
            for(i32 Index = 0;
                Index < Combat->NumberOfHeros;
                ++Index)
            {
                if(Combat->Players[Index].Alive)
                {
                    i32 HeroID = Combat->FirstHeroID + Index;
                    tree::node *HeroNode = Combat->EntityTree.FindNodeByID(HeroID);
                    HeroNode->Disabled = false;
                }
            }
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
        //if(Enemy->Stats.MP_Now - GameState->Spells[0].MP_Cost >= 0)
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
        if(Combat->NumberOfHeroSet < Combat->NumberOfHeros)
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
            i32 HeroID = Combat->FirstHeroID + Index;
            tree::node *HeroNode = Combat->EntityTree.FindNodeByID(HeroID);
            HeroNode->Disabled = true;
            HeroNode->Dead = true;
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
            i32 EnemyID = Combat->FirstEnemyID + Index;
            tree::node *EnemyNode = Combat->EntityTree.FindNodeByID(EnemyID);
            EnemyNode->Dead = true;
            ++Combat->NumberOfEnemiesKill;
        }

        if(!Combat->EnemiesActionSet)
        {
            if(Enemies[Index].Alive)
            {
                SetEntityRandomAction(GameState, Combat, Index);
            }  
            if(Index == (Combat->NumberOfEnemies - 1))
            { 
                Combat->EnemiesActionSet = true;
            }  
        }
    }
}

void SelectHero(combat *Combat, input *Input)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        if(Combat->EntityNode->BackSibling && !Combat->EntityNode->BackSibling->Disabled)
        {
            Combat->EntityNode = Combat->EntityNode->BackSibling;
        }
        else if(Combat->EntityNode->BackSibling && Combat->EntityNode->BackSibling->Disabled)
        {
            tree::node *TestNode = Combat->EntityNode->BackSibling;
            while(TestNode)
            {
                if(!TestNode->Disabled)
                {
                    Combat->EntityNode = TestNode;
                    break; 
                }
                TestNode = TestNode->BackSibling;
            }
        }
            
    }
    if(OnKeyDown(Input->Buttons->Down))
    {
        if(Combat->EntityNode->NextSibling && !Combat->EntityNode->NextSibling->Disabled)
        {
            Combat->EntityNode = Combat->EntityNode->NextSibling;
        }
        else if(Combat->EntityNode->NextSibling && Combat->EntityNode->NextSibling->Disabled)
        {
            tree::node *TestNode = Combat->EntityNode->NextSibling;
            while(TestNode)
            {
                if(!TestNode->Disabled)
                {
                    Combat->EntityNode = TestNode;
                    break; 
                }
                TestNode = TestNode->NextSibling;
            }
        }
    }
    
    while(Combat->EntityNode->NextSibling && Combat->EntityNode->Disabled)
    {
        Combat->EntityNode = Combat->EntityNode->NextSibling;
    }

    if(OnKeyDown(Input->Buttons->Start))
    {
        Combat->Hero = Combat->EntityNode;
        Combat->EntityNode = Combat->EntityTree.FindNodeByID(Combat->FirstEnemyID);
        Combat->SelectingAction = true;
    }
}

void SelectTarget(combat *Combat, input *Input)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        if(Combat->EntityNode->BackSibling && !Combat->EntityNode->BackSibling->Dead)
        {
            Combat->EntityNode = Combat->EntityNode->BackSibling;
        }
        else if(Combat->EntityNode->BackSibling && Combat->EntityNode->BackSibling->Dead)
        {
            tree::node *TestNode = Combat->EntityNode->BackSibling;
            while(TestNode)
            {
                if(!TestNode->Dead)
                {
                    Combat->EntityNode = TestNode;
                    break; 
                }
                TestNode = TestNode->BackSibling;
            }
        }
            
    }
    if(OnKeyDown(Input->Buttons->Down))
    {
        if(Combat->EntityNode->NextSibling && !Combat->EntityNode->NextSibling->Dead)
        {
            Combat->EntityNode = Combat->EntityNode->NextSibling;
        }
        else if(Combat->EntityNode->NextSibling && Combat->EntityNode->NextSibling->Dead)
        {
            tree::node *TestNode = Combat->EntityNode->NextSibling;
            while(TestNode)
            {
                if(!TestNode->Dead)
                {
                    Combat->EntityNode = TestNode;
                    break; 
                }
                TestNode = TestNode->NextSibling;
            }
        }

    }
    if(OnKeyDown(Input->Buttons->Left))
    {
        if(Combat->EntityNode->Parent->BackSibling)
        {
            Combat->EntityNode = Combat->EntityNode->Parent->BackSibling->Child;
            while(Combat->EntityNode->NextSibling && Combat->EntityNode->Disabled)
            {
                Combat->EntityNode = Combat->EntityNode->NextSibling;
            }
        }
    }
    if(OnKeyDown(Input->Buttons->Right))
    {
        if(Combat->EntityNode->Parent->NextSibling)
        {
            Combat->EntityNode = Combat->EntityNode->Parent->NextSibling->Child;
            while(Combat->EntityNode->NextSibling && Combat->EntityNode->Disabled)
            {
                Combat->EntityNode = Combat->EntityNode->NextSibling;
            }

        }
    }

    while(Combat->EntityNode->NextSibling && Combat->EntityNode->Dead)
    {
        Combat->EntityNode = Combat->EntityNode->NextSibling;
    }
    
    if(OnKeyDown(Input->Buttons->Start))
    {
        Combat->Target_ = Combat->EntityNode;
        Combat->EntityNode = Combat->EntityTree.FindNodeByID(Combat->FirstHeroID);
    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        Combat->SelectingAction = true;
        Combat->Action = 0;
    }
}

void SelectAction(combat *Combat, input *Input)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        if(Combat->Node->BackSibling)
        {
            Combat->Node = Combat->Node->BackSibling;
        }
    } 
    if(OnKeyDown(Input->Buttons->Down))
    {
        if(Combat->Node->NextSibling)
        {
            Combat->Node = Combat->Node->NextSibling;
        }
    }
    if(OnKeyDown(Input->Buttons->Start))
    {
        if(Combat->Node->Child)
        {
            Combat->Node = Combat->Node->Child;
        }
        // TODO: NOT PERMANENT FIX THISS !!!
        else if(Combat->Node->ID != ITEMS) 
        {
            Combat->SelectingAction = false;
            Combat->Action = Combat->Node;
        }
    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        if(Combat->Node->Parent)
        {
            Combat->Action = 0;
            Combat->Node = Combat->Node->Parent;
        }
        else
        {
            Combat->SelectingAction = false;
            Combat->Hero = 0;
            Combat->EntityNode = Combat->EntityTree.FindNodeByID(Combat->FirstHeroID);
        }
    }
}

void GetCombatImput(combat *Combat, input *Input)
{
    if(Combat->NumberOfHeroSet >= (Combat->NumberOfHeros - Combat->NumberOfHerosKill))
    {
        return;
    }
    if(Combat->Hero == 0)
    {
        SelectHero(Combat, Input);
    }
    else if(Combat->Action == 0) 
    { 
        SelectAction(Combat, Input);
    }
    else
    {
        SelectTarget(Combat, Input);
    }
}

void NextHeroTurn(combat *Combat)
{
    Combat->Hero = 0;
    Combat->Action = 0;
    Combat->Target_ = 0;
}

void SetHeroEvents(game_state *GameState, combat *Combat, entity *Player, inventory *Inventory, i32 *NumberOfHeroSet)
{
    if(Combat->Hero && Combat->Action && Combat->Target_)
    {
        i32 Offset = 1;
        if(Combat->Target_->Parent->ID == ENEMIES) { Offset = 5;}
        Combat->Hero->Disabled = true; 
        if(Combat->Action->ID == ATTACK)
        {
            SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->Hero->Value + 1, Combat->Target_->Value + Offset, ATTACK, 1);
            NextHeroTurn(Combat); 
        }
        else if(Combat->Action->Parent->ID == SPELLS)
        {
            u32 SpellIndex = Player->Spells[Combat->Action->Value] + 1;
            SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->Hero->Value + 1, Combat->Target_->Value + Offset, SPELLS, SpellIndex);  
            NextHeroTurn(Combat);  
        }
        else if(Combat->Action->Parent->ID == ITEMS)
        {
            inventory_item *Item = &Inventory->Items[Combat->Action->Value];
            SetCombatEventQueue(&Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, Combat->Hero->Value + 1, Combat->Target_->Value + Offset, ITEMS, Item->ID); 
            NextHeroTurn(Combat);
        }
        ++*NumberOfHeroSet;
    }
}

void UpdateAndRenderCombat(game_state *GameState, combat *Combat, input *Input, r32 DeltaTime)
{
    // TODO(manuto): ...
    entity *Players = Combat->Players;
    entity *Enemies = Combat->Enemies;
    inventory *Inventory = &GameState->Inventory;
    
    UpdatePlayer(Combat, Players);
    UpdateEnemies(GameState, Combat, Enemies);
    EndCombatIfNecessary(GameState, Combat);
   
    GetCombatImput(Combat, Input);
    
    entity *Player = 0;
    if(Combat->Hero)
    {
        Player = &Combat->Players[Combat->Hero->Value]; 
    }
    else
    {
        Player = &Combat->Players[Combat->EntityNode->Value]; 
    } 
    SetHeroEvents(GameState, Combat, Player, Inventory, &Combat->NumberOfHeroSet); 
    if(Combat->Action && Combat->Action->ID == RUN)
    {
        GameState->GamePlayState = WORLD;        
    }
 
    // Render Combat UI 
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f)); 
    // Render background image
    i32 Height = WND_HEIGHT * 0.75f;
    i32 Offset = WND_HEIGHT * 0.25f;
    RenderUIQuad(GameState, -(WND_WIDTH*0.5f), -(WND_HEIGHT*0.5f) + Offset, WND_WIDTH, Height, GameState->CombatBgTexture);
    RenderUIQuad(GameState, -(WND_WIDTH*0.5f), -(WND_HEIGHT*0.5f), WND_WIDTH, Offset, 22.0f, 25.0f, 37.0f);
    // Render Actions and Options
    RenderCombatHeroUI(GameState, Player);
    RenderCombatOptionUI(GameState, Combat->Node);
    // Render Combat Scene
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
    
    // Render Player
    mat4 Trans = IdentityMat4();
    mat4 Scale = IdentityMat4();
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
    } 
    // Render Hero and Target selector
    if((Combat->NumberOfHeroSet < (Combat->NumberOfHeros - Combat->NumberOfHerosKill)) && !Combat->SelectingAction)
    {
        if(!Combat->EntityNode->Dead)
        { 
            color_const_buffer ColorBuffer = {};
            if(Combat->EntityNode->Parent->ID == HEROES)
            {
                ColorBuffer.Color = Color(0.0f, 255.0f, 0.0f);
                MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1); 
                Trans = TranslationMat4({Players[Combat->EntityNode->Value].Position.X + 4.0f, Players[Combat->EntityNode->Value].Position.Y + 24.0f, 0.0f});
            }
            else if(Combat->EntityNode->Parent->ID == ENEMIES)
            {
                ColorBuffer.Color = Color(255.0f, 0.0f, 0.0f);
                MapConstBuffer(GameState->Renderer, GameState->ColorConstBuffer, (void *)&ColorBuffer, sizeof(color_const_buffer), 1); 
                Trans = TranslationMat4({Enemies[Combat->EntityNode->Value].Position.X + 4.0f, Enemies[Combat->EntityNode->Value].Position.Y + 24.0f, 0.0f});
            }
            Scale = ScaleMat4({8, 8, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIColorShader);
        }
    }

    if(Combat->NumberOfHeroSet >= (Combat->NumberOfHeros - Combat->NumberOfHerosKill))
    {
        UpdateCombatEventQueue(GameState, Combat, &Combat->EntitiesEventQueue, &Combat->ActionsEventQueue, DeltaTime); 
    }
}
