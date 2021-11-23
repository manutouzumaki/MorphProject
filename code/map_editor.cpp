void InitEditor(editor *Editor, u32 TileWidth, u32 TileHeight, arena *Arena, game_state *GameState)
{
    // Get the first texture on the list
    texture *FirstTexture = GameState->TilesheetTextures;
    FirstTexture -= (GameState->TilesheetTexturesCount - 1);
    // Create tilesheet
    tilesheet TileSheet = {};
    texture_info TextureInfo = GetTextureInfo(FirstTexture);
    TileSheet.Texture = FirstTexture;
    TileSheet.TexWidth = TextureInfo.Width;
    TileSheet.TexHeight = TextureInfo.Height; 
    TileSheet.TileWidth = TileWidth;
    TileSheet.TileHeight = TileHeight;
    // Load Editor
    Editor->TileSheet = TileSheet;
    Editor->TileSheetSelected = 0;
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
    Editor->UITexture3 = CreateTexture(GameState->Renderer, "../data/showLayer.bmp", &GameState->EngineArena);
    Editor->UITexture4 = CreateTexture(GameState->Renderer, "../data/arrows.bmp", &GameState->EngineArena);
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

bool SaveTilemapToFile(char *FileName, editor *Editor, arena *Arena)
{
    layer *FirstLayer = Editor->Tilemap.Layers;
    FirstLayer -= (Editor->Tilemap.LayersCount - 1);
    
    save_layers *SaveLayers;
    for(i32 Index = 0;
        Index < Editor->Tilemap.LayersCount;
        ++Index)
    {
        layer *ActualLayer = FirstLayer + Index;
        SaveLayers = PushStruct(Arena, save_layers);
        SaveLayers->Cols = Editor->Tilemap.Cols;
        SaveLayers->Rows = Editor->Tilemap.Rows;
        SaveLayers->TileWidth = Editor->Tilemap.TileWidth;
        SaveLayers->TileHeight = Editor->Tilemap.TileHeight;
        SaveLayers->LayersCount = Editor->Tilemap.LayersCount;
        SaveLayers->TexWidth = Editor->TileSheet.TexWidth;
        SaveLayers->TexHeight = Editor->TileSheet.TexHeight;
        memcpy(SaveLayers->Tiles, ActualLayer->Tiles, TILEMAP_ROWS*TILEMAP_COLS*sizeof(tile));
    }
    save_layers *Data = SaveLayers;
    Data -= (Editor->Tilemap.LayersCount - 1);
    bool Result = WriteEntireFile(FileName, sizeof(save_layers) * Editor->Tilemap.LayersCount, (void *)Data);
    Arena->Used = 0;
    return Result;
}

void SetBrushValue(input *Input, editor *Editor)
{
    tilesheet *TileSheet = &Editor->TileSheet;
    if(PointOnQuad(TileSheet->Position.X, TileSheet->Position.Y,
                   176.0f, 288.0f,
                   Input->MouseX, Input->MouseY))
    {
        Editor->MouseOnUI = true;
        if(Input->MouseButtons->Left.IsDown != Input->MouseButtons->Left.WasDown)
        {
            if(Input->MouseButtons->Left.IsDown)
            {
                i32 XFrame = (i32)((Input->MouseX - (TileSheet->Position.X*TileSheet->TexWidth)) / TileSheet->TileWidth);
                i32 YFrame = (i32)((Input->MouseY - (TileSheet->Position.Y*TileSheet->TexHeight)) / TileSheet->TileHeight);
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

                layer *FirstLayer = Editor->Tilemap.Layers;
                FirstLayer -= (Editor->Tilemap.LayersCount - 1);
                layer *ActualLayer = FirstLayer + Editor->LayerSelected;

                if(Editor->ZSelected == BASE)
                {
                    ActualLayer->Tiles[Index].Base = Editor->BrushValue; 
                    ActualLayer->Tiles[Index].BaseTexIndex = Editor->TileSheetSelected;
                }
                if(Editor->ZSelected == DECORATION)
                {
                    ActualLayer->Tiles[Index].Decoration = Editor->BrushValue; 
                    ActualLayer->Tiles[Index].DecorationTexIndex = Editor->TileSheetSelected;
                }
                if(Editor->ZSelected == COLLISION)
                {
                    ActualLayer->Tiles[Index].Collision = Editor->BrushValue; 
                    ActualLayer->Tiles[Index].CollisionTexIndex = Editor->TileSheetSelected;
                }
            }
        }
    }
}

i32 UpdateAndRenderEditorUIHorizontal(game_state *GameState, input *Input, ui_state *UIState, texture *Texture, i32 NumOfButtons, i32 ButtonSelected)
{
    i32 OutValue = -1;
    for(i32 Index = 0;
        Index < NumOfButtons;
        ++Index)
    {
        i32 Offset = 0;
        r32 ActualX = (UIState->Position.X * (UIState->Size.X + UIState->Offset.X)) + 0.0f; 
        r32 ActualY = UIState->Position.Y;//(UIState->Position.Y * (UIState->Size.Y + UIState->Offset.Y)) + 5.0f; 
        // Update
        if(PointOnQuad(ActualX, ActualY, UIState->Size.X, UIState->Size.Y,
                       Input->MouseX, Input->MouseY))
        {
            GameState->Editor.MouseOnUI = true;
            if(ButtonSelected != PERSISTANT_BUTTON)
            {
                if(Input->MouseButtons->Left.IsDown != Input->MouseButtons->Left.WasDown)
                {
                    if(Input->MouseButtons->Left.IsDown)
                    {
                        OutValue =  Index;
                        if(ButtonSelected == SINGLE_BUTTON)
                        {
                            Offset = NumOfButtons;
                        }
                    }
                }
            }
            else
            {
                if(Input->MouseButtons->Left.IsDown)
                {
                    OutValue =  Index;
                }
            }
        }

        // Render
        mat4 Scale = ScaleMat4({UIState->Size.X, UIState->Size.Y, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        if(Index == ButtonSelected && ButtonSelected != SINGLE_BUTTON && ButtonSelected != NO_BUTTON_SELECTED)
        {
            Offset = NumOfButtons;
        }
        RenderFrame(GameState->Renderer, GameState->Mesh, GameState->UIFrameShader, Texture,
                    GameState->FrameConstBuffer, 16, 16, Index + Offset, 0);

        ++UIState->Position.X;
    }
    return OutValue;

}
i32 UpdateAndRenderEditorUIVertical(game_state *GameState, input *Input, ui_state *UIState, texture *Texture, i32 NumOfButtons, i32 ButtonSelected)
{
    i32 OutValue = -1;
    for(i32 Index = 0;
        Index < NumOfButtons;
        ++Index)
    {
        i32 Offset = 0;
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
                    OutValue =  Index;
                    if(ButtonSelected == SINGLE_BUTTON)
                    {
                        Offset = NumOfButtons;
                    }
                }
            }
        }

        // Render
        mat4 Scale = ScaleMat4({UIState->Size.X, UIState->Size.Y, 0.0f});
        mat4 Trans = TranslationMat4({ActualX - WND_WIDTH*0.5f, ActualY - WND_HEIGHT*0.5f, 0.0f});
        SetWorldMat4(GameState, Trans * Scale);
        if(Index == ButtonSelected && ButtonSelected != SINGLE_BUTTON && ButtonSelected != NO_BUTTON_SELECTED)
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
        GameState->CamOffset.Y += CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Down.IsDown)
    {
        GameState->CamOffset.Y -= CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Left.IsDown)
    { 
        GameState->CamOffset.X -= CameraSpeed * DeltaTime; 
    }
    if(Input->Buttons->Right.IsDown)
    { 
        GameState->CamOffset.X += CameraSpeed * DeltaTime; 
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
    if(Input->Buttons->Back.IsDown != Input->Buttons->Back.WasDown)
    {
        if(Input->Buttons->Back.IsDown)
        {
            if(SaveTilemapToFile("../data/new_map/map.save", Editor, &GameState->MapEditorSaves))
            {
                OutputDebugString("Map saved!!\n");
            }
        }
    }

    GameState->CamPosition.X = floorf(GameState->CamOffset.X);
    GameState->CamPosition.Y = floorf(GameState->CamOffset.Y);
    GameState->CamTarget.X = GameState->CamPosition.X;
    GameState->CamTarget.Y = GameState->CamPosition.Y;
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

    Editor->MouseOnUI = false;
    SetBrushValue(Input, Editor);
    
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*GameState->Editor.Zoom,
                                              WND_HEIGHT*GameState->Editor.Zoom,
                                              1.0f, 100.0f));
    // TODO: Render Tilemap...
    texture *FirstTexture = GameState->TilesheetTextures;
    FirstTexture -= (GameState->TilesheetTexturesCount - 1);

    tilemap *Tilemap = &Editor->Tilemap;
    tilesheet *TileSheet = &Editor->TileSheet;

    TileSheet->Texture = FirstTexture + Editor->TileSheetSelected;
    texture_info TextureInfo = GetTextureInfo(TileSheet->Texture);
    TileSheet->TexWidth = TextureInfo.Width;
    TileSheet->TexHeight = TextureInfo.Height;

    for(i32 Index = 0;
        Index < Editor->Tilemap.LayersCount;
        ++Index)
    {
        if(!GET_BIT(Editor->LayersShowBitField, Index))
        {
            layer *FirstLayer = Editor->Tilemap.Layers;
            FirstLayer -= (Editor->Tilemap.LayersCount - 1);
            layer *ActualLayer = FirstLayer + Index;
            RenderLayer(GameState, &Editor->Tilemap, ActualLayer, true, Editor->Zoom);
        }
    } 

    // Render UI
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));

    // TODO: Render TileSheet
    tilesheet_ui_const_buffer ConstBufferData = {};
    ConstBufferData.TexSize.X = TileSheet->TexWidth;
    ConstBufferData.TexSize.Y = TileSheet->TexHeight;
    ConstBufferData.Offset.X = TileSheet->Position.X;
    ConstBufferData.Offset.Y = TileSheet->Position.Y;
    MapConstBuffer(GameState->Renderer, GameState->UITileSheetConstBuffer, &ConstBufferData, sizeof(tilesheet_ui_const_buffer), 1);
    mat4 Scale = ScaleMat4({176.0f, 288.0f, 0.0f});
    mat4 Trans = TranslationMat4({-WND_WIDTH*0.5f, -WND_HEIGHT*0.5f, 0.0f});
    SetWorldMat4(GameState, Trans * Scale);
    RenderMesh(GameState->Renderer, GameState->Mesh, GameState->UIShader, TileSheet->Texture);

    // TODO(manuto): Improve this SUPER hacke way to render th UI
    ui_state UIState = {};
    UIState.Position = {0.0f, 288.0f + 5.0f};
    UIState.Size = {32.0f, 32.0f};
    UIState.Offset = {5.0f, 5.0f}; 
    i32 OutValue = UpdateAndRenderEditorUIHorizontal(GameState, Input, &UIState, Editor->UITexture4, 4, PERSISTANT_BUTTON);
    if(OutValue == SCROLL_UP)
    {
        if(TileSheet->TexHeight > 288.0f)
        {
            TileSheet->Position.Y -= 0.5f * DeltaTime;
            if(TileSheet->Position.Y < -((TileSheet->TexHeight - 288.0f)/TileSheet->TexHeight))
            {
                TileSheet->Position.Y = -((TileSheet->TexHeight - 288.0f)/TileSheet->TexHeight);
            }
        }
    }
    if(OutValue == SCROLL_DOWN)
    {
        TileSheet->Position.Y += 0.5f * DeltaTime;
        if(TileSheet->Position.Y > 0.0f)
        {
            TileSheet->Position.Y = 0.0f;
        }
    }
    if(OutValue == SCROLL_LEFT)
    {
        TileSheet->Position.X += 0.5f * DeltaTime;
        if(TileSheet->Position.X > 0.0f)
        {
            TileSheet->Position.X = 0.0f;
        }
    }
    if(OutValue == SCROLL_RIGHT)
    {

        if(TileSheet->TexWidth > 176.0f)
        {
            TileSheet->Position.X -= 0.5f * DeltaTime;
            if(TileSheet->Position.X < -((TileSheet->TexWidth - 176.0f)/TileSheet->TexWidth))
            {
                TileSheet->Position.X = -((TileSheet->TexWidth - 176.0f)/TileSheet->TexWidth);
            }
        }
    }

    UIState = {};
    UIState.Position = {176.0f + 5, 0.0f};
    UIState.Size = {32.0f, 32.0f};
    UIState.Offset = {5.0f, 5.0f}; 
    OutValue = UpdateAndRenderEditorUIVertical(GameState, Input, &UIState, Editor->UITexture2, 2, SINGLE_BUTTON);
    if(OutValue == UP_LAYER)
    {
        OutputDebugString("Next Texture \n");
        TileSheet->Position.Y = 0.0f;
        TileSheet->Position.X = 0.0f;
        ++Editor->TileSheetSelected;
        if(Editor->TileSheetSelected >= GameState->TilesheetTexturesCount)
        {
            Editor->TileSheetSelected = GameState->TilesheetTexturesCount - 1;
        }   
    }
    if(OutValue == DOWN_LAYER)
    {
        OutputDebugString("Back Texture\n");
        TileSheet->Position.Y = 0.0f;
        TileSheet->Position.X = 0.0f;
        --Editor->TileSheetSelected;
        if(Editor->TileSheetSelected < 0)
        {
            Editor->TileSheetSelected = 0;
        }
    }

    UIState = {};
    UIState.Position = {WND_WIDTH - 37.0f, 0.0f};
    UIState.Size = {32.0f, 32.0f};
    UIState.Offset = {5.0f, 5.0f}; 
    OutValue = UpdateAndRenderEditorUIVertical(GameState, Input, &UIState, Editor->UITexture0, 3, Editor->ZSelected);
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
    OutValue = UpdateAndRenderEditorUIVertical(GameState, Input, &UIState, Editor->UITexture1, 2, SINGLE_BUTTON);
    if(OutValue == ADD_LAYER)
    {
        OutputDebugString("Add LAYER\n");
        Editor->Tilemap.Layers = PushStruct(&GameState->MapEditorArena, layer);
        ++Editor->Tilemap.LayersCount;
        Editor->LayerSelected = Editor->Tilemap.LayersCount - 1;
    }
    if(OutValue == REMOVE_LAYER)
    {
        if(Editor->Tilemap.LayersCount > 1)
        {
            OutputDebugString("Remove LAYER\n");
            GameState->MapEditorArena.Used -= sizeof(layer);
            for(i32 Index = 0;
                Index < TILEMAP_ROWS*TILEMAP_COLS;
                ++Index)
            {
                tile ZeroTile = {};
                Editor->Tilemap.Layers->Tiles[Index] = ZeroTile;
            }
            --Editor->Tilemap.Layers;
            --Editor->Tilemap.LayersCount;
            --Editor->LayerSelected;
            if(Editor->LayerSelected < 0)
            {
                Editor->LayerSelected = 0;
            }
        }
    }
    OutValue = UpdateAndRenderEditorUIVertical(GameState, Input, &UIState, Editor->UITexture2, 2, SINGLE_BUTTON);
    if(OutValue == UP_LAYER)
    {
        OutputDebugString("Up LAYER\n");
        ++Editor->LayerSelected;
        if(Editor->LayerSelected >= Editor->Tilemap.LayersCount)
        {
            Editor->LayerSelected = Editor->Tilemap.LayersCount - 1;
        }
    }
    if(OutValue == DOWN_LAYER)
    {
        OutputDebugString("Down LAYER\n");
        --Editor->LayerSelected;
        if(Editor->LayerSelected < 0)
        {
            Editor->LayerSelected = 0;
        }
    }

    UIState = {};
    UIState.Position = {WND_WIDTH - (32.0f*1.5f + 10), 0.0f};
    UIState.Size = {16.0f, 16.0f};
    UIState.Offset = {5.0f, 5.0f};
    for(i32 Index = 0;
        Index < Editor->Tilemap.LayersCount;
        ++Index)
    {
        i32 ButtonSelected = 0;
        if(!GET_BIT(Editor->LayersShowBitField, Index))
        {
            ButtonSelected = NO_BUTTON_SELECTED; 
        }
        OutValue = UpdateAndRenderEditorUIVertical(GameState, Input, &UIState, Editor->UITexture3, 1, ButtonSelected);
        if(OutValue != -1)
        {
            if(GET_BIT(Editor->LayersShowBitField, Index))
            {
                UNSET_BIT(Editor->LayersShowBitField, Index);
            } 
            else
            {
                SET_BIT(Editor->LayersShowBitField, Index);
            }
        }
    }

    // Affter setup everithing we paint
    PaintTilemap(Input, Editor, GameState->CamPosition);
}
