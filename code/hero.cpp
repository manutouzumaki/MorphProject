void SetEntityPosition(entity *Entity, tilemap *Tilemap, i32 TileX, i32 TileY)
{
    Entity->Position.X = TileX * 16.0f;
    Entity->Position.Y = TileY * 16.0f;
    SetCollition(Tilemap, Entity->Position, Entity->Layer, Entity->ID);
}

void GetHeroInput(input *Input, entity *Entity, tilemap *Tilemap)
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

            // TODO(manuto): Search for more entities near by
            if(Entity->Action[0] >= 0 && Entity->Action[0] != (Entity->ID - 2))
            {
                // fisrt search on the positive x axis
                v2 ShearchPosition = PositionToCheck;
                ShearchPosition.X += 16.0f;
                i32 NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                i32 Counter = 1;
                while(NewTileValue >= 0 && NewTileValue != (Entity->ID - 2))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.X += 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    if(Counter >= 4) break; 
                }
                // search the negative x axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.X -= 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                while(NewTileValue >= 0 && NewTileValue != (Entity->ID - 2))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.X -= 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    if(Counter >= 4) break; 
                }

                // search on the positive y axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.Y += 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                while(NewTileValue >= 0 && NewTileValue != (Entity->ID - 2))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.Y += 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    if(Counter >= 4) break; 
                }
                // search the negative y axis
                ShearchPosition = PositionToCheck;
                ShearchPosition.Y -= 16.0f;
                NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                while(NewTileValue >= 0 && NewTileValue != (Entity->ID - 2))
                {
                    Entity->Action[Counter++] = NewTileValue;
                    ShearchPosition.Y -= 16.0f;
                    NewTileValue = (GetTilemapValue(Tilemap, ShearchPosition, Entity->Layer) - 2);
                    if(Counter >= 4) break; 
                }
                Entity->NumbOfActions = Counter;
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
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
        else if(Input->Buttons->Down.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.Y -= 16.0f;
            Entity->Facing = BIT(DOWN);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }    
        else if(Input->Buttons->Left.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
            NextPosition.X -= 16.0f;
            Entity->Facing = BIT(LEFT);
            if(!IsCollision(NextPosition, 0, Tilemap))
            {
                SetCollition(Tilemap, NextPosition, Entity->Layer, Entity->ID);
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
        else if(Input->Buttons->Right.IsDown)
        {
            Entity->OldPosiotion = Entity->Position;
            v2 NextPosition = Entity->OldPosiotion;
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
