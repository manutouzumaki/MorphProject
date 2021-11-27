void InitInventory(inventory *Inventory)
{
    Inventory->ItemsCount = 0;
    Inventory->SelectedOptions[0] = -1;
    Inventory->SelectedOptions[1] = -1;
    Inventory->OptionSelected = 0;
    Inventory->NumberOfOptions = 15;
    for(i32 Index = 0;
        Index < 15;
        ++Index)
    {
        inventory_item ZeroItem = {};
        Inventory->Items[Index] = ZeroItem;
    }
}

void AddItem(inventory *Inventory, i32 ItemID, i32 Count)
{
    bool ItemFound = false;
    for(i32 Index = 0;
        Index < Inventory->ItemsCount;
        ++Index)
    {
        if(ItemID == Inventory->Items[Index].ID)
        {
            Inventory->Items[Index].Count += Count;
            ItemFound = true;
        }
    }
    if(!ItemFound)
    {
        Inventory->Items[Inventory->ItemsCount].ID = ItemID;
        Inventory->Items[Inventory->ItemsCount].Count = Count;
        ++Inventory->ItemsCount;
    }
}



void RenderHeroInfo(game_state *GameState, i32 *X, i32 *Y, i32 YOffset, entity *Hero, i32 Index)
{
    i32 XPos = *X - WND_WIDTH*0.5f;
    i32 YPos = *Y - WND_HEIGHT*0.5f;
    i32 Width = (WND_WIDTH*0.5f)*0.8f;
    i32 Height = 100;
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f};

    inventory *Inventory = &GameState->Inventory;
    RenderUIQuad(GameState, XPos, YPos, Width, Height, 35.0f, 57.0f, 91.0f);
    switch(Inventory->SelectedOptions[1])
    {
        case -1:
        {
            if((Inventory->SelectedOptions[0] != -1) && (Inventory->OptionSelected == Index))
            {
                RenderUIQuad(GameState, XPos, YPos, Width, Height, 134.0f, 165.0f, 217.0f);
            }
        }break;
        default:
        {
            if(Inventory->SelectedOptions[1] == Index)
            {
                RenderUIQuad(GameState, XPos, YPos, Width, Height, 134.0f, 165.0f, 217.0f);    
            }
        }break;
    }



    RenderString(GameState, Hero->Name, (XPos+FontSize.X), YPos + 50 - 9, FontSize.X, FontSize.Y);
    
    YPos += 50;
    XPos += Width*0.5f;
    i32 XOffset = RenderString(GameState, "HP: ", XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    XOffset = RenderUInt(GameState, Hero->Stats.HP_Now, XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    XOffset = RenderString(GameState, "/", XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    RenderUInt(GameState, Hero->Stats.HP_Max, XPos, YPos, FontSize.X, FontSize.Y);
    
    YPos -= FontSize.Y;
    XPos = *X - WND_WIDTH*0.5f;
    XPos += Width*0.5f;
    XOffset = RenderString(GameState, "MP: ", XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    XOffset = RenderUInt(GameState, Hero->Stats.MP_Now, XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    XOffset = RenderString(GameState, "/", XPos, YPos, FontSize.X, FontSize.Y);
    XPos += XOffset*FontSize.X;
    RenderUInt(GameState, Hero->Stats.MP_Max, XPos, YPos, FontSize.X, FontSize.Y);

    *Y -= Height + YOffset;
}

void RenderInventoryItem(game_state *GameState, i32 *X, i32 *Y, inventory_item *Item, i32 Index)
{
    i32 XPos = *X - WND_WIDTH*0.5f;
    i32 YPos = *Y - WND_HEIGHT*0.5f;
    i32 Width = (WND_WIDTH*0.5f)*0.8f;
    i32 Height = 28;
    v2 FontSize = {7.0f*2.0f, 9.0f*2.0f};
    
    inventory *Inventory = &GameState->Inventory;    
    RenderUIQuad(GameState, XPos, YPos, Width, Height, 35.0f, 57.0f, 91.0f);
    switch(Inventory->SelectedOptions[0])
    {
        case -1:
        {
            if(Inventory->OptionSelected == Index)
            {
                RenderUIQuad(GameState, XPos, YPos, Width, Height, 134.0f, 165.0f, 217.0f);
            }
        }break;
        default:
        {
            if(Inventory->SelectedOptions[0] == Index)
            {
                RenderUIQuad(GameState, XPos, YPos, Width, Height, 134.0f, 165.0f, 217.0f);    
            }
        }break;
    }


    if(Item->ID > 0)
    {
        item_stats ItemStats = GameState->Items[Item->ID - 1];
        RenderString(GameState, ItemStats.Name, (XPos+FontSize.X), YPos + 14 - 9, FontSize.X, FontSize.Y);
        RenderUInt(GameState, Item->Count, (XPos + Width - (FontSize.X*2.0f)), YPos + 14 - 9, FontSize.X, FontSize.Y);
    }
    *Y -= Height + 1;
}

void InventoryIntput(input *Input, inventory *Inventory)
{
    if(OnKeyDown(Input->Buttons->Up))
    {
        --Inventory->OptionSelected;
    }
    if(OnKeyDown(Input->Buttons->Down))
    {
        ++Inventory->OptionSelected;
    }
    if(OnKeyDown(Input->Buttons->Start))
    {  
        for(i32 Index = 0;
            Index < 2;
            ++Index)
        {
            if(Inventory->SelectedOptions[Index] == -1)
            {
                Inventory->SelectedOptions[Index] = Inventory->OptionSelected;
                break;
            }
        }
    }
    if(OnKeyDown(Input->Buttons->Back))
    {
        Inventory->NumberOfOptions = 15;
        Inventory->OptionSelected = 0;
        Inventory->SelectedOptions[0] = -1;
        Inventory->SelectedOptions[1] = -1;
    }

    if(Inventory->OptionSelected < 0)
    {
        Inventory->OptionSelected = 0;    
    }
    if(Inventory->OptionSelected > Inventory->NumberOfOptions - 1)
    {
        Inventory->OptionSelected = Inventory->NumberOfOptions - 1;
    }
}

void UpdateAndRenderInventory(game_state *GameState, input *Input)
{

    InventoryIntput(Input, &GameState->Inventory);
    inventory *Inventory = &GameState->Inventory;    
    if(Inventory->SelectedOptions[0] != -1 && Inventory->NumberOfOptions == 15)
    {
        Inventory->OptionSelected = 0;
        Inventory->NumberOfOptions = 4;
    }


    RenderUIQuad(GameState, -(WND_WIDTH*0.5f), -(WND_HEIGHT*0.5f), WND_WIDTH, WND_HEIGHT, 22.0f, 25.0f, 37.0f);
    i32 XPos = (WND_WIDTH*0.5f)*0.1f;
    i32 YPos = ((WND_HEIGHT*0.5f) - 100.0f) + (430.0f*0.5f);
    for(i32 Index = 0;
        Index < 4;//GameState->HeroPartyCount;
        ++Index)
    {
        RenderHeroInfo(GameState, &XPos, &YPos, 10, &GameState->Entities[Index], Index);
    }

    XPos += WND_WIDTH*0.5f;
    YPos = ((WND_HEIGHT*0.5f) - 28.0f) + (430.0f*0.5f);
    for(i32 Index = 0;
        Index < 15;
        ++Index)
    {
        RenderInventoryItem(GameState, &XPos, &YPos, &GameState->Inventory.Items[Index], Index);
    }
}
