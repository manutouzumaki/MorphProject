#ifndef COMBAT_H
#define COMBAT_H

#define ATTACK 1
#define SPELLS 2
#define ITEMS 3

struct combat
{
    entity Entities[8];
    i32 NumberOfEntities;
    i32 NumberOfEnemiesKill;
    i32 NumberOfHerosKill;
    
    u64 EntitiesEventQueue;
    u64 ActionsEventQueue;
    u32 EventData; 

    i32 OptionSelected;
    i32 SelectedOptions[3];
    i32 NumberOfOptions;
    
    r32 Timer;
    r32 AnimTimer;
   
    bool PlayerTurnFinish;
    bool EnemyTurnFinish;
    bool ProcessingEvent;
};

#endif
