void CreateMenuTree(tree *Tree, inventory *Invetory, item_stats *Items, spells_stats *Spells, weapon_stats *Weapons)
{
    i32 MenuID = Tree->AddChildByID(0, "Menu", 0);
    // Main menu options
    i32 InventoryID = Tree->AddChildByID(0, "Inventory", MenuID);
    i32 SpellsID    = Tree->AddChildByID(1, "Spells", MenuID);
    i32 WeaponID    = Tree->AddChildByID(2, "Weapon", MenuID);
    i32 SaveID      = Tree->AddChildByID(3, "Save", MenuID);
    i32 ExitID      = Tree->AddChildByID(4, "Exit", MenuID);
    
    // Invetory
    for(i32 Index = 0;
        Index < Invetory->ItemsCount;
        ++Index)
    {
        item_stats Item = Items[Invetory->Items[Index].ID - 1];
        Tree->AddChildByID(Index, Item.Name, InventoryID); 
    }
    for(i32 Index = 0;
        Index < 3; // TODO: use the unlocked spells
        ++Index)
    {
        Tree->AddChildByID(Index, Spells[Index].Name, SpellsID);
    } 
    for(i32 Index = 0;
        Index < 3; // TODO: use the unlocked weapons
        ++Index)
    { 
        Tree->AddChildByID(Index, Weapons[Index].Name, WeaponID); 
    }
}

void InitMenu(game_state *GameState, menu *Menu, arena *Arena)
{
    Menu->Tree.Init(Arena);
    CreateMenuTree(&Menu->Tree, &GameState->Inventory, GameState->Items, GameState->Spells, GameState->Weapons);
    Menu->Option = Menu->Tree.First->Child;
    Menu->Hero = -1;
    Menu->SelectingHero = false;
    Menu->HeroSelected = false; 
}

void GetMenuOptionInput(game_state *GameState, menu *Menu, input *Input)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        if(Menu->Option->BackSibling)
        {
            Menu->Option = Menu->Option->BackSibling;
        }
    }
    if(OnKeyDown(Input->Buttons->Down))
    {
        if(Menu->Option->NextSibling)
        {
            Menu->Option = Menu->Option->NextSibling;
        }
    }
    if(OnKeyDown(Input->Buttons->Start))
    {
        if(Menu->Option->Child)
        {
            Menu->Option = Menu->Option->Child;
        }
        else if(Menu->Option->Parent->ID == OP_INVENTORY ||
                Menu->Option->Parent->ID == OP_SPELLS    ||
                Menu->Option->Parent->ID == OP_WEAPON)
        {
            Menu->Hero = 0;
            Menu->SelectingHero = true;
        }
        else if(Menu->Option->ID == OP_SAVE)
        { 
        }
        else if(Menu->Option->ID == OP_EXIT)
        { 
        }

    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        if(Menu->Option->Parent)
        {
            Menu->Option = Menu->Option->Parent;
            if(Menu->Option->ID == Menu->Tree.First->ID)
            {
                UpdateItems(&GameState->Inventory);
                GameState->GamePlayState = WORLD;
            }
        }
    }
}

void GetMenuHeroInput(menu *Menu, input *Input, i32 HeroPartyCount)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        --Menu->Hero;
        if(Menu->Hero < 0)
        {
            Menu->Hero = 0;
        }
    }
    if(OnKeyDown(Input->Buttons->Down))
    {
        ++Menu->Hero;
        if(Menu->Hero > (HeroPartyCount - 1))
        {
            Menu->Hero = (HeroPartyCount - 1);
        }
    }
    if(OnKeyDown(Input->Buttons->Start))
    {
        Menu->HeroSelected = true;
    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        Menu->Hero = -1;
        Menu->SelectingHero = false;
    }


}

