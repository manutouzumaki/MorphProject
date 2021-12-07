void InitInventory(inventory *Inventory)
{
    Inventory->ItemsCount = 0;
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

void UpdateItems(inventory *Inventory)
{
    i32 ItemsDeleted = 0;
    for(i32 Index = 0;
        Index < Inventory->ItemsCount;
        ++Index)
    {
        while(Inventory->Items[Index].Count == 0 && Inventory->Items[Index].ID != 0)
        {
            ++ItemsDeleted;
            i32 I = Index;
            while(I < Inventory->ItemsCount)
            {
                if(I < Inventory->ItemsCount - 1)
                {
                    Inventory->Items[I] = Inventory->Items[I + 1];
                }
                else
                {
                    inventory_item ZeroItem = {};
                    Inventory->Items[I] = ZeroItem; 
                }
                ++I;
            } 
        }
    }
    Inventory->ItemsCount -= ItemsDeleted;
}
