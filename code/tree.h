#ifndef TREE_H
#define TREE_H

struct tree
{
    struct node
    {
        // menu cell info 
        char *Name;
        i32 Value;
        bool Disabled = false;
        bool Dead = false;
        // data structure info
        i32 ID;
        node *Parent;
        node *Child;
        node *NextSibling;
        node *BackSibling;
        void AddChild(i32 Value, char *Name, i32 ID, arena *Arena);
    };
    node *First;
    arena *Arena;
    i32 Size; 
    void Init(arena *Arena);
    i32 AddChildByID(i32 Value, char *Name, i32 ID);
    bool IsEmpty();
    node *FindNodeByID(i32 ID);
};


#endif
