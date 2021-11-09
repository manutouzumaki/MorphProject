struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    mat4 View;
};

struct frame_const_buffer
{
    v2 TexSize;
    v2 TileSize;
    v2 Frame;
};

u32 Tiles1[] = {        
    48, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 37, 37, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 48,
};

u32 Collitions[] {
    48, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 37, 37, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48,
    48, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 48,
};

struct entity
{
    u32 ID;
    v2 Position;
    u32 Frame;
    bool IsWalking;
    bool Up;
    bool Down;
    bool Left;
    bool Right;
};

struct game_state
{
    window *Window;
    renderer *Renderer;

    arena EngineArena;

    const_buffer *ConstBuffer;
    const_buffer *FrameConstBuffer;
    vs_constant_buffer ConstBufferData= {};
    
    shader *Shader;
    mesh *Mesh;
    texture *MapTexture;
    texture *HeroTexture;

    // Camera Test
    v3 CamPosition;
    v3 CamTarget;

    entity HeroEntity;
    v2 OldPosiotion;
    v2 NextPosition;
};

bool IsCollision(v2 Position)
{
    i32 TileX = Position.X / 16;
    i32 TileY = Position.Y / 16;
    if(TileX >= 0 && TileX < 16 && TileY >= 0 && TileY < 16)
    {
        return Collitions[TileY * 16 + TileX] != 0;
    }
    return true;
}

void SetEntityPosition(entity *Entity, i32 TileX, i32 TileY)
{
    Entity->Position.X = TileX * 16.0f;
    Entity->Position.Y = TileY * 16.0f;
}

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);

    InitArena(Memory, &GameState->EngineArena, Megabytes(100));

    GameState->Window = PlatformCreateWindow(0, 0, 800, 600, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);

    GameState->Shader = CompileShadersFromFile(GameState->Renderer,
                                               "../code/shaders/test_vert.hlsl",  
                                               "../code/shaders/test_frag.hlsl",
                                               &GameState->EngineArena);

    GameState->ConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(vs_constant_buffer), &GameState->EngineArena);
    GameState->FrameConstBuffer = CreateConstBuffer(GameState->Renderer, sizeof(frame_const_buffer), &GameState->EngineArena);
    
    GameState->CamPosition = {400.0f, 300.0f, -0.1f};
    GameState->CamTarget = {400.0f, 300.0f, 0.0f};

    GameState->ConstBufferData.Projection = OrthogonalProjMat4(800, 600, 1.0f, 100.0f);
    GameState->ConstBufferData.World = IdentityMat4();
    GameState->ConstBufferData.View = ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f});
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);

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
    GameState->MapTexture = CreateTexture(GameState->Renderer, "../data/town_tileset.bmp", &GameState->EngineArena);
    GameState->HeroTexture = CreateTexture(GameState->Renderer, "../data/walk_cycle.bmp", &GameState->EngineArena);

    SetEntityPosition(&GameState->HeroEntity, 8, 11);
}

