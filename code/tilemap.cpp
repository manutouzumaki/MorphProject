tilemap LoadMap(game_state *GameState, char *FileName)
{
    tilemap Map = {};
    // TODO(manuto): 
    // Load the file
    size_t FileSize = 0;
    void *Data = ReadEntireFile(FileName, &FileSize, &GameState->EngineArena);
    if(!Data)
    {
        return Map;
    }
    // And fill the tilemap whit the file data
    save_layers *SaveLayer = (save_layers *)Data;
    Map.Cols = SaveLayer->Cols;
    Map.Rows = SaveLayer->Rows;
    Map.TileWidth = SaveLayer->TileWidth;
    Map.TileHeight = SaveLayer->TileHeight;
    Map.LayersCount = SaveLayer->LayersCount;
    Map.TexWidth = SaveLayer->TexWidth;
    Map.TexHeight = SaveLayer->TexHeight;
    for(i32 Index = 0;
        Index < Map.LayersCount;
        ++Index)
    {
        Map.Layers = PushStruct(&GameState->TileMapArena, layer);
        memcpy(Map.Layers->Tiles, SaveLayer->Tiles, TILEMAP_ROWS*TILEMAP_COLS*sizeof(tile));
        ++SaveLayer;
    } 
    // delete the Data from the EngineArena 
    GameState->EngineArena.Used -= FileSize;
    return Map;
}

void ClearMap(game_state *GameState, tilemap *Map)
{
    GameState->TileMapArena.Used -= Map->LayersCount*sizeof(layer);
}

void RenderLayer(game_state *GameState, tilemap *Tilemap, layer *Layer, bool Collision, r32 Ratio)
{
    // Batch Rendering
    i32 XMin = (GameState->CamPosition.X - (WND_WIDTH*0.5f*Ratio)) / 16;
    i32 XMax = (GameState->CamPosition.X + (WND_WIDTH*0.5f*Ratio)) / 16;
    i32 YMin = (GameState->CamPosition.Y - (WND_HEIGHT*0.5f*Ratio)) / 16;
    i32 YMax = (GameState->CamPosition.Y + (WND_HEIGHT*0.5f*Ratio)) / 16;
    if(XMin < 0)
    {
        XMin = 0;
    }
    if(YMin < 0)
    {
        YMin = 0;
    }
    if(XMax > 63)
    {
        XMax = 63;
    }
    if(YMax > 63)
    {
        YMax = 63;
    }
    BeginBatch(GameState->TilemapBatch);
    for(i32 Y = YMin;
        Y <= YMax;
        ++Y)
    {
        for(i32 X = XMin;
            X <= XMax;
            ++X)
        {
            i32 Index = Y * Tilemap->Cols + X; 
            mat4 Scale = ScaleMat4({(r32)Tilemap->TileWidth, (r32)Tilemap->TileHeight, 0.0f});
            mat4 Trans = TranslationMat4({(r32)Tilemap->TileWidth * X, (r32)Tilemap->TileHeight * Y, 0.0f});
            if(Layer->Tiles[Index].Base != 0)
            {
                texture *FirstTexture = GameState->TilesheetTextures;
                FirstTexture -= (GameState->TilesheetTexturesCount - 1);
                texture *ActualTexture = FirstTexture + Layer->Tiles[Index].BaseTexIndex;
                u32 TileSheetCols = ActualTexture->Width / Tilemap->TileWidth;
                u32 XFrame = Layer->Tiles[Index].Base % TileSheetCols;
                u32 YFrame = Layer->Tiles[Index].Base / TileSheetCols;
                
                v2 TileSize = {16.0f, 16.0f};
                v2 Frame = {(r32)XFrame, (r32)YFrame};
                u32 TexIndex = Layer->Tiles[Index].BaseTexIndex;
                mat4 World = Trans * Scale;
                PushQuad(GameState, GameState->TilemapBatch, GameState->BatchShader,
                         World, TexIndex, TileSize, Frame);

            } 
            if(Layer->Tiles[Index].Decoration != 0)
            {
                texture *FirstTexture = GameState->TilesheetTextures;
                FirstTexture -= (GameState->TilesheetTexturesCount - 1);
                texture *ActualTexture = FirstTexture + Layer->Tiles[Index].DecorationTexIndex;
                u32 TileSheetCols = ActualTexture->Width / Tilemap->TileWidth;
                u32 XFrame = Layer->Tiles[Index].Decoration % TileSheetCols;
                u32 YFrame = Layer->Tiles[Index].Decoration / TileSheetCols;
                
                v2 TileSize = {16.0f, 16.0f};
                v2 Frame = {(r32)XFrame, (r32)YFrame};
                u32 TexIndex = Layer->Tiles[Index].DecorationTexIndex;
                mat4 World = Trans * Scale;
                PushQuad(GameState, GameState->TilemapBatch, GameState->BatchShader,
                         World, TexIndex, TileSize, Frame);

            }
            if(Collision && Layer->Tiles[Index].Collision != 0)
            {
                texture *FirstTexture = GameState->TilesheetTextures;
                FirstTexture -= (GameState->TilesheetTexturesCount - 1);
                texture *ActualTexture = FirstTexture + Layer->Tiles[Index].CollisionTexIndex;
                u32 TileSheetCols = ActualTexture->Width / Tilemap->TileWidth; 
                u32 XFrame = Layer->Tiles[Index].Collision % TileSheetCols;
                u32 YFrame = Layer->Tiles[Index].Collision / TileSheetCols;

                v2 TileSize = {16.0f, 16.0f};
                v2 Frame = {(r32)XFrame, (r32)YFrame};
                u32 TexIndex = Layer->Tiles[Index].CollisionTexIndex;
                mat4 World = Trans * Scale;
                PushQuad(GameState, GameState->TilemapBatch, GameState->BatchShader,
                         World, TexIndex, TileSize, Frame);    
            } 
        }
    }
    EndBatch(GameState->Renderer, GameState->TilemapBatch, GameState->BatchShader, &GameState->TexList);
}

