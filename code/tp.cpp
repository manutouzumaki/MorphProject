void UpdateTPs(game_state *GameState, tp *TPs, entity *Heroes)
{
    entity *Hero = &Heroes[0];
    u32 HeroTileX = Hero->Position.X / 16;
    u32 HeroTileY = Hero->Position.Y / 16;
    
    if((HeroTileX == TPs->FromTileX) && (HeroTileY == TPs->FromTileY))
    {
        GameState->ActualTilemap = &GameState->Tilemap[0][0];
        SetEntityPosition(&Heroes[0], GameState->ActualTilemap, TPs->ToTileX, TPs->ToTileY);
        SetEntityPosition(&Heroes[1], GameState->ActualTilemap, TPs->ToTileX, TPs->ToTileY);
        SetEntityPosition(&Heroes[2], GameState->ActualTilemap, TPs->ToTileX, TPs->ToTileY);
        SetEntityPosition(&Heroes[3], GameState->ActualTilemap, TPs->ToTileX, TPs->ToTileY);
    }
}
