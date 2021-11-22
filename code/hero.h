#ifndef HERO_H
#define HERO_H

// defines for the hero facing variable
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_NUM_OF_ENTITIES 20;

enum attack_type
{
    MELE,
    RANGE,
    DEFENSIVE
};

struct item_stats
{
    char *Name;
    i32 HP_Modifire;
    i32 MP_Modifire;
};

struct spells_stats
{
    char *Name;
    i32 MP_Cost;
    i32 Power;
    i32 Defense;
    u32 TextureIndex;
};

struct weapon_stats
{
    char *Name;
    u32 AttackType;
    u32 AttackPower;
    u32 DefensePower;
    u32 Weight;
};

struct entity_stats
{
    i32 HP_Now; // Healt Points
    i32 HP_Max;
    i32 MP_Now; // Magic Points
    i32 MP_Max;
    i32 Strength;
    i32 Speed;
    i32 Intelligence; 
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
    u32 Spells[3];
    u32 SpellsCount;
    u32 Items[2];
    u32 ItemsCount;
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
