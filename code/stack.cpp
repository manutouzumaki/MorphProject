void stack::Init(arena *Arena)
{
    this->Arena = Arena;
    this->Arena->Used = 0;
    First = 0;
    Size = 0;
}

bool stack::IsEmpty()
{
    return (First == 0);
}

void stack::Push(int Value)
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

i32 stack::Pop()
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

i32 stack::GetValueByIndex(i32 Index)
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

void stack::Clear()
{
    while(!IsEmpty())
    {
        Pop();
    }
}

