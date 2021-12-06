#include "game_utility.cpp"
#include "tilemap.cpp"
#include "map_editor.cpp"
#include "stack.cpp"
#include "tree.cpp"
#include "inventory.cpp"
#include "hero.cpp"
#include "combat.cpp"
#include "init.cpp"
#include "dialogue.cpp"
#include "menu.cpp"

void GameSetUp(memory *Memory)
{
    game_state *GameState = (game_state *)Memory->Data;
    Memory->Used = sizeof(game_state);
    
    // Get memory for the game
    InitArena(Memory, &GameState->EngineArena, Megabytes(10));
    InitArena(Memory, &GameState->BatchArena, (64*64)*(24*6));  // (64*64) * (24*6)
    InitArena(Memory, &GameState->TileMapArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorArena, Megabytes(1));
    InitArena(Memory, &GameState->MapEditorSaves, Megabytes(2));
    InitArena(Memory, &GameState->IntToCharTempArena, sizeof(u32) * 10);
    InitArena(Memory, &GameState->TextureArena, Kilobytes(1));
    InitArena(Memory, &GameState->TexListArena, Kilobytes(1));
    InitArena(Memory, &GameState->TreeArena, Kilobytes(1));
    InitArena(Memory, &GameState->EntityTreeArena, Kilobytes(1));
    InitArena(Memory, &GameState->MenuArena, Kilobytes(1));
 
    // Init window and renderer stuff
    GameState->Window = PlatformCreateWindow(0, 0, WND_WIDTH, WND_HEIGHT, "MorphProject", &GameState->EngineArena);
    GameState->Renderer = PlatformCreateRenderer(GameState->Window, &GameState->EngineArena);
    InitShaders(GameState);
    InitConstBuffers(GameState);
    InitTextures(GameState);

    // Create a vertex Buffer.
    r32 Vertices[] = 
    {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };   
    GameState->Mesh = CreateMesh(GameState->Renderer, Vertices, ArrayCount(Vertices), &GameState->EngineArena); 
    GameState->TilemapBatch = CreateBatch(GameState->Renderer, &GameState->BatchArena, 64*64, &GameState->EngineArena);
    GameState->Tilemap = LoadMap(GameState, "../data/map.save");
    
    // Init gameplay
    InitEntities(GameState);
    InitWeapons(GameState);
    InitSpells(GameState);
    InitItems(GameState);
    InitEditor(&GameState->Editor, 16, 16, &GameState->MapEditorArena, GameState);

    // Initialize Camera Position and Target
    GameState->CamPosition.X = GameState->Entities[2].Position.X;
    GameState->CamPosition.Y = GameState->Entities[2].Position.Y;
    GameState->CamPosition.Z = -0.1f;
    if(GameState->CamPosition.X - (WND_WIDTH*0.5f*0.5f) < 0)
    {
        GameState->CamPosition.X -= GameState->CamPosition.X - (WND_WIDTH*0.5f*0.5f);
    } 
    if(GameState->CamPosition.X + (WND_WIDTH*0.5f*0.5f) > 64*16)
    {
        GameState->CamPosition.X -= (GameState->CamPosition.X + (WND_WIDTH*0.5f*0.5f) - (64*16));
    }
    if(GameState->CamPosition.Y - (WND_HEIGHT*0.5f*0.5f) < 0)
    {
        GameState->CamPosition.Y -= GameState->CamPosition.Y - (WND_HEIGHT*0.5f*0.5f); 
    }
    if(GameState->CamPosition.Y + (WND_HEIGHT*0.5f*0.5f) > 64*16)
    { 
        GameState->CamPosition.Y -= (GameState->CamPosition.Y + (WND_HEIGHT*0.5f*0.5f) - (64*16));
    }
    GameState->CamTarget.X = GameState->CamPosition.X;
    GameState->CamTarget.Y = GameState->CamPosition.Y;
    GameState->CamTarget.Z = 0.0f;
    SetWorldMat4(GameState, IdentityMat4());
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));
    SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));

    // Set up the game state
    GameState->AppState = GAME_STATE;
    GameState->GamePlayState = WORLD;

    GameState->HeroPartyCount = 4;

    InitInventory(&GameState->Inventory);
    AddItem(&GameState->Inventory, 1, 2);
    AddItem(&GameState->Inventory, 2, 3);
    AddItem(&GameState->Inventory, 1, 2);
}

