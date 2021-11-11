void InitEditor(editor *Editor, texture *Texture, u32 TileWidth, u32 TileHeight, arena *Arena, game_state *GameState)
{
    // Create tilesheet
    tilesheet TileSheet = {};
    texture_info TextureInfo = GetTextureInfo(Texture);
    TileSheet.Texture = Texture;
    TileSheet.TexWidth = TextureInfo.Width;
    TileSheet.TexHeight = TextureInfo.Height; 
    TileSheet.TileWidth = TileWidth;
    TileSheet.TileHeight = TileHeight;
    // Load Editor
    Editor->TileSheet = TileSheet;
    Editor->Tilemap.TileWidth = TileWidth;
    Editor->Tilemap.TileHeight = TileHeight;
    Editor->Tilemap.Rows = TILEMAP_ROWS;
    Editor->Tilemap.Cols = TILEMAP_COLS;
    Editor->Zoom = 0.5f;
    Editor->Tilemap.Layers = PushStruct(Arena, layer);
    ++Editor->Tilemap.LayersCount;

    // Load Textures for editor only
    Editor->UITexture0 = CreateTexture(GameState->Renderer, "../data/TileSelector.bmp", &GameState->EngineArena);
    Editor->UITexture1 = CreateTexture(GameState->Renderer, "../data/addlayer.bmp",&GameState->EngineArena);
    Editor->UITexture2 = CreateTexture(GameState->Renderer, "../data/selectlayer.bmp", &GameState->EngineArena);
}

bool PointOnQuad(i32 X, i32 Y, i32 Width, i32 Height,
                       i32 XPos, i32 YPos)
{
    if(XPos >= X && XPos <= X + Width &&
       YPos >= Y && YPos <= Y + Height)
    {
        return true;
    }
    return false;
}

void SetBrushValue(input *Input, editor *Editor)
{
    tilesheet *TileSheet = &Editor->TileSheet;
    if(PointOnQuad(TileSheet->Position.X, TileSheet->Position.Y,
                   TileSheet->TexWidth, TileSheet->TexHeight,
                   Input->MouseX, Input->MouseY))
    {
        Editor->MouseOnUI = true;
        if(Input->MouseButtons->Left.IsDown != Input->MouseButtons->Left.WasDown)
        {
            if(Input->MouseButtons->Left.IsDown)
            {
                i32 XFrame = (i32)((Input->MouseX - TileSheet->Position.X) / TileSheet->TileWidth);
                i32 YFrame = (i32)((Input->MouseY - TileSheet->Position.Y) / TileSheet->TileHeight);
                i32 NumberOfCols = TileSheet->TexWidth / TileSheet->TileWidth;
                Editor->BrushValue = YFrame * NumberOfCols + XFrame;
            }
        }
    }
    else
    { 
        Editor->MouseOnUI = false;
    }
}

void PaintTilemap(input *Input, editor *Editor, v3 CamPosition)
{
    tilemap *Tilemap = &Editor->Tilemap;
    if(!Editor->MouseOnUI)
    {
        if(Input->MouseButtons->Left.IsDown)
        {
            // TODO(manuto): mapping mouse to camera size TEST...
            r32 XRatio = (WND_WIDTH*Editor->Zoom) / WND_WIDTH;
            r32 YRatio = (WND_HEIGHT*Editor->Zoom) / WND_HEIGHT;

            // Adjust the camera...
            CamPosition.X -= WND_WIDTH*0.5f*XRatio;
            CamPosition.Y -= WND_HEIGHT*0.5f*YRatio;

            i32 MouseX = (Input->MouseX * XRatio) + CamPosition.X;
            i32 MouseY = (Input->MouseY * YRatio) + CamPosition.Y;

            if(PointOnQuad(0, 0,
                           Tilemap->Cols*Tilemap->TileWidth - 1,
                           Tilemap->Rows*Tilemap->TileHeight - 1,
                           MouseX, MouseY))
            {
                i32 XFrame = (i32)(MouseX / Tilemap->TileWidth);
                i32 YFrame = (i32)(MouseY / Tilemap->TileHeight);
                u32 Index = YFrame * Tilemap->Cols + XFrame;

                Tilemap->Layers->Tiles[Index].Base = Editor->BrushValue; 
            }
        }
    }
}


