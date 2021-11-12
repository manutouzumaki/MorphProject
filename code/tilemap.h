#ifndef TILEMAP_H
#define TILEMAP_H

#define TILEMAP_ROWS 64
#define TILEMAP_COLS 64

struct tile
{
    u32 Base;
    u32 Decoration;
    u32 Collision; 
};

struct layer
{
    tile Tiles[TILEMAP_ROWS*TILEMAP_COLS]; // for now our tilemap are going to be 64x64 tiles
};

struct tilemap
{
    u32 Cols;
    u32 Rows;
    u32 TileWidth;
    u32 TileHeight;
    layer *Layers;
    u32 LayersCount;
};

#endif
