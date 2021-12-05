#ifndef DIALOGUE_H
#define DIALOGUE_H

struct dialoque_state
{
    u32 Offset;
    u32 Counter;
    u32 NumbLetterToDraw;
    r32 TimeToWait;
    r32 Timer;
    bool Finish;
    bool Writing;
    
};

struct dialogue_action
{
    bool ShowingDialogue;
    entity *Entity;
};

#endif
