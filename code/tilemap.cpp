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
    Map.TexIndex = SaveLayer->TexIndex;
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
    return true;
}
