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
    arena TileMapArena;
    arena MapEditorArena;
    arena MapEditorSaves;
    arena IntToCharTempArena;

    const_buffer *ConstBuffer;
    const_buffer *FrameConstBuffer;
    const_buffer *MemoryConstBuffer;
    vs_constant_buffer ConstBufferData = {};
    
    shader *MainShader;
    shader *FrameShader;
    shader *UIShader;
    shader *UIFrameShader;
    shader *MemBarShader;

    mesh *Mesh;
    texture *MapTexture;
    texture *MapTexture2;
    texture *HeroTexture;
    texture *FontTexture;

    v3 CamPosition;
    v3 CamTarget;

    entity HeroEntity;
    tilemap Tilemap;

    editor Editor;
};

#endif
