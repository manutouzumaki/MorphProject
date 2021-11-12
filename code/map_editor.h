#ifndef MAPEDITOR_H
#define MAPEDITOR_H

struct tilesheet
{
    texture *Texture;
    i32 TexWidth;
    i32 TexHeight;
    i32 TileWidth;
    i32 TileHeight;
    v2 Position;
};

enum z_selected
{
    BASE,
    DECORATION,
    COLLISION,
};

struct ui_state
{
    v2 Position;
    v2 Size;
    v2 Offset;
};

struct editor
{
    tilesheet TileSheet;
    tilemap Tilemap;
    u32 BrushValue;
    r32 Zoom;
    bool MouseOnUI;

    texture *UITexture0;
    texture *UITexture1;
    texture *UITexture2;

    z_selected ZSelected;
    ui_state UIState;
};

#endif
