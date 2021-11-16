#ifndef MAIN_H
#define MAIN_H

#include "tilemap.h"
#include "map_editor.h"
#include "hero.h"

struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    mat4 View;
};

struct frame_const_buffer
{
    v2 TexSize;
    v2 TileSize;
    v2 Frame;
};

struct  memory_const_buffer
{
    r32 MemoryData;
};

struct tilesheet_ui_const_buffer
{
    v2 TexSize;
    v2 Offset;
};

enum app_state
{
    GAME_STATE,
    EDITOR_STATE
};

struct game_state
{
    window *Window;
    renderer *Renderer;

    app_state AppState; 

    arena EngineArena;
    arena TextureArena;
    arena TileMapArena;
    arena MapEditorArena;
    arena MapEditorSaves;
    arena IntToCharTempArena;
    // for batch rendering
    arena TexListArena;
    arena BatchArena;

    const_buffer *ConstBuffer;
    const_buffer *FrameConstBuffer;
    const_buffer *MemoryConstBuffer;
    const_buffer *UITileSheetConstBuffer;
    vs_constant_buffer ConstBufferData = {};
    
    shader *MainShader;
    shader *FrameShader;
    shader *UIShader;
    shader *UIFrameShader;
    shader *MemBarShader;
    shader *BatchShader;

    mesh *Mesh;
    texture *MapTexture;
    texture *MapTexture2;
    texture *TilesheetTextures;
    u32 TilesheetTexturesCount;
    texture *HeroTexture;
    texture *FontTexture;
    batch *TilemapBatch;

    v3 CamPosition;
    v3 CamTarget;

    entity HeroEntity;
    tilemap Tilemap;
    texture_list TexList;

    editor Editor;
};

#endif
