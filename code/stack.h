#ifndef STACK_H
#define STACK_H

struct i32_ll_stack
{
    struct node
    {
        i32 Value;
        node* Next;
    };
    node *First;
    i32 Size;
    arena *Arena;
    void Init(arena *Arena);
    bool IsEmpty();
    void Push(int Value);
    i32 Pop();
    i32 GetValueByIndex(i32 Index);
    void Clear();
};

#endif
