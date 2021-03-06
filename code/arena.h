#ifndef ARENA_H
#define ARENA_H

struct memory
{
    void *Data;
    size_t Size;
    size_t Used;
};

struct arena
{
    char *Base;
    size_t Size;
    size_t Used;
};

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
    // Clear the memory to zero
    for(i32 Index = 0;
        Index < Arena->Size;
        ++Index)
    {
        Arena->Base[Index] = 0;
    }
}

void ClearArena(arena *Arena)
{
    // Clear the memory to zero
    for(i32 Index = 0;
        Index < Arena->Size;
        ++Index)
    {
        Arena->Base[Index] = 0;
    }
    Arena->Used = 0;
}

#define PushStruct(Arena, Type) (Type *)PushData(Arena, sizeof(Type))
#define PushArray(Arena, Type, Count) (Type *)PushData(Arena, sizeof(Type)*(Count))

#endif
