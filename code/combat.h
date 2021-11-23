#ifndef COMBAT_H
#define COMBAT_H

#define ATTACK 1
#define SPELLS 2
#define ITEMS 3

struct combat
{
    entity Entities[8];
    
    u64 EntitiesEventQueue;
    u64 ActionsEventQueue;
    u32 EventData;
    
    i32 ActualOption;
    i32 OptionSelected;
    i32 OptionLevel;
    i32 NumberOfOptions;
    
    r32 Timer;
    r32 AnimTimer;
   
    bool PlayerTurnFinish;
    bool EnemyTurnFinish;
    bool ProcessingEvent;
};

#endif