void UpdateAndRenderEditor(game_state *GameState, input *Input, r32 DeltaTime)
{
    editor *Editor = &GameState->Editor;
    float CameraSpeed = 200.0f;
    if(Input->Buttons->Up.IsDown)
    {
        GameState->CamPosition.Y += CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Down.IsDown)
    {
        GameState->CamPosition.Y -= CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Left.IsDown)
    { 
        GameState->CamPosition.X -= CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Right.IsDown)
    { 
        GameState->CamPosition.X += CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Start.IsDown)
    { 
        Editor->Zoom -= DeltaTime; 
    }
    if(Input->Buttons->Back.IsDown)
    { 
        Editor->Zoom += DeltaTime; 
    }

    if(Editor->Zoom >= 2.0f)
    {
        Editor->Zoom = 2.0f;
    }
    if(Editor->Zoom <= 0.1f)
    {
        Editor->Zoom = 0.1f;
    }

    GameState->CamTarget.X = GameState->CamPosition.X;
    GameState->CamTarget.Y = GameState->CamPosition.Y;
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

    SetBrushValue(Input, Editor);
    PaintTilemap(Input, Editor, GameState->CamPosition);

    // TODO: Render Tilemaip...
    tilemap *Tilemap = &Editor->Tilemap;
    tilesheet *TileSheet = &Editor->TileSheet;

    for(i32 Y = 0;
        Y < Tilemap->Rows;
        ++Y)
    {
        for(i32 X = 0;
            X < Tilemap->Cols;
            ++X)
        {
            i32 Index = Y * Tilemap->Cols + X;
            u32 TileSheetCols = TileSheet->TexWidth / TileSheet->TileWidth;
            u32 XFrame = Tilemap->Layers->Tiles[Index].Base % TileSheetCols;
            u32 YFrame = Tilemap->Layers->Tiles[Index].Base / TileSheetCols;  
            
            mat4 Scale = ScaleMat4({(r32)Tilemap->TileWidth, (r32)Tilemap->TileHeight, 0.0f});
            mat4 Trans = TranslationMat4({(r32)Tilemap->TileWidth * X, (r32)Tilemap->TileHeight * Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);
            RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                        GameState->FrameConstBuffer, TileSheet->TileWidth, TileSheet->TileHeight, XFrame, YFrame);
        }
    } 


    // Render UI
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));

    // TODO: Render TileSheet
    mat4 Scale = ScaleMat4({(r32)TileSheet->TexWidth, (r32)TileSheet->TexHeight, 0.0f});
    mat4 Trans = TranslationMat4({TileSheet->Position.X - WND_WIDTH*0.5f,
                                  TileSheet->Position.Y - WND_HEIGHT*0.5f,
                                  0.0f});
    SetWorldMat4(GameState, Trans * Scale);
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIShader, TileSheet->Texture);

    r32 XPos = 0.0f;
    r32 YPos = 0.0f;
    r32 Width = 32.0f;
    r32 Height = 32.0f;
    r32 XOffset = 5.0f;
    r32 YOffset = 5.0f;

    for(i32 Index = 0;
        Index < 3;
        ++Index)
    {
        r32 ActualX = WND_WIDTH - 37.0f; 
        r32 ActualY = (YPos * (Height + YOffset)) + 5.0f; 

        mat4 Scale = ScaleMat4({Width, Height, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Editor->UITexture0,
                    GameState->FrameConstBuffer, 16, 16, Index, 0);

        ++YPos;
    }
    for(i32 Index = 0;
        Index < 2;
        ++Index)
    {
        r32 ActualX = WND_WIDTH - 37.0f; 
        r32 ActualY = (YPos * (Height + YOffset)) + 5.0f; 

        mat4 Scale = ScaleMat4({Width, Height, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Editor->UITexture1,
                    GameState->FrameConstBuffer, 16, 16, 1 - Index, 0);

        ++YPos;
    }
    for(i32 Index = 0;
        Index < 2;
        ++Index)
    {
        r32 ActualX = WND_WIDTH - 37.0f; 
        r32 ActualY = (YPos * (Height + YOffset)) + 5.0f; 

        mat4 Scale = ScaleMat4({Width, Height, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Editor->UITexture2,
                    GameState->FrameConstBuffer, 16, 16, 1 - Index, 0);

        ++YPos;
    }

    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*GameState->Editor.Zoom,
                                              WND_HEIGHT*GameState->Editor.Zoom,
                                              1.0f, 100.0f));
}
