#ifndef INVENTORY_H
#define INVENTORY_H

struct inventory_item
{
    u32 Count;
    u32 ID;
};

struct inventory
{
    inventory_item Items[15];
    i32 ItemsCount;
    // TODO(manuto): Add more option for inventory managmen
    i32 OptionSelected;
    i32 NumberOfOptions;
    i32 SelectedOptions[2]; 
};

#endif
