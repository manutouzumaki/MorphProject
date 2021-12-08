#ifndef TP_H
#define TP_H

// special object that is use for teleporting the player 
// arrown diferents maps
struct tp
{
    u32 FromTileX;
    u32 FromTileY;
    u32 ToTileX;
    u32 ToTileY;
    u32 ToTilemapX;
    u32 ToTilemapY;
};

#endif
