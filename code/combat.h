#ifndef COMBAT_H
#define COMBAT_H

#define ATTACK 1
#define SPELLS 2
#define ITEMS 3
#define RUN 4

#define HEROES 1
#define ENEMIES 2

#define SELECT_HERO 0
#define SELECT_TARGET 1

struct combat
{
    entity Players[4];
    entity Enemies[4];
    i32 NumberOfHeros;
    i32 NumberOfEnemies;
    i32 NumberOfEnemiesKill;
    i32 NumberOfHerosKill;
    i32 NumberOfHeroSet;
    i32 FirstHeroID;
    i32 FirstEnemyID;
    
    u64 EntitiesEventQueue;
    u64 ActionsEventQueue;
    u32 EventData; 
 
    tree Tree;
    tree::node *Node;
    tree::node *Action;
    
    tree EntityTree;
    tree::node *EntityNode;
    tree::node *Hero;
    tree::node *Target_;

    r32 Timer;
    r32 AnimTimer;
   
    bool EnemiesActionSet;
    bool ProcessingEvent;
    bool SelectingAction;
};

#endif
