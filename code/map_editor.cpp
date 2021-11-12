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
    Editor->Zoom = 1.0f;
    Editor->Tilemap.Layers = PushStruct(Arena, layer);
    ++Editor->Tilemap.LayersCount;
    Editor->ZSelected = BASE;

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
                if(Editor->ZSelected == BASE)
                    Tilemap->Layers->Tiles[Index].Base = Editor->BrushValue; 
                if(Editor->ZSelected == DECORATION)
                    Tilemap->Layers->Tiles[Index].Decoration = Editor->BrushValue; 
                if(Editor->ZSelected == COLLISION)
                    Tilemap->Layers->Tiles[Index].Collision = Editor->BrushValue; 
            }
        }
    }
}

i32 UpdateAndRenderEditorUI(game_state *GameState, input *Input, ui_state *UIState, texture *Texture, i32 NumOfButtons, i32 ButtonSelected)
{
    i32 OutValue = -1;
    for(i32 Index = 0;
        Index < NumOfButtons;
        ++Index)
    {
        r32 ActualX = UIState->Position.X;//(UIState->Position.X * (UIState->Size.X + UIState->Offset.X)) + 0.0f; 
        r32 ActualY = (UIState->Position.Y * (UIState->Size.Y + UIState->Offset.Y)) + 5.0f; 
        // Update
        if(PointOnQuad(ActualX, ActualY, UIState->Size.X, UIState->Size.Y,
                       Input->MouseX, Input->MouseY))
        {
            GameState->Editor.MouseOnUI = true;
            if(Input->MouseButtons->Left.IsDown != Input->MouseButtons->Left.WasDown)
            {
                if(Input->MouseButtons->Left.IsDown)
                {
                    OutValue = Index;
                }
            }
        }

        // Render
        mat4 Scale = ScaleMat4({UIState->Size.X, UIState->Size.Y, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        i32 Offset = 0;
        if(Index == ButtonSelected && ButtonSelected != -1)
        {
            Offset = NumOfButtons;
        }
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                    GameState->FrameConstBuffer, 16, 16, Index + Offset, 0);

        ++UIState->Position.Y;
    }
    return OutValue;
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
    if(Input->Buttons->Start.IsDown != Input->Buttons->Start.WasDown)
    {
        if(Input->Buttons->Start.IsDown)
        { 
            if(Editor->Zoom == 1.0f)
                Editor->Zoom = 0.5f;
            else
               Editor->Zoom = 1.0f; 
        }
    }

    GameState->CamPosition.X = floorf(GameState->CamPosition.X);
    GameState->CamPosition.Y = floorf(GameState->CamPosition.Y);
    GameState->CamTarget.X = GameState->CamPosition.X;
    GameState->CamTarget.Y = GameState->CamPosition.Y;
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

    Editor->MouseOnUI = false;
    SetBrushValue(Input, Editor);

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
            
            mat4 Scale = ScaleMat4({(r32)Tilemap->TileWidth, (r32)Tilemap->TileHeight, 0.0f});
            mat4 Trans = TranslationMat4({(r32)Tilemap->TileWidth * X, (r32)Tilemap->TileHeight * Y, 0.0f});
            SetWorldMat4(GameState, Trans * Scale);

            if(Tilemap->Layers->Tiles[Index].Base != 0)
            {
                u32 XFrame = Tilemap->Layers->Tiles[Index].Base % TileSheetCols;
                u32 YFrame = Tilemap->Layers->Tiles[Index].Base / TileSheetCols; 
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, TileSheet->TileWidth, TileSheet->TileHeight, XFrame, YFrame);
            }
            if(Tilemap->Layers->Tiles[Index].Decoration != 0)
            {
                u32 XFrame = Tilemap->Layers->Tiles[Index].Decoration % TileSheetCols;
                u32 YFrame = Tilemap->Layers->Tiles[Index].Decoration / TileSheetCols; 
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, TileSheet->TileWidth, TileSheet->TileHeight, XFrame, YFrame);
            }
            // TODO(manuto): Only render collition in editor
            if(Tilemap->Layers->Tiles[Index].Collision != 0)
            {
                u32 XFrame = Tilemap->Layers->Tiles[Index].Collision % TileSheetCols;
                u32 YFrame = Tilemap->Layers->Tiles[Index].Collision / TileSheetCols; 
                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->MapTexture,
                            GameState->FrameConstBuffer, TileSheet->TileWidth, TileSheet->TileHeight, XFrame, YFrame);
            }
                        
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


    ui_state UIState = {};
    UIState.Position = {WND_WIDTH - 37.0f, 0.0f};
    UIState.Size = {32.0f, 32.0f};
    UIState.Offset = {5.0f, 5.0f};
    
    i32 OutValue;
    OutValue = UpdateAndRenderEditorUI(GameState, Input, &UIState, Editor->UITexture0, 3, Editor->ZSelected);
    if(OutValue == BASE)
    {
        Editor->ZSelected = BASE;
    }
    if(OutValue == DECORATION)
    {
        Editor->ZSelected = DECORATION;
    }
    if(OutValue == COLLISION)
    {
        Editor->ZSelected = COLLISION;
    }
    OutValue = UpdateAndRenderEditorUI(GameState, Input, &UIState, Editor->UITexture1, 2, -1);
    OutValue = UpdateAndRenderEditorUI(GameState, Input, &UIState, Editor->UITexture2, 2, -1);
    //Editor->MouseOnUI = MouseOnUI;
    
    // TODO(manuto): Affter setup everithing we paint
    PaintTilemap(Input, Editor, GameState->CamPosition);

    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*GameState->Editor.Zoom,
                                              WND_HEIGHT*GameState->Editor.Zoom,
                                              1.0f, 100.0f));
}
