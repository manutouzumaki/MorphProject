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
};

#endif
