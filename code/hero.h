#ifndef HERO_H
#define HERO_H

// defines for the hero facing variable
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_NUM_OF_ENTITIES 20;

struct weapon_stats
{
    u32 AttackType;
    u32 AttackPower;
    u32 DefensePower;
    u32 AttackRate;
};

struct entity_stats
{
    u32 HP_Now; // Healt Points
    u32 HP_Max;
    u32 MP_Now; // Magic Points
    u32 MP_Max;
    u32 Strength;
    u32 Speed;
    u32 Intelligence; 
};

struct entity
{   
    // Sate stuff
    u32 ID;
    i32 Action;
    // game play stuff
    char *Name;
    entity_stats Stats;
    u32 Weapon;
    u32 Ability0;
    u32 Ability1;
    u32 Ability2;
    u32 Stage;

    // renderer stuff
    u32 Skin;
    u32 Frame;
    u32 Facing;
    u32 Layer;
    v2 Position;
    v2 OldPosiotion;
    v2 NextPosition;
    r32 WalkDistance;
    r32 AnimTimer;
    r32 TimeToWait;
    r32 WaitTime;
    bool IsWalking;
};

#endif