void GameUpdateAndRender(memory *Memory, input *Input, r32 DeltaTime)
{
    game_state *GameState = (game_state *)Memory->Data;
    // TODO(manuto): Update...
    if(OnKeyDown(Input->Buttons->Debug))
    {
        if(GameState->AppState == GAME_STATE)
        {
            GameState->AppState = EDITOR_STATE;
        }
        else if(GameState->AppState == EDITOR_STATE)
        {
            GameState->AppState = GAME_STATE;
        }
    }
 
    if(GameState->AppState == GAME_STATE)
    {
        // Update...
        if(GameState->GamePlayState == WORLD)
        {
            if(OnKeyDown(Input->Buttons->Back))
            {
                InitMenu(GameState, &GameState->Menu, &GameState->MenuArena);
                GameState->GamePlayState = MENU;
            }

            if(!GameState->DialogueAction.ShowingDialogue)
            {
                GetHeroInput(Input ,&GameState->Entities[0], &GameState->Tilemap, GameState->Entities);
                for(i32 Index = 0;
                    Index < ArrayCount(GameState->Entities);
                    ++Index)
                {
                    entity *Entity = &GameState->Entities[Index];
                    if((Index > 3) && (Entity->TimeToWait >= 0.0f) && Entity->Alive)
                    {
                        SetEntityInRandomDirection(Entity, &GameState->Tilemap);
                    }

                    if(((Entity->ID - 2) < GameState->HeroPartyCount) || ((Entity->ID - 2) >= 4))
                    {
                        MoveEntity(Entity, &GameState->Tilemap, DeltaTime);
                    }
                }
            }
            else
            {
                GetDialogueInput(GameState, Input, &GameState->DialogueAction); 
            }
            
            v2 NewCamPos = GameState->Entities[0].Position;

            if(NewCamPos.X - (WND_WIDTH*0.5f*0.5f) < 0 ||
               NewCamPos.X + (WND_WIDTH*0.5f*0.5f) > 64*16)
            {
                NewCamPos.X = GameState->CamPosition.X;
            }
            if(NewCamPos.Y - (WND_HEIGHT*0.5f*0.5f) < 0 ||
               NewCamPos.Y + (WND_HEIGHT*0.5f*0.5f) > 64*16)
            { 
                NewCamPos.Y = GameState->CamPosition.Y;
            }
            GameState->CamPosition.X = NewCamPos.X;
            GameState->CamPosition.Y = NewCamPos.Y;
            GameState->CamTarget.X = GameState->CamPosition.X;
            GameState->CamTarget.Y = GameState->CamPosition.Y;
            SetViewMat4(GameState, ViewMat4(GameState->CamPosition, GameState->CamTarget, {0.0f, 1.0f, 0.0f}));
            
            SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH*0.5f, WND_HEIGHT*0.5f, 1.0f, 100.0f));

            i32 EntityID = GameState->Entities[0].Action[0];
            entity *EntityToCheck = GetEntitiByID(GameState->Entities, EntityID + 2);
            if(EntityToCheck && EntityToCheck->Alive)
            {
                switch(EntityToCheck->Type)
                {
                    case ENEMY:
                    {
                        GameState->GamePlayState = COMBAT;
                        InitCombat(GameState, &GameState->Combat, &GameState->Entities[0]); 
                    }break;
                    case NPC:
                    {
                        InitDialogue(&GameState->Dialogue);
                        GameState->DialogueAction.ShowingDialogue = true;
                        GameState->DialogueAction.Entity = EntityToCheck;
                        GameState->Entities[0].Action[0] = -1;
                    }break;
                    default:
                    {}break;
                }
            }

            // Render...
            tilemap *Tilemap = &GameState->Tilemap;        
            if(Tilemap->LayersCount > 0)
            {
                for(i32 Index = 0;
                    Index < Tilemap->LayersCount;
                    ++Index)
                {
                    layer *FirstLayer = Tilemap->Layers;
                    FirstLayer -= (Tilemap->LayersCount - 1);
                    layer *ActualLayer = FirstLayer + Index;

                    RenderLayer(GameState, Tilemap, ActualLayer, false, 0.5f);
                    entity Entities[8];
                    memcpy(Entities, GameState->Entities, 8*sizeof(entity));
                    InsertionSort(Entities, 8); 
                    for(i32 I = 0;
                        I < ArrayCount(Entities);
                        ++I)
                    {
                        if(Index == Entities[I].Layer)
                        {
                            if(Entities[I].Alive && (((Entities[I].ID - 2) < GameState->HeroPartyCount) || ((Entities[I].ID - 2) >= 4)))
                            {
                                mat4 Scale = ScaleMat4({16, 24, 0.0f});
                                mat4 Trans = TranslationMat4({Entities[I].Position.X, Entities[I].Position.Y, 0.0f});
                                SetWorldMat4(GameState, Trans * Scale);
                                RenderFrame(GameState->Renderer, GameState->Mesh, GameState->FrameShader, GameState->HeroTexture,
                                            GameState->FrameConstBuffer, 16, 24, Entities[I].Frame, Entities[I].Skin); 
                            }
                        }
                    }
                }
            }
            if(GameState->DialogueAction.ShowingDialogue)
            {
                UpdateAndRenderDialogue(GameState, GameState->DialogueAction.Entity, DeltaTime);
            }

        }
        else if(GameState->GamePlayState == COMBAT)
        {
            UpdateAndRenderCombat(GameState, &GameState->Combat, Input, DeltaTime);
        }
        else if(GameState->GamePlayState == MENU) 
        {
            //SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));
            //UpdateAndRenderInventory(GameState, Input);
            UpdateAndRenderMenu(GameState, Input);
        }
    }
        
    if(GameState->AppState == EDITOR_STATE)
    {
        UpdateAndRenderEditor(GameState, Input, DeltaTime);
    }

#if 1
    // Show information about the memory of the system and debug info
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f));
    RenderString(GameState, "FPS: ", -WND_WIDTH*0.5f, (WND_HEIGHT*0.5f - 9));
    RenderUInt(GameState, (u32)Input->FPS, -WND_WIDTH*0.5f + 5*7, (WND_HEIGHT*0.5f - 9));
    i32 XPos = WND_WIDTH*0.5f - 205;
    i32 YPos = WND_HEIGHT*0.5f - (9);
    RenderMemoryData(GameState, &GameState->EngineArena, "Engine Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TextureArena, "Texture Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TileMapArena, "Tilemap Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->MapEditorArena, "Map Editor Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->MapEditorSaves, "Map Editor Saves Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TexListArena, "TexList Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->BatchArena, "Batch Renderer Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->TreeArena, "Tree Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->EntityTreeArena, "Entity Tree Arena", &XPos, &YPos);
    RenderMemoryData(GameState, &GameState->MenuArena, "Menu Arena", &XPos, &YPos);
#endif 

}