void GameUpdateAndRender(memory *Memory, input *Input, r32 DeltaTime)
{
    game_state *GameState = (game_state *)Memory->Data;
    // TODO(manuto): Update...
        
    if(!GameState->HeroEntity.IsWalking)
    {
        if(Input->Buttons->Up.IsDown)
        {
            GameState->OldPosiotion = GameState->HeroEntity.Position;
            v2 NextPosition = GameState->OldPosiotion;
            NextPosition.Y += 16.0f;
            if(!IsCollision(NextPosition))
            {
                GameState->NextPosition = NextPosition;
                GameState->HeroEntity.IsWalking = true;
                GameState->HeroEntity.Up = true;
                GameState->HeroEntity.Down = false;
                GameState->HeroEntity.Left = false;
                GameState->HeroEntity.Right = false;
            }
        }
        if(Input->Buttons->Down.IsDown)
        {
            GameState->OldPosiotion = GameState->HeroEntity.Position;
            v2 NextPosition = GameState->OldPosiotion;
            NextPosition.Y -= 16.0f;
            if(!IsCollision(NextPosition))
            {
                GameState->NextPosition = NextPosition;
                GameState->HeroEntity.IsWalking = true;
                GameState->HeroEntity.Up = false;
                GameState->HeroEntity.Down = true;
                GameState->HeroEntity.Left = false;
                GameState->HeroEntity.Right = false;
            }
        }    
        if(Input->Buttons->Left.IsDown)
        {
            GameState->OldPosiotion = GameState->HeroEntity.Position;
            v2 NextPosition = GameState->OldPosiotion;
            NextPosition.X -= 16.0f;
            if(!IsCollision(NextPosition))
            {
                GameState->NextPosition = NextPosition;
                GameState->HeroEntity.IsWalking = true;
                GameState->HeroEntity.Up = false;
                GameState->HeroEntity.Down = false;
                GameState->HeroEntity.Left = true;
                GameState->HeroEntity.Right = false;

            }
        }
        if(Input->Buttons->Right.IsDown)
        {
            GameState->OldPosiotion = GameState->HeroEntity.Position;
            v2 NextPosition = GameState->OldPosiotion;
            NextPosition.X += 16.0f;
            if(!IsCollision(NextPosition))
            {
                GameState->NextPosition = NextPosition;
                GameState->HeroEntity.IsWalking = true;
                GameState->HeroEntity.Up = false;
                GameState->HeroEntity.Down = false;
                GameState->HeroEntity.Left = false;
                GameState->HeroEntity.Right = true;

            }
        }
    }

    if(GameState->HeroEntity.IsWalking)
    {
        static float T = 0.0f;
        v2 ActualPosition = Lerp(GameState->OldPosiotion, GameState->NextPosition, T);
        GameState->HeroEntity.Position = ActualPosition;
        if(T > 1.0f)
        {
            GameState->HeroEntity.IsWalking = false;
            T = 0.0f;
            i32 TileX = GameState->NextPosition.X / 16;
            i32 TileY = GameState->NextPosition.Y / 16;
            SetEntityPosition(&GameState->HeroEntity, TileX, TileY);
        }
        T += 2.0f*DeltaTime;


        // TODO(manuto): Update Player Animation
        static r32 Counter = 0.0f;
        GameState->HeroEntity.Frame = (u32)Counter % 4;
        Counter += 5.0f*DeltaTime;

        if(GameState->HeroEntity.Up)
        {
            GameState->HeroEntity.Frame += 0; 
        }
        if(GameState->HeroEntity.Down)
        {
            GameState->HeroEntity.Frame += 8; 
        }
        if(GameState->HeroEntity.Left)
        {
            GameState->HeroEntity.Frame += 12; 
        }
        if(GameState->HeroEntity.Right)
        {
            GameState->HeroEntity.Frame += 4; 
        }


    }

    GameState->CamPosition.X = GameState->HeroEntity.Position.X;
    GameState->CamPosition.Y = GameState->HeroEntity.Position.Y;
    GameState->CamTarget.X = GameState->HeroEntity.Position.X;
    GameState->CamTarget.Y = GameState->HeroEntity.Position.Y;
    GameState->ConstBufferData.View = ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f});
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer, &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0);

    // TODO(manuto): Render...    
    u32 Row = 15;
    for(i32 Y = 0;
        Y < 16;
        ++Y)
    {
        for(i32 X = 0;
            X < 16;
            ++X)
        {
            i32 Index = Y * 16 + X;
            u32 TileSheetCols = 176 / 16;
            u32 XFrame = Tiles1[Index] % TileSheetCols;
            u32 YFrame = Tiles1[Index] / TileSheetCols;  
            
            mat4 Scale = ScaleMat4({16, 16, 0.0f});
            mat4 Trans = TranslationMat4({16.0f * X, 16.0f * Y, 0.0f});
            GameState->ConstBufferData.World = Trans * Scale;
            MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                           &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->Shader, GameState->MapTexture,
                        GameState->FrameConstBuffer, 16, 16, XFrame, YFrame);
        }
        --Row;
    } 

    mat4 Scale = ScaleMat4({16, 24, 0.0f});
    mat4 Trans = TranslationMat4({GameState->HeroEntity.Position.X, GameState->HeroEntity.Position.Y, 0.0f});
    GameState->ConstBufferData.World = Trans * Scale;
    MapConstBuffer(GameState->Renderer, GameState->ConstBuffer,
                   &GameState->ConstBufferData, sizeof(vs_constant_buffer), 0); 
    RenderFrame(GameState->Renderer, GameState->Mesh, GameState->Shader, GameState->HeroTexture,
                GameState->FrameConstBuffer, 16, 24, GameState->HeroEntity.Frame, 0);

}
