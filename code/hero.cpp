void SetEntityPosition(entity *Entity, tilemap *Tilemap, i32 TileX, i32 TileY)
{
    Entity->Position.X = TileX * 16.0f;
    Entity->Position.Y = TileY * 16.0f;
    SetCollition(Tilemap, Entity->Position, Entity->Layer, Entity->ID);
}

u32 GetFacing(v2 NextPosition, v2 OldPosition, u32 Facing)
{
    u32 Result = Facing;
    v2 Direction = NormalizeV2(NextPosition - OldPosition);
    if(Direction.X > 0.0f)
    {
        Result = BIT(RIGHT);
    }
    if(Direction.X < 0.0f)
    {
        Result = BIT(LEFT); 
    }
    if(Direction.Y > 0.0f)
    {
        Result = BIT(UP);
    }
    if(Direction.Y < 0.0f)
    {
        Result = BIT(DOWN); 
    }
    return Result;
}

entity *GetEntitiByID(entity *Entities, i32 ID)
{
    if(ID < 2)
    {
        return 0;
    }
    return &Entities[ID - 2];
}

void MoveHeroParty(entity *Entities, v2 OldPosition)
{
    Entities[1].OldPosiotion = Entities[1].Position;
    Entities[1].NextPosition = OldPosition;
    Entities[1].IsWalking = true;
    Entities[1].Facing = GetFacing(Entities[1].NextPosition, Entities[1].OldPosiotion, Entities[1].Facing);

    Entities[2].OldPosiotion = Entities[2].Position;
    Entities[2].NextPosition = Entities[1].OldPosiotion;
    Entities[2].IsWalking = true;
    Entities[2].Facing = GetFacing(Entities[2].NextPosition, Entities[2].OldPosiotion, Entities[2].Facing);

    Entities[3].OldPosiotion = Entities[3].Position;
    Entities[3].NextPosition = Entities[2].OldPosiotion;
    Entities[3].IsWalking = true;
    Entities[3].Facing = GetFacing(Entities[3].NextPosition, Entities[3].OldPosiotion, Entities[3].Facing);
}

void GetHeroInput(input *Input, entity *Entity, tilemap *Tilemap, entity *Entities)
{
    if(Input->Buttons->Start.IsDown != Input->Buttons->Start.WasDown)
    {
        if(Input->Buttons->Start.IsDown)
        {
            v2 PositionToCheck = Entity->Position;
            if(GET_BIT(Entity->Facing, UP))
            {
                PositionToCheck.Y += 16.0f;
                Entity->Action[0] = GetTilemapValue(Tilemap, PositionToCheck, Entity->Layer) - 2;
            }
            else if(GET_BIT(Entity->Facing, DOWN))
            {
                PositionToCheck.Y -= 16.0f;
                Entity->Action[0] = GetTilemapValue(Tilemap, PositionToCheck, Entity->Layer) - 2;
            }
            else if(GET_BIT(Entity->Facing, LEFT))
            {
                PositionToCheck.X -= 16.0f;
                Entity->Action[0] = GetTilemapValue(Tilemap, PositionToCheck, Entity->Layer) - 2;
            }
            else if(GET_BIT(Entity->Facing, RIGHT))
            {
                PositionToCheck.X += 16.0f;
                Entity->Action[0] = GetTilemapValue(Tilemap, PositionToCheck, Entity->Layer) - 2;
            }
            entity *EntityToCheck = GetEntitiByID(Entities, Entity->Action[0] + 2);
            if(EntityToCheck && !EntityToCheck->IsWalking && (EntityToCheck->Type == ENEMY))
            {
                // TODO(manuto): Search for more entities near by
                // fisrt search on the positive x axis
                v2 ShearchPosition = PositionToCheck;
                ShearchPosition.X += 16.0f;
                i32 NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                i32 Counter = 1;
                EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                while(EntityToCheck && (EntityToCheck->Type == ENEMY))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.X += 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                    if(Counter >= 4) break; 
                }
                // search the negative x axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.X -= 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                while(EntityToCheck && (EntityToCheck->Type == ENEMY))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.X -= 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                    if(Counter >= 4) break; 
                }
                // search on the positive y axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.Y += 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                while(EntityToCheck && (EntityToCheck->Type == ENEMY))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.Y += 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                    if(Counter >= 4) break; 
                }
                // search the negative y axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.Y -= 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                while(EntityToCheck && (EntityToCheck->Type == ENEMY))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.Y -= 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    EntityToCheck = GetEntitiByID(Entities, NewTileValue + 2);
                    if(Counter >= 4) break; 
                }
                Entity->NumbOfActions = Counter;
            }
            else
            {
                Entity->NumbOfActions = 0;
            }
        }
    }
    else
    {
        Entity->Action[0] = -1;
        Entity->Action[1] = -1;
        Entity->Action[2] = -1;
        Entity->Action[3] = -1;
    }

    if(!Entity->IsWalking)
    {
        if(Input->Buttons->Up.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.Y += 16.0f;
            Entity->Facing = BIT(UP);
            i32 ID = GetTilemapValue(Tilemap, NextPosition, 0);
            entity *Collision = GetEntitiByID(Entities, ID);
            if(!IsCollision(NextPosition, 0, Tilemap) || (Collision && (Collision->Type == HERO)))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
                MoveHeroParty(Entities, Entity->OldPosiotion);
            }
        }
        else if(Input->Buttons->Down.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.Y -= 16.0f;
            Entity->Facing = BIT(DOWN);
            i32 ID = GetTilemapValue(Tilemap, NextPosition, 0);
            entity *Collision = GetEntitiByID(Entities, ID);
            if(!IsCollision(NextPosition, 0, Tilemap) || (Collision && (Collision->Type == HERO)))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
                MoveHeroParty(Entities, Entity->OldPosiotion);
            }
        }    
        else if(Input->Buttons->Left.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.X -= 16.0f;
            Entity->Facing = BIT(LEFT);
            i32 ID = GetTilemapValue(Tilemap, NextPosition, 0);
            entity *Collision = GetEntitiByID(Entities, ID);
            if(!IsCollision(NextPosition, 0, Tilemap) || (Collision && (Collision->Type == HERO)))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
                MoveHeroParty(Entities, Entity->OldPosiotion);
            }
        }
        else if(Input->Buttons->Right.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.X += 16.0f;
            Entity->Facing = BIT(RIGHT);
            i32 ID = GetTilemapValue(Tilemap, NextPosition, 0);
            entity *Collision = GetEntitiByID(Entities, ID);
            if(!IsCollision(NextPosition, 0, Tilemap) || (Collision && (Collision->Type == HERO)))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
                MoveHeroParty(Entities, Entity->OldPosiotion);
            }
        }
    }
}

