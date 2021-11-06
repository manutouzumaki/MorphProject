// here we are going to create the function for the arenas "memory manager" ;)
void *PushData(arena *Arena, size_t Size)
{
    Assert(Arena->Used + Size <= Arena->Size);
    void *Result = (void *)(Arena->Base + Arena->Used);
    Arena->Used += Size;
    return Result;
}

void InitArena(memory *Memory, arena *Arena, size_t Size)
{
    Assert(Memory->Used + Size <= Memory->Size);
    Arena->Base = (char *)Memory->Data + Memory->Used;
    Arena->Size = Size;
    Memory->Used += Size;
}

void ClearArena(arena *Arena)
{
    Arena->Used = 0;
}
