#ifndef MAIN_H
#define MAIN_H

#include "tilemap.h"
#include "map_editor.h"
#include "stack.h"
#include "tree.h"
#include "inventory.h"
#include "hero.h"
#include "entities_def.h"
#include "combat.h"
#include "dialogue.h"
#include "menu.h"
#include "tp.h"

struct vs_constant_buffer
{
    mat4 Projection;
    mat4 World;
    mat4 View;
};

struct color_const_buffer
{
    v3 Color;
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

enum gameplay_state
{
    WORLD,
    COMBAT,
    MENU,
};

#define MAX_NUM_OF_ENTITIES 255

struct game_state
{
    window *Window;
    renderer *Renderer;

    app_state AppState;
    gameplay_state GamePlayState; 

    arena EngineArena;
    arena TextureArena;
    arena TileMapArena;
    arena MapEditorArena;
    arena MapEditorSaves;
    arena IntToCharTempArena;
    arena TexListArena;
    arena BatchArena;
    arena TreeArena;
    arena EntityTreeArena;
    arena MenuArena;

    const_buffer *ConstBuffer;
    const_buffer *ColorConstBuffer;
    const_buffer *FrameConstBuffer;
    const_buffer *MemoryConstBuffer;
    const_buffer *UITileSheetConstBuffer;
    vs_constant_buffer ConstBufferData = {};
    
    shader *MainShader;
    shader *FrameShader;
    shader *UIShader;
    shader *UISimpleShader;
    shader *UIColorShader;
    shader *UIFrameShader;
    shader *MemBarShader;
    shader *BatchShader;

    texture *HeroTexture;
    texture *HeroPortraitTexture;
    texture *FontTexture;
    texture *TilesheetTextures;
    texture *CombatBgTexture;
    texture *CombatSlashTexture;
    texture *FireTexture;
    texture *RestHPTexture;
    texture *RestMPTexture;
    texture *ReviveTexture;
    texture *UseItemTexture;

    u32 TilesheetTexturesCount;
    texture_list TexList;

    mesh *Mesh;
    batch *TilemapBatch;

    v3 CamPosition;
    v3 CamOffset;
    v3 CamTarget;
    
    entity Entities[MAX_NUM_OF_ENTITIES];
    i32 EntitiesCount;

    weapon_stats Weapons[3];
    spells_stats Spells[3];
    item_stats Items[2]; 
    inventory Inventory;

    i32 ActualTilemapX;
    i32 ActualTilemapY;
    tilemap ActualTilemap;

    combat Combat;
    u32 HeroPartyCount;
    dialogue_action DialogueAction;
    dialoque_state Dialogue;
    menu Menu;

    editor Editor;
};

#endif