void SetEntityInRandomDirection(entity *Entity, tilemap *Tilemap)
{
    if(!Entity->IsWalking && Entity->WaitTime <= 0.0f)
    {
        i32 RandValue = rand() % 4;
        Entity->OldPosiotion = Entity->Position;
        v2 NextPosition = Entity->OldPosiotion;
        if(RandValue == 0)
        {
            NextPosition.Y += 16.0f; 
            Entity->Facing = BIT(UP);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
        else if(RandValue == 1)
        { 
            NextPosition.Y -= 16.0f; 
            Entity->Facing = BIT(DOWN);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
        else if(RandValue == 2)
        {
            NextPosition.X -= 16.0f; 
            Entity->Facing = BIT(LEFT);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
        else if(RandValue == 3)
        {
            NextPosition.X += 16.0f; 
            Entity->Facing = BIT(RIGHT);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
    }
}

void MoveEntity(entity *Entity, tilemap *Tilemap, r32 DeltaTime)
{
    if(Entity->IsWalking)
    {   
        v2 ActualPosition = Lerp(Entity->OldPosiotion, Entity->NextPosition, Entity->WalkDistance);
        Entity->Position.X = floorf(ActualPosition.X);
        Entity->Position.Y = floorf(ActualPosition.Y);
        if(Entity->WalkDistance > 1.0f)
        {
            UnsetCollision(Tilemap, Entity->OldPosiotion, Entity->Layer);
            Entity->IsWalking = false;
            Entity->WaitTime = Entity->TimeToWait;
            Entity->WalkDistance = 0.0f;
            i32 TileX = Entity->NextPosition.X / 16;
            i32 TileY = Entity->NextPosition.Y / 16;
            SetEntityPosition(Entity, Tilemap, TileX, TileY);
        }
        Entity->WalkDistance += 2.0f*DeltaTime;

        // TODO(manuto): Update Player Animation
        Entity->Frame = (u32)Entity->AnimTimer % 4;
        Entity->AnimTimer += 8.0f*DeltaTime;
    }
    else
    { 
        Entity->WaitTime -= DeltaTime;
        Entity->Frame = 0;
    }

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
}

bool MoveEntityInCombat(entity *Entity, r32 DeltaTime)
{
    if(Entity->IsWalking)
    {   
        v2 ActualPosition = Lerp(Entity->OldPosiotion, Entity->NextPosition, Entity->WalkDistance);
        Entity->Position.X = floorf(ActualPosition.X);
        Entity->Position.Y = floorf(ActualPosition.Y);
        if(Entity->WalkDistance > 1.0f)
        {
            Entity->IsWalking = false;
            Entity->WalkDistance = 0.0f;
            return false;
        }
        Entity->WalkDistance += 1.0f*DeltaTime;

        // TODO(manuto): Update Player Animation
        Entity->Frame = (u32)Entity->AnimTimer % 4;
        Entity->AnimTimer += 12.0f*DeltaTime;
    }
    else
    { 
        Entity->Frame = 0;
    }

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
    return true;
}
