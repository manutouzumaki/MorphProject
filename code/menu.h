#ifndef MENU_H
#define MENU_H

enum option
{
    OP_MENU,
    OP_INVENTORY,
    OP_SPELLS,
    OP_WEAPON,
    OP_SAVE,
    OP_EXIT
};
 

struct menu
{
    tree Tree;
    tree::node *Option;

    i32 Hero;
    bool SelectingHero;
};

#endif
