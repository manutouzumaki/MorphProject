#ifndef COMBAT_H
#define COMBAT_H

#define ATTACK 1
#define SPELLS 2
#define ITEMS 3
#define RUN 4

struct combat
{
    entity Players[4];
    entity Enemies[4];
    i32 NumberOfHeros;
    i32 NumberOfEnemies;
    i32 NumberOfEnemiesKill;
    i32 NumberOfHerosKill;
    
    u64 EntitiesEventQueue;
    u64 ActionsEventQueue;
    u32 EventData; 

    i32 HeroEventSet;
    
    tree Tree;
    tree::node *Node;
    tree::node *NodeSelected;
    bool SelectingTarget;
    i32 Target;

    r32 Timer;
    r32 AnimTimer;
   
    bool PlayerTurnFinish;
    bool EnemyTurnFinish;
    bool ProcessingEvent;
};

#endif
