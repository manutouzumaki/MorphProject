#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#define SINGLE_BUTTON -1
#define NO_BUTTON_SELECTED -2
#define PERSISTANT_BUTTON -3

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

enum add_layer_state
{
    REMOVE_LAYER,
    ADD_LAYER,
};

enum layer_selected
{
    DOWN_LAYER,
    UP_LAYER,
};

enum scroll
{
    SCROLL_UP,
    SCROLL_DOWN,
    SCROLL_LEFT,
    SCROLL_RIGHT
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
    texture *UITexture3;
    texture *UITexture4;

    z_selected ZSelected;
    i32 LayerSelected;
    u32 LayersShowBitField;
    i32 TileSheetSelected;    
};

#endif
