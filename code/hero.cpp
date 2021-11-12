void SetEntityPosition(entity *Entity, i32 TileX, i32 TileY)
{
    Entity->Position.X = TileX * 16.0f;
    Entity->Position.Y = TileY * 16.0f;
}

void GetHeroInput(input *Input, entity *Entity, tilemap *Tilemap)
{
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
                Entity->NextPosition = NextPosition;
                Entity->IsWalking = true;
            }
        }
    }
}

void MoveEntity(entity *Entity, r32 DeltaTime)
{
    if(Entity->IsWalking)
    {
        static float T = 0.0f;
        v2 ActualPosition = Lerp(Entity->OldPosiotion, Entity->NextPosition, T);
        Entity->Position.X = floorf(ActualPosition.X);
        Entity->Position.Y = floorf(ActualPosition.Y);
        if(T > 1.0f)
        {
            Entity->IsWalking = false;
            T = 0.0f;
            i32 TileX = Entity->NextPosition.X / 16;
            i32 TileY = Entity->NextPosition.Y / 16;
            SetEntityPosition(Entity, TileX, TileY);
        }
        T += 2.0f*DeltaTime;

        // TODO(manuto): Update Player Animation
        static r32 Counter = 0.0f;
        Entity->Frame = (u32)Counter % 4;
        Counter += 5.0f*DeltaTime;
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
}