// MAIN MENU
void RenderMainMenu(game_state *GameState, menu *Menu, tree::node *FirstSibling)
{
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    i32 XPos = -(WND_WIDTH*0.5f*0.5f);
    i32 YPos = 0 + FontSize.Y*3;
    RenderUIQuad(GameState, XPos - 10, YPos - (FontSize.Y*4) - 10, (WND_WIDTH*0.5f) + 20, (FontSize.Y*5) + 20, 35.0f, 57.0f, 91.0f);
    while(FirstSibling)
    {
        if(FirstSibling->ID == Menu->Option->ID)
        { 
            RenderUIQuad(GameState, XPos, YPos, (WND_WIDTH*0.5f), FontSize.Y, 134.0f, 165.0f, 217.0f);
        }
        RenderString(GameState, FirstSibling->Name, XPos, YPos, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        FirstSibling = FirstSibling->NextSibling;
    }
}

// TODO: INVENTORY MENU
void RenderInventoryMenu(game_state *GameState, menu *Menu, tree::node *FirstSibling)
{
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    char *StatsNames[] = {"HP: ", "MP: ", "ST: ", "SP: ", "IN: "};
    i32 XPos = 5;
    i32 YPos = (WND_HEIGHT*0.5f) - 10;
    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*15), (WND_WIDTH*0.5f) - 15, (FontSize.Y*15), 35.0f, 57.0f, 91.0f);
    while(FirstSibling)
    {
        if(FirstSibling->ID == Menu->Option->ID)
        { 
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - FontSize.Y, (WND_WIDTH*0.5f) - 35, FontSize.Y, 134.0f, 165.0f, 217.0f);
        }
        i32 XOffset = RenderString(GameState, FirstSibling->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        RenderUInt(GameState,  GameState->Inventory.Items[FirstSibling->Value].Count, XPos + 10 + ((XOffset+1)*FontSize.X), YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
       
        YPos -= FontSize.Y;
        FirstSibling = FirstSibling->NextSibling;
    }


    XPos = -(WND_WIDTH*0.5f) + 10;
    YPos = (WND_HEIGHT*0.5f) - 10;

    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*25), (WND_WIDTH*0.5f) - 15, FontSize.Y*25, 35.0f, 57.0f, 91.0f);
    for(i32 Index = 0;
        Index < GameState->HeroPartyCount;
        ++Index)
    {
        if(Menu->Hero == Index)
        {
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - (FontSize.Y*6), (WND_WIDTH*0.5f) - 35, FontSize.Y*6, 134.0f, 165.0f, 217.0f);
        }

        entity *Hero = &GameState->Entities[Index];
        RenderString(GameState, Hero->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        XPos += FontSize.X; 
        i32 NameIndex = 0;
        for(i32 Index = 0;
            Index < 4;
            Index += 2)
        {
            i32 XOffset = RenderString(GameState, StatsNames[NameIndex++], XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            i32 InnerXPos = XPos + (XOffset*(FontSize.X));
            XOffset = RenderUInt(GameState, Hero->Stats.Stat[Index], InnerXPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            InnerXPos += (XOffset*(FontSize.X));
            XOffset = RenderString(GameState, "/", InnerXPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            InnerXPos += (XOffset*(FontSize.X));
            RenderUInt(GameState, Hero->Stats.Stat[Index + 1], InnerXPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            YPos -= FontSize.Y;
        }
        for(i32 Index = 0;
            Index < 3;
            ++Index)
        {
            i32 XOffset = RenderString(GameState, StatsNames[Index + 2], XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            i32 InnerXPos = XPos + (XOffset*(FontSize.X));
            RenderUInt(GameState, Hero->Stats.Stat[Index + 4], InnerXPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            YPos -= FontSize.Y;
        }
        XPos -= FontSize.X; 
    }
}

// TODO: SPELLS MENU
void RenderSpellsMenu(game_state *GameState, menu *Menu, tree::node *FirstSibling)
{
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    i32 XPos = 5;
    i32 YPos = (WND_HEIGHT*0.5f) - 10;
    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*15), (WND_WIDTH*0.5f) - 15, (FontSize.Y*15), 35.0f, 57.0f, 91.0f);
    while(FirstSibling)
    {
        if(FirstSibling->ID == Menu->Option->ID)
        { 
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - FontSize.Y, (WND_WIDTH*0.5f) - 35, FontSize.Y, 134.0f, 165.0f, 217.0f);
        }
        RenderString(GameState, FirstSibling->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        FirstSibling = FirstSibling->NextSibling;
    }

    XPos = -(WND_WIDTH*0.5f) + 10;
    YPos = (WND_HEIGHT*0.5f) - 10;
    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*17), (WND_WIDTH*0.5f) - 15, FontSize.Y*17, 35.0f, 57.0f, 91.0f);
    for(i32 Index = 0;
        Index < GameState->HeroPartyCount;
        ++Index)
    {
        if(Menu->Hero == Index)
        {
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - (FontSize.Y*4), (WND_WIDTH*0.5f) - 35, FontSize.Y*4, 134.0f, 165.0f, 217.0f);
        }
        entity *Hero = &GameState->Entities[Index];
        RenderString(GameState, Hero->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        XPos += FontSize.X; 
        for(i32 Index = 0;
            Index < 3;
            ++Index)
        {
            if(Hero->Spells[Index] >= 0)
            {
                RenderString(GameState, GameState->Spells[Hero->Spells[Index]].Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            }
            else
            {
                RenderString(GameState, "Empty", XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
            }
            YPos -= FontSize.Y;
        }
        XPos -= FontSize.X; 
    }

}

// TODO: WEAPON MENU
void RenderWeaponMenu(game_state *GameState, menu *Menu, tree::node *FirstSibling)
{
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f}; 
    i32 XPos = 5;
    i32 YPos = (WND_HEIGHT*0.5f) - 10;
    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*15), (WND_WIDTH*0.5f) - 15, (FontSize.Y*15), 35.0f, 57.0f, 91.0f);
    while(FirstSibling)
    {
        if(FirstSibling->ID == Menu->Option->ID)
        { 
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - FontSize.Y, (WND_WIDTH*0.5f) - 35, FontSize.Y, 134.0f, 165.0f, 217.0f);
        }
        RenderString(GameState, FirstSibling->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        FirstSibling = FirstSibling->NextSibling;
    }

    XPos = -(WND_WIDTH*0.5f) + 10;
    YPos = (WND_HEIGHT*0.5f) - 10;
    RenderUIQuad(GameState, XPos, YPos - (FontSize.Y*9), (WND_WIDTH*0.5f) - 15, FontSize.Y*9, 35.0f, 57.0f, 91.0f);
    for(i32 Index = 0;
        Index < GameState->HeroPartyCount;
        ++Index)
    {
        if(Menu->Hero == Index)
        {
            RenderUIQuad(GameState, XPos + 10, YPos - 10 - (FontSize.Y*2), (WND_WIDTH*0.5f) - 35, FontSize.Y*2, 134.0f, 165.0f, 217.0f);
        }
        entity *Hero = &GameState->Entities[Index];
        RenderString(GameState, Hero->Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        YPos -= FontSize.Y;
        XPos += FontSize.X; 
        if(Hero->Weapon >= 0)
        {
            RenderString(GameState, GameState->Weapons[Hero->Weapon].Name, XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        }
        else
        {
            RenderString(GameState, "ERROR", XPos + 10, YPos - 10 - FontSize.Y, FontSize.X, FontSize.Y);
        }
        YPos -= FontSize.Y;
        XPos -= FontSize.X; 
    }
}

void UpdateAndRenderMenu(game_state *GameState, input *Input)
{
    menu *Menu = &GameState->Menu;
    if(Menu->SelectingHero)
    {
        GetMenuHeroInput(Menu, Input, GameState->HeroPartyCount);
    }
    else
    {
        GetMenuOptionInput(GameState, Menu, Input);
    }

    if(Menu->Option->Parent && Menu->HeroSelected)
    {
        entity *Hero = &GameState->Entities[Menu->Hero];
        switch(Menu->Option->Parent->ID)
        {
            case OP_INVENTORY:
            {
                if(GameState->Inventory.Items[Menu->Option->Value].Count != 0)
                {
                    item_stats Item = GameState->Items[GameState->Inventory.Items[Menu->Option->Value].ID - 1];
                    Hero->Stats.HP_Now += Item.HP_Modifire;
                    Hero->Stats.MP_Now += Item.MP_Modifire;
                    --GameState->Inventory.Items[Menu->Option->Value].Count;
                }
            }break;
            case OP_SPELLS:
            {
                Hero->Spells[2] = Hero->Spells[1]; 
                Hero->Spells[1] = Hero->Spells[0]; 
                Hero->Spells[0] = Menu->Option->Value;
            }break;
            case OP_WEAPON:
                Hero->Weapon = Menu->Option->Value;
            {}break;
            default:
            {}break;
        };
        Menu->HeroSelected = false;
    }

    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f)); 
    RenderUIQuad(GameState, -(WND_WIDTH*0.5f), -(WND_HEIGHT*0.5f), WND_WIDTH, WND_HEIGHT, 22.0f, 25.0f, 37.0f);
    
    tree::node *FirstSibling = Menu->Option;
    while(FirstSibling->BackSibling)
    {
        FirstSibling = FirstSibling->BackSibling;
    }
    if(Menu->Option->Parent)
    {
        switch(Menu->Option->Parent->ID)
        {
            case OP_MENU:
            {
                RenderMainMenu(GameState, Menu, FirstSibling);
            }break;
            case OP_INVENTORY:
            {
                RenderInventoryMenu(GameState, Menu, FirstSibling);
            }break;
            case OP_SPELLS:
            {
                RenderSpellsMenu(GameState, Menu, FirstSibling);
            }break;
            case OP_WEAPON:
            {
                RenderWeaponMenu(GameState, Menu, FirstSibling);
            }break;
        };
    }
}
