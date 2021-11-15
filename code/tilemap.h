#ifndef TILEMAP_H
#define TILEMAP_H

#define TILEMAP_ROWS 64
#define TILEMAP_COLS 64

struct tile
{
    u32 Base;
    i32 BaseTexIndex;

    u32 Decoration;
    i32 DecorationTexIndex;

    u32 Collision;
    i32 CollisionTexIndex;
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
    u32 LayersCount;
    u32 TexWidth;
    u32 TexHeight;
    layer *Layers;
};

struct save_layers
{
    u32 Cols;
    u32 Rows;
    u32 TileWidth;
    u32 TileHeight;
    u32 LayersCount;
    u32 TexWidth;
    u32 TexHeight;
    tile Tiles[TILEMAP_ROWS*TILEMAP_COLS];
};

#endif
