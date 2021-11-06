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

void *PushData(arena *Arena, size_t Size);
void InitArena(memory *Memory, arena *Arena, size_t Size);
void ClearArena(arena *Arena);

#define PushStruct(Arena, Type) (Type *)PushData(Arena, sizeof(Type))
#define PushArray(Arena, Type, Count) (Type *)PushData(Arena, sizeof(Type)*(Count))

#endif
