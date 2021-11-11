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