bool IsCollision(v2 Position, u32 Layer, tilemap *Tilemap)
{
    i32 TileX = Position.X / 16;
    i32 TileY = Position.Y / 16;
    if(TileX >= 0 && TileX < Tilemap->Cols && TileY >= 0 && TileY < Tilemap->Cols)
    {
        layer *FirstLayer = Tilemap->Layers;
        FirstLayer -= (Tilemap->LayersCount - 1);
        layer *ActualLayer = FirstLayer + Layer;

        return ActualLayer->Tiles[TileY * Tilemap->Cols + TileX].Collision != 0;
    }
    return false;
}

i32 GetTilemapValue(tilemap *Tilemap, v2 Position, u32 Layer)
{
    i32 TileX = Position.X / 16;
    i32 TileY = Position.Y / 16;
    if(TileX >= 0 && TileX < Tilemap->Cols && TileY >= 0 && TileY < Tilemap->Cols)
    {
        layer *FirstLayer = Tilemap->Layers;
        FirstLayer -= (Tilemap->LayersCount - 1);
        layer *ActualLayer = FirstLayer + Layer;

        return ActualLayer->Tiles[TileY * Tilemap->Cols + TileX].Collision;
    }
    return -1;

}

void SetCollition(tilemap *Tilemap, v2 Position, u32 Layer, u32 EntityID)
{
    i32 TileX = Position.X / 16;
    i32 TileY = Position.Y / 16;
    if(TileX >= 0 && TileX < Tilemap->Cols && TileY >= 0 && TileY < Tilemap->Cols)
    {
        layer *FirstLayer = Tilemap->Layers;
        FirstLayer -= (Tilemap->LayersCount - 1);
        layer *ActualLayer = FirstLayer + Layer;
        ActualLayer->Tiles[TileY * Tilemap->Cols + TileX].Collision = EntityID;
    }
}

void UnsetCollision(tilemap *Tilemap, v2 Position, u32 Layer)
{
    i32 TileX = Position.X / 16;
    i32 TileY = Position.Y / 16;
    if(TileX >= 0 && TileX < Tilemap->Cols && TileY >= 0 && TileY < Tilemap->Cols)
    {
        layer *FirstLayer = Tilemap->Layers;
        FirstLayer -= (Tilemap->LayersCount - 1);
        layer *ActualLayer = FirstLayer + Layer;
        ActualLayer->Tiles[TileY * Tilemap->Cols + TileX].Collision = 0;
    }

}
