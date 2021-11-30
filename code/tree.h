#ifndef TREE_H
#define TREE_H

struct tree
{
    struct node
    {
        // menu cell info 
        char *Name;
        i32 Value;
        // data structure info
        i32 ID;
        node *Parent;
        node *Child;
        node *NextSibling;
        node *BackSibling;
        void AddChild(i32 Value, i32 ID, arena *Arena);
    };
    node *First;
    node *Data;
    arena *Arena;
    i32 Size; 
    void Init(arena *Arena);
    i32 AddChildByID(i32 First, i32 ID);
    bool IsEmpty();

private:
    node *FindNodeByID(node *Actual, i32 ID);
};


#endif
