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
