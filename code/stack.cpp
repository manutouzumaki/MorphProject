void i32_ll_stack::Init(arena *Arena)
{
    this->Arena = Arena;
    First = 0;
    Size = 0;
}

bool i32_ll_stack::IsEmpty()
{
    return (First == 0);
}

void i32_ll_stack::Push(int Value)
{
    if(First == 0)
    {
        First = PushStruct(Arena, node);
        First->Value = Value;
        First->Next = 0;
    }
    else
    {
        node *OldFirst = First;
        First = PushStruct(Arena, node);
        First->Value = Value;
        First->Next = OldFirst;
    }
    ++Size;
}

i32 i32_ll_stack::Pop()
{
    if(IsEmpty())
    {
        return 0;
    }
    i32 Result = First->Value;
    node *Next = First->Next;
    Arena->Used -= sizeof(node);
    First = 0;
    First = Next;
    --Size;
    return Result;
}

i32 i32_ll_stack::GetValueByIndex(i32 Index)
{
    if((Index > (Size - 1)) || Index < 0)
    {
        return 0;
    }
    i32 TimesToLoop = (Size - 1) - Index;
    node *Actual = First;
    while(TimesToLoop > 0)
    {
        Actual = Actual->Next;
        --TimesToLoop;
    }
    return Actual->Value;
}

void i32_ll_stack::Clear()
{
    while(!IsEmpty())
    {
        Pop();
    }
}

