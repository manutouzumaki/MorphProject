char *Dialogues[] = {
    "...",
    "Hola Mi nombre es Hailee\nEste es el pueblo donde naci, Illiandore el pueblo donde los ricos se hacen mas ricos y los pobres mas pobres\nTu eres el Heroe Manuto?\na que si verdad\nbueno bienvenido a Illiandore",
    "Sal de aqui forastero!!!"
};

void InitDialogue(dialoque_state *Dialogue)
{
    Dialogue->Offset = 0;
    Dialogue->Counter = 0;
    Dialogue->NumbLetterToDraw = 0;
    Dialogue->TimeToWait = 0.08f;
    Dialogue->Timer = 0.0f;
    Dialogue->Finish = false;
    Dialogue->Writing = true;

}

void GetDialogueInput(game_state *GameState, input *Input, dialogue_action *DialogueAction)
{
    dialoque_state *Dialogue = &GameState->Dialogue;
   
    
    if(Dialogue->Writing && Input->Buttons->Start.IsDown)
    {
        Dialogue->TimeToWait = 0.02f;
    }
    else
    {
        Dialogue->TimeToWait = 0.08f; 
    }

    if(OnKeyDown(Input->Buttons->Start))
    {
        if(!Dialogue->Writing)
        {
            if(Dialogue->Finish)
            {
                DialogueAction->ShowingDialogue = false;
            }
            else 
            {
                ++Dialogue->Counter;
                Dialogue->Offset += Dialogue->Counter;
                Dialogue->NumbLetterToDraw = 0;
                Dialogue->Writing = true;
            }
        }
    }
}

void UpdateAndRenderDialogue(game_state *GameState, entity *Entity, r32 DeltaTime)
{
    // Update ....
    dialoque_state *Dialogue = &GameState->Dialogue;
    i32 Index = Entity->Dialogue; 
    
    char *Letter = Dialogues[Index];
    Letter += Dialogue->Offset;
    Dialogue->Counter = 0;
    while(*Letter)
    {
        if(*Letter == '\n')
        {
            break;
        }
        else
        {
            ++Dialogue->Counter;
        }
        ++Letter;
    }
    
    Letter = Dialogues[Index];
    Letter += Dialogue->Offset;
    if(*(Letter + Dialogue->Counter) == '\0')
    {
        Dialogue->Finish = true;
    } 
    
    if(Dialogue->Writing)
    { 
        if(Dialogue->Timer > Dialogue->TimeToWait)
        {
            ++Dialogue->NumbLetterToDraw;
            Dialogue->Timer = 0;
        }
        if(Dialogue->NumbLetterToDraw >= Dialogue->Counter)
        {
            Dialogue->Writing = false;
        }
        Dialogue->Timer += DeltaTime;
    }

    // Render ....
    SetProjMat4(GameState, OrthogonalProjMat4(WND_WIDTH, WND_HEIGHT, 1.0f, 100.0f)); 
    i32 Height = WND_HEIGHT * 0.25f;
    i32 Width = WND_WIDTH;
    RenderUIQuad(GameState, -(WND_WIDTH*0.5f), -(WND_HEIGHT*0.5f), Width, Height, 22.0f, 25.0f, 37.0f);
    i32 XPos = -(WND_WIDTH*0.5f);
    i32 YPos = -(WND_HEIGHT*0.5f) + Height - 9.0f*2.0f;
    i32 XOffset = 0;
    for(u32 LetterIndex = 0;
            LetterIndex < Dialogue->NumbLetterToDraw;
            ++LetterIndex)
    {
        if(XOffset >= (Width - 7.0f*2.0f))
        {
            XOffset = 0;
            XPos = -(WND_WIDTH*0.5f);
            YPos -= 9.0f*2.0f;
        }
        RenderLetter(GameState, &Letter[LetterIndex], XPos, YPos, 7.0f*2.0f, 9.0f*2.0f);
        XPos += 7.0f*2.0f;
        XOffset += 7.0f*2.0f;
    }

}
