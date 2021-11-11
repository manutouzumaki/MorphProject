#ifndef HERO_H
#define HERO_H

// defines for the hero facing variable
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

struct entity
{
    u32 ID;
    u32 Frame;
    u32 Facing;
    bool IsWalking;
    v2 Position;
    v2 OldPosiotion;
    v2 NextPosition;
};

#endif
